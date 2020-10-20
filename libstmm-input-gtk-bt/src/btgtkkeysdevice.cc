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
 * File:   btgtkkeysdevice.cc
 */

#include "btgtkkeysdevice.h"

#include "btgtkwindowdata.h"
#include "btgtklistenerextradata.h"

#include <stmm-input-ev/keyevent.h>
#include <stmm-input-base/basicdevicemanager.h>
#include <stmm-input/devicemanager.h>
#include <stmm-input/event.h>

#include <cstdint>
#include <limits>
#include <utility>
#include <cassert>

namespace stmi { class GtkAccessor; }

namespace stmi
{

namespace Private
{
namespace Bt
{

BtKeysDevice::BtKeysDevice(const std::string& sName, const shared_ptr<BtGtkDeviceManager>& refDeviceManager) noexcept
: BasicDevice<BtGtkDeviceManager>(sName, refDeviceManager)
{
}
shared_ptr<Device> BtKeysDevice::getDevice() const noexcept
{
	shared_ptr<const BtKeysDevice> refConstThis = shared_from_this();
	shared_ptr<BtKeysDevice> refThis = std::const_pointer_cast<BtKeysDevice>(refConstThis);
	return refThis;
}
shared_ptr<Capability> BtKeysDevice::getCapability(const Capability::Class& oClass) const noexcept
{
	shared_ptr<Capability> refCapa;
	if (oClass == typeid(KeyCapability)) {
		shared_ptr<const BtKeysDevice> refConstThis = shared_from_this();
		shared_ptr<BtKeysDevice> refThis = std::const_pointer_cast<BtKeysDevice>(refConstThis);
		refCapa = refThis;
	}
	return refCapa;
}
shared_ptr<Capability> BtKeysDevice::getCapability(int32_t nCapabilityId) const noexcept
{
	const auto nKeysCapaId = KeyCapability::getId();
	if (nCapabilityId != nKeysCapaId) {
		return shared_ptr<Capability>{};
	}
	shared_ptr<const BtKeysDevice> refConstThis = shared_from_this();
	shared_ptr<BtKeysDevice> refThis = std::const_pointer_cast<BtKeysDevice>(refConstThis);
	return refThis;
}
std::vector<int32_t> BtKeysDevice::getCapabilities() const noexcept
{
	return {KeyCapability::getId()};
}
std::vector<Capability::Class> BtKeysDevice::getCapabilityClasses() const noexcept
{
	return {KeyCapability::getClass()};
}
bool BtKeysDevice::onBlueKey(KeyEvent::KEY_INPUT_TYPE eInputType, HARDWARE_KEY eHardwareKey
							, const shared_ptr<GtkWindowData>& refWindowData) noexcept
{
	const bool bContinue = true;
	auto refOwner = getOwnerDeviceManager();
	if (!refOwner) {
		return !bContinue; //---------------------------------------------------
	}
	//
	BtGtkDeviceManager* p0Owner = refOwner.get();
	auto refListeners = p0Owner->getListeners();
	uint64_t nPressedTimeStamp = std::numeric_limits<uint64_t>::max();
	auto itFind = m_oPressedKeys.find(eHardwareKey);
	const bool bHardwareKeyPressed = (itFind != m_oPressedKeys.end());
	shared_ptr<BtKeysDevice> refKeysDevice = shared_from_this();
	shared_ptr<KeyCapability> refCapability = refKeysDevice;
	auto refWindowAccessor = refWindowData->getAccessor();
	assert(refWindowAccessor);
	if (eInputType == KeyEvent::KEY_PRESS) {
		if (bHardwareKeyPressed) {
			// Key repeat: suppressed
			return bContinue; //------------------------------------------------
		}
		nPressedTimeStamp = BtGtkDeviceManager::getUniqueTimeStamp();
		KeyData oKeyData;
		oKeyData.m_nPressedTimeStamp = nPressedTimeStamp;
		m_oPressedKeys.emplace(eHardwareKey, oKeyData);
	} else {
		if (!bHardwareKeyPressed) {
			// orphan release or release_cancel , ignore
//std::cout << "BtKeysDevice::onBlueKey orphan release" << '\n';
			return bContinue; //------------------------------------------------
		}
		KeyData& oKeyData = itFind->second;
		nPressedTimeStamp = oKeyData.m_nPressedTimeStamp;
		m_oPressedKeys.erase(itFind);
	}
	shared_ptr<Event> refEvent;
	const int64_t nEventTimeUsec = DeviceManager::getNowTimeMicroseconds();
	for (auto& p0ListenerData : *refListeners) {
		sendKeyEventToListener(*p0ListenerData, nEventTimeUsec, nPressedTimeStamp, eInputType, eHardwareKey
								, refWindowAccessor, refCapability, p0Owner->m_nClassIdxKeyEvent, refEvent);
		if ((eInputType == KeyEvent::KEY_PRESS) && (m_oPressedKeys.find(eHardwareKey) == m_oPressedKeys.end())) {
			// The key was canceled by the callback
			break; // for -------
		}
	}
	return bContinue;
}
void BtKeysDevice::finalizeListener(BtGtkDeviceManager::ListenerData& oListenerData, int64_t nEventTimeUsec) noexcept
{
	auto refOwner = getOwnerDeviceManager();
	if (!refOwner) {
		return;
	}
	BtGtkDeviceManager* p0Owner = refOwner.get();
	if (!p0Owner->isEventClassEnabled(Event::Class{typeid(KeyEvent)})) {
		return; //--------------------------------------------------------------
	}
	auto& refSelected = p0Owner->m_refSelected;
	if (!refSelected) {
		return; //--------------------------------------------------------------
	}
	auto refSelectedAccessor = refSelected->getAccessor();
	shared_ptr<BtKeysDevice> refThis = shared_from_this();
	shared_ptr<KeyCapability> refCapability = refThis;

	BtGtkListenerExtraData* p0ExtraData = nullptr;
	oListenerData.getExtraData(p0ExtraData);

	// Send KEY_RELEASE_CANCEL for the currently pressed keys to the listener
	// work on copy
	auto oPressedKeys = m_oPressedKeys;
	for (auto& oPair : oPressedKeys) {
		const int32_t nHardwareKey = oPair.first;
		const KeyData& oKeyData = oPair.second;
		//
		const auto nKeyPressedTimeStamp = oKeyData.m_nPressedTimeStamp;
		const HARDWARE_KEY eHardwareKey = static_cast<HARDWARE_KEY>(nHardwareKey);
		//
		if (p0ExtraData->isKeyCanceled(nHardwareKey)) {
			continue; // for ------------
		}
		p0ExtraData->setKeyCanceled(nHardwareKey);
		//
		shared_ptr<Event> refEvent;
		sendKeyEventToListener(oListenerData, nEventTimeUsec, nKeyPressedTimeStamp, KeyEvent::KEY_RELEASE_CANCEL
								, eHardwareKey, refSelectedAccessor, refCapability, p0Owner->m_nClassIdxKeyEvent, refEvent);
		if (!refSelected) {
			// There can't be pressed keys without an active window
			break; // for -----------
		}
	}
}
void BtKeysDevice::removingDevice() noexcept
{
	resetOwnerDeviceManager();
}
void BtKeysDevice::cancelSelectedAccessorKeys() noexcept
{
	auto refOwner = getOwnerDeviceManager();
	if (!refOwner) {
		return;
	}
	BtGtkDeviceManager* p0Owner = refOwner.get();
	if (!p0Owner->isEventClassEnabled(Event::Class{typeid(KeyEvent)})) {
		return; // -------------------------------------------------------------
	}
	auto& refSelected = p0Owner->m_refSelected;
	if (!refSelected) {
		return; // -------------------------------------------------------------
	}
	auto refSelectedAccessor = refSelected->getAccessor();

	auto refListeners = p0Owner->getListeners();
	//
	shared_ptr<BtKeysDevice> refThis = shared_from_this();
	shared_ptr<KeyCapability> refCapability = refThis;
	// Remove all keys generated by the accessor (widget)
	const int64_t nEventTimeUsec = DeviceManager::getNowTimeMicroseconds();
	// work on copy
	auto oPressedKeys = m_oPressedKeys;
	for (auto& oPair : oPressedKeys) {
		const int32_t nHardwareKey = oPair.first;
		const KeyData& oKeyData = oPair.second;
		//
		const HARDWARE_KEY eHardwareKey = static_cast<HARDWARE_KEY>(nHardwareKey);
		const auto nKeyPressedTimeStamp = oKeyData.m_nPressedTimeStamp;
		//
		shared_ptr<Event> refEvent;
		for (auto& p0ListenerData : *refListeners) {
			BtGtkListenerExtraData* p0ExtraData = nullptr;
			p0ListenerData->getExtraData(p0ExtraData);
			if (p0ExtraData->isKeyCanceled(nHardwareKey)) {
				continue; // for itListenerData ------------
			}
			p0ExtraData->setKeyCanceled(nHardwareKey);
			//
			sendKeyEventToListener(*p0ListenerData, nEventTimeUsec, nKeyPressedTimeStamp, KeyEvent::KEY_RELEASE_CANCEL
									, eHardwareKey, refSelectedAccessor, refCapability, p0Owner->m_nClassIdxKeyEvent, refEvent);
		}
	}
	m_oPressedKeys.clear();
}
void BtKeysDevice::sendKeyEventToListener(const BtGtkDeviceManager::ListenerData& oListenerData, int64_t nEventTimeUsec
												, uint64_t nPressedTimeStamp
												, KeyEvent::KEY_INPUT_TYPE eInputType, HARDWARE_KEY eHardwareKey
												, const shared_ptr<GtkAccessor>& refAccessor
												, const shared_ptr<KeyCapability>& refCapability
												, int32_t nClassIdxKeyEvent
												, shared_ptr<Event>& refEvent) noexcept
{
	const auto nAddTimeStamp = oListenerData.getAddedTimeStamp();
//std::cout << "BtKeysDevice::sendKeyEventToListener nAddTimeUsec=" << nAddTimeUsec;
//std::cout << "  nTimePressedUsec=" << nTimePressedUsec << "  nEventTimeUsec=" << nEventTimeUsec << '\n';
	if (nPressedTimeStamp < nAddTimeStamp) {
		// The listener was added after the key was pressed
		return;
	}
	if (!refEvent) {
		m_oKeyEventRecycler.create(refEvent, nEventTimeUsec, refAccessor, refCapability, eInputType, eHardwareKey);
	}
	oListenerData.handleEventCallIf(nClassIdxKeyEvent, refEvent);
		// no need to reset because KeyEvent cannot be modified.
}


} // namespace Bt
} // namespace Private

} // namespace stmi
