/*
 * Copyright © 2017-2019  Stefano Marsili, <stemars@gmx.ch>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, see <http://www.gnu.org/licenses/>
 */
/*
 * File:   btgtkdevicemanager.cc
 */

#include "btgtkdevicemanager.h"

#include "btgtkkeysdevice.h"
#include "btgtkwindowdata.h"
#include "btgtkbackend.h"

#include <stmm-input-ev/keyevent.h>

#include <stmm-input-ev/devicemgmtevent.h>
#include <stmm-input-ev/keycapability.h>
#include <stmm-input-gtk/gtkaccessor.h>
#include <stmm-input/capability.h>
#include <stmm-input/devicemanager.h>

#include <algorithm>
#include <type_traits>
#include <cassert>

#ifdef STMM_SNAP_PACKAGING
#include <array>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#endif //STMM_SNAP_PACKAGING

namespace stmi { class Accessor; }
namespace stmi { class ChildDeviceManager; }

namespace stmi
{

////////////////////////////////////////////////////////////////////////////////
using std::unique_ptr;
using Private::Bt::GtkBackend;
using Private::Bt::BtKeysDevice;
using Private::Bt::GtkWindowData;
using Private::Bt::GtkWindowDataFactory;
using Private::Bt::BtGtkListenerExtraData;

#ifdef STMM_SNAP_PACKAGING
static std::string getEnvString(const char* p0Name) noexcept
{
	const char* p0Value = ::secure_getenv(p0Name);
	std::string sValue{(p0Value == nullptr) ? "" : p0Value};
	return sValue;
}
static bool execCmd(const char* sCmd, std::string& sResult, std::string& sError) noexcept
{
	::fflush(nullptr);
	sError.clear();
	sResult.clear();
	std::array<char, 128> aBuffer;
	FILE* pFile = ::popen(sCmd, "r");
	if (pFile == nullptr) {
		sError = std::string("Error: popen() failed: ") + ::strerror(errno) + "(" + std::to_string(errno) + ")";
		return false; //--------------------------------------------------------
	}
	while (!::feof(pFile)) {
		if (::fgets(aBuffer.data(), sizeof(aBuffer), pFile) != nullptr) {
			sResult += aBuffer.data();
		}
	}
	const auto nRet = ::pclose(pFile);
	return (nRet == 0);
}
#endif //STMM_SNAP_PACKAGING

std::pair<shared_ptr<BtGtkDeviceManager>, std::string> BtGtkDeviceManager::create(const std::string& sAppName
																				, bool bEnableEventClasses, const std::vector<Event::Class>& aEnDisableEventClasses) noexcept
{
	#ifdef STMM_SNAP_PACKAGING
	{
	std::string sError;
	const std::string sSnapName = getEnvString("SNAP_NAME");
	if (sSnapName.empty()) {
		sError = "SNAP_NAME environment variable not defined!";
		return std::make_pair(shared_ptr<BtGtkDeviceManager>{}, std::move(sError)); //--------
	}
	std::string sResult;
	if (! execCmd("snapctl is-connected bluetooth-control", sResult, sError)) {
		sError = "Not allowed to use bluetooth."
				"\nPlease grant permission with 'sudo snap connect " + sSnapName + ":bluetooth-control :bluetooth-control'";
		return std::make_pair(shared_ptr<BtGtkDeviceManager>{}, std::move(sError)); //--------
	}
	}
	#endif //STMM_SNAP_PACKAGING

	shared_ptr<BtGtkDeviceManager> refInstance(new BtGtkDeviceManager(bEnableEventClasses, aEnDisableEventClasses));
	//
	auto oPairBackend = GtkBackend::create(refInstance.operator->(), sAppName);
	unique_ptr<GtkBackend>& refBackend = oPairBackend.first;
	std::string& sError = oPairBackend.second;
	if (! sError.empty()) {
		return std::make_pair(shared_ptr<BtGtkDeviceManager>{}, std::move(sError));
	}
	auto refFactory = std::make_unique<GtkWindowDataFactory>();
	refInstance->init(refFactory, refBackend);
	return std::make_pair(refInstance, std::move(sError));
}

BtGtkDeviceManager::BtGtkDeviceManager(bool bEnableEventClasses, const std::vector<Event::Class>& aEnDisableEventClasses) noexcept
: StdDeviceManager({Capability::Class{typeid(KeyCapability)}}
					, {Event::Class{typeid(DeviceMgmtEvent)}, Event::Class{typeid(KeyEvent)}}
					, bEnableEventClasses, aEnDisableEventClasses)
, m_nCancelingNestedDepth(0)
, m_nClassIdxKeyEvent(getEventClassIndex(Event::Class{typeid(KeyEvent)}))
{
}
void BtGtkDeviceManager::init(std::unique_ptr<Private::Bt::GtkWindowDataFactory>& refFactory
								, std::unique_ptr<Private::Bt::GtkBackend>& refBackend) noexcept
{
	assert(refFactory);
	assert(refBackend);
	m_refFactory.swap(refFactory);
	m_refBackend.swap(refBackend);
}
void BtGtkDeviceManager::enableEventClass(const Event::Class& oEventClass) noexcept
{
	StdDeviceManager::enableEventClass(oEventClass);
}
void BtGtkDeviceManager::onDeviceAdded(const std::string& sName, int32_t nBackendId) noexcept
{
//std::cout << "BtGtkDeviceManager::onDeviceAdded sName=" << sName << "  nBackendId=" << nBackendId << '\n';
	shared_ptr<ChildDeviceManager> refChildThis = shared_from_this();
	assert(std::dynamic_pointer_cast<BtGtkDeviceManager>(refChildThis));
	auto refThis = std::static_pointer_cast<BtGtkDeviceManager>(refChildThis);
	//
	auto refNewDevice = std::make_shared<BtKeysDevice>(sName, refThis);
	#ifndef NDEBUG
	auto itFind = std::find_if(m_aBtDevices.begin(), m_aBtDevices.end()
					, [&](const shared_ptr<Private::Bt::BtKeysDevice>& refDevice)
					{
						return refDevice && (refDevice->getDeviceId() == refNewDevice->getDeviceId());
					});
	#endif
	assert(itFind == m_aBtDevices.end());
	//
	if (nBackendId >= static_cast<int32_t>(m_aBtDevices.size())) {
		m_aBtDevices.resize(nBackendId + 1);
	}
	assert(! m_aBtDevices[nBackendId]);
	m_aBtDevices[nBackendId] = refNewDevice;
	//
	#ifndef NDEBUG
	const bool bAdded = 
	#endif
	StdDeviceManager::addDevice(refNewDevice);
	assert(bAdded);
	sendDeviceMgmtToListeners(DeviceMgmtEvent::DEVICE_MGMT_ADDED, refNewDevice);
}
void BtGtkDeviceManager::onDeviceRemoved(int32_t nBackendId) noexcept
{
//std::cout << "BtGtkDeviceManager::onDeviceRemoved nBackendId=" << nBackendId << '\n';
	assert((nBackendId >= 0) && (nBackendId < static_cast<int32_t>(m_aBtDevices.size())));
	shared_ptr<BtKeysDevice>& refBtKeysDevice = m_aBtDevices[nBackendId];
	assert(refBtKeysDevice);
	//
	cancelDeviceKeys(refBtKeysDevice);
	refBtKeysDevice->removingDevice();

	auto refRemovedBtKeysDevice = refBtKeysDevice; // copy!
	#ifndef NDEBUG
	const bool bRemoved =
	#endif //NDEBUG
	StdDeviceManager::removeDevice(refBtKeysDevice);
	assert(bRemoved);
	//m_aBtDevices.erase(itFind);
	m_aBtDevices[nBackendId].reset();
	//
	sendDeviceMgmtToListeners(DeviceMgmtEvent::DEVICE_MGMT_REMOVED, refRemovedBtKeysDevice);
}
bool BtGtkDeviceManager::onBlueKey(int32_t nBackendId, KeyEvent::KEY_INPUT_TYPE eType, HARDWARE_KEY eHK) noexcept
{
//std::cout << "BtGtkDeviceManager::onBlueKey eHK=" << static_cast<int32_t>(eHK) << '\n';
	assert((nBackendId >= 0) && (nBackendId < static_cast<int32_t>(m_aBtDevices.size())));
	if (!m_refSelected) {
		// shouldn't happen
		return true; //---------------------------------------------------------
	}
	shared_ptr<BtKeysDevice>& refBtKeysDevice = m_aBtDevices[nBackendId];
	assert(refBtKeysDevice);
	return refBtKeysDevice->onBlueKey(eType, eHK, m_refSelected);
}
void BtGtkDeviceManager::finalizeListener(ListenerData& oListenerData) noexcept
{
	++m_nCancelingNestedDepth;
	const int64_t nEventTimeUsec = DeviceManager::getNowTimeMicroseconds();
	for (auto& refBtKeysDevice : m_aBtDevices) {
		refBtKeysDevice->finalizeListener(oListenerData, nEventTimeUsec);
	}
	--m_nCancelingNestedDepth;
	if (m_nCancelingNestedDepth == 0) {
		resetExtraDataOfAllListeners();
	}
}
bool BtGtkDeviceManager::findWindow(Gtk::Window* p0GtkmmWindow
				, std::vector< std::pair<Gtk::Window*, shared_ptr<GtkWindowData> > >::iterator& itFind) noexcept
{
	for (itFind = m_aGtkWindowData.begin(); itFind != m_aGtkWindowData.end(); ++itFind) {
		if (itFind->first == p0GtkmmWindow) {
			return true;
		}
	}
	return false;
}
bool BtGtkDeviceManager::hasAccessor(const shared_ptr<Accessor>& refAccessor, bool& bValid
				, std::vector< std::pair<Gtk::Window*, shared_ptr<GtkWindowData> > >::iterator& itFind) noexcept
{
	bValid = false;
	if (!refAccessor) {
		return false; //--------------------------------------------------------
	}
	shared_ptr<GtkAccessor> refGtkAccessor = std::dynamic_pointer_cast<GtkAccessor>(refAccessor);
	if (!refGtkAccessor) {
		return false; //--------------------------------------------------------
	}
	bValid = true;
	// Note: might be that refGtkAccessor->isDeleted()
	Gtk::Window* p0GtkmmWindow = refGtkAccessor->getGtkmmWindow();
	const bool bFoundWindow = findWindow(p0GtkmmWindow, itFind);
	return bFoundWindow;
}
bool BtGtkDeviceManager::hasAccessor(const shared_ptr<Accessor>& refAccessor) noexcept
{
	bool bValid;
	std::vector< std::pair<Gtk::Window*, shared_ptr<GtkWindowData> > >::iterator itFind;
	return hasAccessor(refAccessor, bValid, itFind);
}
bool BtGtkDeviceManager::addAccessor(const shared_ptr<Accessor>& refAccessor) noexcept
{
//std::cout << "BtGtkDeviceManager::addAccessor()" << '\n';
	std::vector< std::pair<Gtk::Window*, shared_ptr<GtkWindowData> > >::iterator itFind;
	bool bValid;
	const bool bHasAccessor = hasAccessor(refAccessor, bValid, itFind);
	if (!bValid) {
		return false; //--------------------------------------------------------
	}
	shared_ptr<GtkAccessor> refGtkAccessor = std::static_pointer_cast<GtkAccessor>(refAccessor);
	if (refGtkAccessor->isDeleted()) {
		// the window was already destroyed, unusable
		// but if a zombie GtkAccessorData is still around, remove it
		if (bHasAccessor) {
			removeAccessor(itFind);
		}
		return false; //--------------------------------------------------------
	}
	if (bHasAccessor) {
		// Accessor is already present
		return false; //--------------------------------------------------------
	}
	Gtk::Window* p0GtkmmWindow = refGtkAccessor->getGtkmmWindow();
	m_aGtkWindowData.emplace_back(p0GtkmmWindow, m_refFactory->create()); //getGtkWindowData()
	shared_ptr<GtkWindowData> refData = m_aGtkWindowData.back().second;
	GtkWindowData& oData = *refData;
	oData.enable(refGtkAccessor, this);

	const bool bIsActive = oData.isWindowActive();
	if (!m_refSelected) {
		if (bIsActive) {
			m_refSelected = refData;
		}
	} else {
		if (bIsActive) {
			auto refSelectedAccessor = m_refSelected->getAccessor();
			if (refSelectedAccessor->isDeleted()) {
				Gtk::Window* p0SelectedGtkmmWindow = refSelectedAccessor->getGtkmmWindow();
				std::vector< std::pair<Gtk::Window*, shared_ptr<GtkWindowData> > >::iterator itSelectedFind;
				#ifndef NDEBUG
				const bool bFoundWindow = 
				#endif //NDEBUG
				findWindow(p0SelectedGtkmmWindow, itSelectedFind);
				assert(bFoundWindow);
				removeAccessor(itSelectedFind);
			} else {
				// This shouldn't happen: the added window is active while another is still selected.
				deselectAccessor();
			}
			m_refSelected = refData;
		} else {
			deselectAccessor();
		}
	}
	return true;
}
bool BtGtkDeviceManager::removeAccessor(const shared_ptr<Accessor>& refAccessor) noexcept
{
//std::cout << "BtGtkDeviceManager::removeAccessor()" << '\n';
	std::vector<std::pair<Gtk::Window*, shared_ptr<GtkWindowData> > >::iterator itFind;
	bool bValid;
	const bool bHasAccessor = hasAccessor(refAccessor, bValid, itFind);
	if (!bHasAccessor) {
		return false; //--------------------------------------------------------
	}
//std::cout << "BtGtkDeviceManager::removeAccessor &oWindowData=" << &(itFind->second) << '\n';
	removeAccessor(itFind);
	return true;
}
void BtGtkDeviceManager::removeAccessor(const std::vector<std::pair<Gtk::Window*, shared_ptr<GtkWindowData> > >::iterator& itGtkData) noexcept
{
	assert(itGtkData->second);
	// Note: an additional shared_ptr to the object is created to avoid it
	//       being recycled during deselectAccessor()
	auto refData = itGtkData->second;

	const bool bIsSelected = (m_refSelected == refData);

	refData->disable(); // doesn't clear accessor!
	//
	m_aGtkWindowData.erase(itGtkData);

	if (bIsSelected) {
		deselectAccessor();
	}
}
void BtGtkDeviceManager::selectAccessor(const shared_ptr<GtkWindowData>& refData) noexcept
{
//std::cout << "BtGtkDeviceManager::selectAccessor  accessor=" << (int64_t)&(*(refData->getAccessor())) << '\n';
	m_refSelected = refData;
}
void BtGtkDeviceManager::deselectAccessor() noexcept
{
//std::cout << "BtGtkDeviceManager::deselectAccessor  m_nCancelingNestedDepth=" << m_nCancelingNestedDepth << '\n';
	++m_nCancelingNestedDepth;
	//
	for (auto& refDevice : m_aBtDevices) {
		// cancel all keys that are pressed for the currently selected accessor
		refDevice->cancelSelectedAccessorKeys();
	}
	m_refSelected.reset();
	//
	--m_nCancelingNestedDepth;
	if (m_nCancelingNestedDepth == 0) {
		resetExtraDataOfAllListeners();
	}
}
void BtGtkDeviceManager::cancelDeviceKeys(const shared_ptr<Private::Bt::BtKeysDevice>& refBtKeysDevice) noexcept
{
//std::cout << "BtGtkDeviceManager::cancelDeviceKeys  m_nCancelingNestedDepth=" << m_nCancelingNestedDepth << '\n';
	++m_nCancelingNestedDepth;
	//
	refBtKeysDevice->cancelSelectedAccessorKeys();
	//
	--m_nCancelingNestedDepth;
	if (m_nCancelingNestedDepth == 0) {
		resetExtraDataOfAllListeners();
	}
}
void BtGtkDeviceManager::onIsActiveChanged(const shared_ptr<GtkWindowData>& refWindowData) noexcept
{
//std::cout << "BtGtkDeviceManager::onIsActiveChanged  accessor=" << (int64_t)&(*(refWindowData->getAccessor()));
	if (!refWindowData->isEnabled()) {
//std::cout << '\n';
		return;
	}
	auto& refGtkAccessor = refWindowData->getAccessor();
	bool bIsActive = false;
	if (!refGtkAccessor->isDeleted()) {
		bIsActive = refWindowData->isWindowActive();
	}
//std::cout << " active=" << bIsActive << '\n';
	const bool bIsSelected = (refWindowData == m_refSelected);
	if (bIsActive) {
		if (bIsSelected) {
			// Activating the already active window ... shouldn't happen.
			return; //----------------------------------------------------------
		}
		if (m_refSelected) {
			deselectAccessor();
		}
		selectAccessor(refWindowData);
	} else {
		if (bIsSelected) {
			// Send cancels for open keys
			deselectAccessor();
		}
	}
}

} // namespace stmi
