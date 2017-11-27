/*
 * Copyright © 2017  Stefano Marsili, <stemars@gmx.ch>
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
 * File:   btgtkbackend.cc
 */

#include "btgtkbackend.h"

#include "btgtkdevicemanager.h"
#include "bluetoothsources.h"
#include "btgtkkeysdevice.h"

#include <stmm-input/hardwarekey.h>
#include <stmm-input-ev/keyevent.h>

#include <cassert>
#include <set>

#include <bluetooth/bluetooth.h>

#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>

namespace stmi
{

namespace Private
{
namespace Bt
{

static const int32_t s_nL2capPort = 0x20A1;

unique_ptr<GtkBackend> GtkBackend::create(BtGtkDeviceManager* p0Owner, const std::string& sAppName)
{
	auto refBackend = std::unique_ptr<GtkBackend>(new GtkBackend(p0Owner, sAppName));
	refBackend->initServer();
	return refBackend;
}

GtkBackend::GtkBackend(BtGtkDeviceManager* p0Owner, const std::string& sAppName)
: m_p0Owner(p0Owner)
, m_sAppName(sAppName)
{
	assert(p0Owner != nullptr);
}
GtkBackend::~GtkBackend()
{
	if (m_refServerAccept) {
		m_refServerAccept->destroy();
	}
	if (! m_sAppName.empty()) {
		std::cout << m_sAppName << ": ";
	}
	std::cout << "Bluetooth btkeys server stopped on L2CAP port " << s_nL2capPort << '\n';
}
void GtkBackend::initServer()
{
	//TODO pass -1 and let the bind choose the port
	// then spawn a SDP entry process to publicize the port
	m_refServerAccept = Glib::RefPtr<BlueServerAcceptSource>(new BlueServerAcceptSource(s_nL2capPort));
	if (! m_refServerAccept->getErrorStr().empty()) {
		std::string sError = "Bluetooth error (initServer):\n -> " + m_refServerAccept->getErrorStr();
		throw std::runtime_error(sError); //------------------------------------
	}
	if (! m_sAppName.empty()) {
		std::cout << m_sAppName << ": ";
	}
	std::cout << "Bluetooth btkeys server started on L2CAP port " << s_nL2capPort << '\n';
	m_refServerAccept->connect(sigc::mem_fun(this, &GtkBackend::doServerAcceptClient));
	m_refServerAccept->attach();
}
int32_t GtkBackend::getBackendId(const std::vector<bdaddr_t>& aAddrs, const bdaddr_t& oBdAddr)
{
	auto itFind = std::find_if(aAddrs.begin(), aAddrs.end(), [&](const bdaddr_t& oCurBdAddr)
	{
		return (bacmp(&oCurBdAddr, &oBdAddr) == 0);
	});
	if (itFind == aAddrs.end()) {
		return -1; //-----------------------------------------------------------
	}
	return static_cast<int32_t>(std::distance(aAddrs.begin(), itFind));
}
int32_t GtkBackend::getBackendId(const bdaddr_t& oBdAddr) const
{
	return getBackendId(m_aPermanentAddrs, oBdAddr);
}
bool GtkBackend::doServerAcceptClient(int32_t nClientFD, const struct sockaddr_l2& oClientAddr)
{
	const bdaddr_t& oClientBdAddr = oClientAddr.l2_bdaddr;
	int32_t nBackendId = getBackendId(oClientBdAddr);
	const bool bKnownDevice = (nBackendId >= 0);
	if (! bKnownDevice) {
		bdaddr_t oEmptyBdAddr;
		resetBdAddr(oEmptyBdAddr);
		nBackendId = getBackendId(oEmptyBdAddr);
		if (nBackendId < 0) {
			nBackendId = static_cast<int32_t>(m_aPermanentAddrs.size());
			m_aPermanentAddrs.push_back(oClientBdAddr);
			m_aInputSources.push_back({});
		} else {
			m_aPermanentAddrs[nBackendId] = oClientBdAddr;
			assert(! m_aInputSources[nBackendId]);
		}
	} else {
		auto& refSource = m_aInputSources[nBackendId];
		if (refSource) {
			refSource->destroy();
			refSource.reset();
		}
	}
	auto& refSource = m_aInputSources[nBackendId];
	refSource = Glib::RefPtr<BlueServerReceiveSource>(new BlueServerReceiveSource(nBackendId, nClientFD));
	refSource->connect(sigc::mem_fun(this, &GtkBackend::doServerReceive));
	refSource->attach();

	if (! bKnownDevice) {
		m_p0Owner->onDeviceAdded(getBdAddrAsString(oClientBdAddr), nBackendId);
	}
	return true;
}
bool GtkBackend::doServerReceive(int32_t nBackendId, bool bRemove, const KeyPacket& oPkt)
{
	assert((nBackendId >= 0) && (nBackendId < static_cast<int32_t>(m_aInputSources.size())));
	if (oPkt.m_nCmd == PACKET_CMD_KEY) {
		const auto eType = static_cast<KeyEvent::KEY_INPUT_TYPE>(oPkt.m_nKeyType);
		const auto eHK = static_cast<HARDWARE_KEY>(oPkt.m_nHardwareKey);
		assert(m_p0Owner != nullptr);
		const bool bContinue = m_p0Owner->onBlueKey(nBackendId, eType, eHK);
		return bContinue; //----------------------------------------------------
	}
	m_aInputSources[nBackendId].reset();
	if (bRemove) {
		assert(oPkt.m_nCmd == PACKET_CMD_REMOVE_DEVICE);
		assert((nBackendId >= 0) && (nBackendId < static_cast<int32_t>(m_aPermanentAddrs.size())));
		resetBdAddr(m_aPermanentAddrs[nBackendId]);
		m_p0Owner->onDeviceRemoved(nBackendId);
	}
	return false;
}
void GtkBackend::resetBdAddr(bdaddr_t& oBdAddr)
{
	memset(static_cast<void*>(&oBdAddr), 0, sizeof(bdaddr_t));
}
bool GtkBackend::isEmptyBdAddr(const bdaddr_t& oBdAddr)
{
	bdaddr_t oEmptyBdAddr;
	resetBdAddr(oEmptyBdAddr);
	return (bacmp(&oEmptyBdAddr, &oBdAddr) == 0);
}
std::string GtkBackend::getBdAddrAsString(const bdaddr_t& oBdAddr)
{
	char oAddr[19];
	memset(oAddr, 0, sizeof(oAddr));
	ba2str(&oBdAddr, oAddr);
	return std::string(oAddr);
}

} // namespace Bt
} // namespace Private

} // namespace stmi
