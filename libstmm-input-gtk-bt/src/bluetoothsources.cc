/*
 * Copyright © 2017-2020  Stefano Marsili, <stemars@gmx.ch>
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
 * File:   bluetoothsources.cc
 */

#include "bluetoothsources.h"
#include "keypacket.h"

#include <iostream>
#include <cassert>

#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/socket.h>

namespace stmi
{

namespace Private
{
namespace Bt
{

	//constexpr int32_t s_nMtu = 48; /// Bytes
constexpr char s_nMagic1 = '7';
constexpr char s_nMagic2 = 'A';

BlueServerAcceptSource::BlueServerAcceptSource(int32_t nL2capPort) noexcept
: Glib::Source()
, m_nL2capPort(nL2capPort)
, m_nListenerFD(-1)
{
	static_assert(sizeof(int) <= sizeof(int32_t), "");
	static_assert(false == FALSE, "");
	static_assert(true == TRUE, "");
	//
	m_nListenerFD = ::socket(AF_BLUETOOTH, SOCK_SEQPACKET, BTPROTO_L2CAP);
	//
	if (m_nListenerFD < 0) {
		m_sErrorStr = "BlueServerAcceptSource(): socket failed: " + std::string(strerror(errno));
		return; //--------------------------------------------------------------
	}
	// bind socket to given port of the first available
	// bluetooth adapter
	::sockaddr_l2 oLocalAddr;
	memset(&oLocalAddr, 0, sizeof(oLocalAddr));
	const short nPort = static_cast<short>(m_nL2capPort); //0x20A1
	oLocalAddr.l2_family = AF_BLUETOOTH;
	memset(&(oLocalAddr.l2_bdaddr), 0, sizeof(oLocalAddr.l2_bdaddr)); // set to BDADDR_ANY !!!
	oLocalAddr.l2_psm = htobs(nPort);
	oLocalAddr.l2_cid = 0;
	oLocalAddr.l2_bdaddr_type = 0;
	//
	auto nRes = ::bind(m_nListenerFD, reinterpret_cast<sockaddr*>(&oLocalAddr), sizeof(oLocalAddr));
	//
	if (nRes < 0) {
		m_sErrorStr = "BlueServerAcceptSource(): bind failed: " + std::string(strerror(errno));
		close(m_nListenerFD);
		m_nListenerFD = -1;
		return; //--------------------------------------------------------------
	}
	//// Set MTU
	//struct l2cap_options oOpts;
	//int nOptLen = static_cast<int>(sizeof(oOpts));
	//nRes = getsockopt( m_nListenerFD, SOL_L2CAP, L2CAP_OPTIONS, &oOpts, &nOptLen );
	//if (nRes == 0) {
	//	oOpts.imtu = s_nMtu; // i
	//	oOpts.omtu = s_nMtu; // not really used
	//	nRes = setsockopt( m_nListenerFD, SOL_L2CAP, L2CAP_OPTIONS, &oOpts, nOptLen );
	//}
	//if (nRes < 0) {
	//	m_sErrorStr = "BlueServerAcceptSource(): setsockopt failed: " + std::string(strerror(errno));
	//	close(m_nListenerFD);
	//	m_nListenerFD = -1;
	//	return; //--------------------------------------------------------------
	//}

	// put socket into listening mode
	nRes = ::listen(m_nListenerFD, 3);
	if (nRes < 0) {
		m_sErrorStr = "BlueServerAcceptSource(): listen failed: " + std::string(strerror(errno));
		close(m_nListenerFD);
		m_nListenerFD = -1;
		return; //--------------------------------------------------------------
	}

	m_oAcceptPollFD.set_fd(m_nListenerFD);
	m_oAcceptPollFD.set_events(Glib::IO_IN | Glib::IO_HUP | Glib::IO_ERR | Glib::IO_NVAL);
	Glib::Source::add_poll(m_oAcceptPollFD);

	set_priority(Glib::PRIORITY_DEFAULT);
	set_can_recurse(false);
}
BlueServerAcceptSource::~BlueServerAcceptSource() noexcept
{
	if (m_nListenerFD >= 0) {
		::close(m_nListenerFD);
	}
}
sigc::connection BlueServerAcceptSource::connect(const sigc::slot<bool, int32_t, const struct sockaddr_l2&>& oSlot) noexcept
{
	if (m_nListenerFD == -1) {
		// Error, return an empty connection
		return sigc::connection();
	}
	return connect_generic(oSlot);
}

bool BlueServerAcceptSource::prepare(int& nTimeout) noexcept
{
	nTimeout = -1;

	return false;
}
bool BlueServerAcceptSource::check() noexcept
{
	bool bRet = false;

	if ((m_oAcceptPollFD.get_revents() & (Glib::IO_IN | Glib::IO_HUP | Glib::IO_ERR | Glib::IO_NVAL)) != 0) {
		bRet = true;
	}

	return bRet;
}
bool BlueServerAcceptSource::dispatch(sigc::slot_base* p0Slot) noexcept
{
	bool bContinue = true;

	if (p0Slot == nullptr) {
		return bContinue; //----------------------------------------------------
	}
//std::cout << "BlueServerAcceptSource::dispatch" << '\n';

	auto nIOFlags = m_oAcceptPollFD.get_revents();
	assert(nIOFlags != 0);
	const bool bSomeError = ((nIOFlags & (Glib::IO_HUP | Glib::IO_ERR | Glib::IO_NVAL)) != 0);
	if (bSomeError) {
		std::cerr << "BlueServerAcceptSource::dispatch: accept failed:";
		if ((nIOFlags & Glib::IO_HUP) != 0) {
			std::cerr << " IO_HUP";
		}
		if ((nIOFlags & Glib::IO_ERR) != 0) {
			std::cerr << " IO_ERR";
		}
		if ((nIOFlags & Glib::IO_NVAL) != 0) {
			std::cerr << " IO_NVAL";
		}
		std::cerr << '\n';
		// no idea if it's a good idea to just go on accepting
		// is the error caused by the connecting client or the
		// socket to the local bluetooth chip?
		return bContinue; //----------------------------------------------------
	}
	if ((nIOFlags & Glib::IO_IN) == 0) {
		return bContinue; //----------------------------------------------------
	}

	::sockaddr_l2 oRemoteAddr;
	memset(&oRemoteAddr, 0, sizeof(oRemoteAddr));
	socklen_t nRemoteAdrLen = sizeof(oRemoteAddr);

	// accept one connection
	int32_t nFdClient = ::accept(m_nListenerFD, reinterpret_cast<sockaddr*>(&oRemoteAddr), &nRemoteAdrLen);
	if (nFdClient < 0) {
		std::cerr <<  "BlueServerAcceptSource::dispatch: accept failed: " << strerror(errno) << '\n';
		// no idea if it's a good idea to just go on accepting
		return bContinue; //----------------------------------------------------
	}
	//char sBuf[19];
	//ba2str( &(oRemoteAddr.l2_bdaddr), sBuf );

	bContinue = (*static_cast<sigc::slot<bool, int32_t, const struct ::sockaddr_l2&>*>(p0Slot))(nFdClient, oRemoteAddr);

	return bContinue;
}
////////////////////////////////////////////////////////////////////////////////
BlueServerReceiveSource::BlueServerReceiveSource(int32_t nBackendId, int32_t nClientFD) noexcept
: Glib::Source()
, m_nBackendId(nBackendId)
, m_nClientFD(nClientFD)
{
	assert(m_nBackendId >= 0);
	assert(m_nClientFD >= 0);

	m_oClientPollFD.set_fd(m_nClientFD);
	m_oClientPollFD.set_events(Glib::IO_IN | Glib::IO_HUP | Glib::IO_ERR | Glib::IO_NVAL);
	Glib::Source::add_poll(m_oClientPollFD);

	set_priority(Glib::PRIORITY_DEFAULT);
	set_can_recurse(false);
}
BlueServerReceiveSource::~BlueServerReceiveSource() noexcept
{
	if (m_nClientFD >= 0) {
		::close(m_nClientFD);
	}
}
sigc::connection BlueServerReceiveSource::connect(const sigc::slot<bool, int32_t, bool, const KeyPacket&>& oSlot) noexcept
{
	if (m_nClientFD < 0) {
		// Error, return an empty connection
		return sigc::connection();
	}
	return connect_generic(oSlot);
}

bool BlueServerReceiveSource::prepare(int& nTimeout) noexcept
{
	nTimeout = -1;

	return false;
}
bool BlueServerReceiveSource::check() noexcept
{
	bool bRet = false;

	if ((m_oClientPollFD.get_revents() & (Glib::IO_IN | Glib::IO_HUP | Glib::IO_ERR | Glib::IO_NVAL)) != 0) {
		bRet = true;
	}

	return bRet;
}
bool BlueServerReceiveSource::closeConnection(sigc::slot_base* p0Slot, bool bRemove, KeyPacket& oPkt, const std::string& sErr) noexcept
{
	Glib::Source::destroy();
	if (! sErr.empty()) {
		std::cerr << sErr << '\n';
	}
	::close(m_nClientFD);
	m_nClientFD = -1;
	bool bContinue = false;
	if (p0Slot != nullptr) {
		bContinue = (*static_cast<sigc::slot<bool, int32_t, bool, const KeyPacket&>*>(p0Slot))
													(m_nBackendId, bRemove, oPkt);
		//assert(!bContinue);
	}
	return bContinue;
}
bool BlueServerReceiveSource::dispatch(sigc::slot_base* p0Slot) noexcept
{
	bool bContinue = true;

	if (p0Slot == nullptr) {
		return bContinue; //----------------------------------------------------
	}
//std::cout << "BlueServerReceiveSource::dispatch" << '\n';

	auto nIOFlags = m_oClientPollFD.get_revents();
	const bool bSomeError = ((nIOFlags & (Glib::IO_HUP | Glib::IO_ERR | Glib::IO_NVAL)) != 0);
	if (bSomeError) {
//std::cout << "BlueServerReceiveSource::dispatch()  Error: Glib::IO_HUP | Glib::IO_ERR | Glib::IO_NVAL!!" << '\n';
		KeyPacket oDummyPacket;
		bContinue = closeConnection(p0Slot, false, oDummyPacket, "");
		return bContinue; //----------------------------------------------------
	}
	if ((nIOFlags & Glib::IO_IN) == 0) {
		return bContinue; //----------------------------------------------------
	}
	constexpr auto nPktSize = sizeof(KeyPacket);
	static_assert(nPktSize == 8, "");
	//
	constexpr auto nToRead = static_cast<size_t>(L2CAP_DEFAULT_MTU / nPktSize) - 1;
	assert(nToRead > 0);

	KeyPacket aPackets[nToRead] __attribute__ ((aligned(__alignof__(KeyPacket))));

	auto nBytesReceived = ::recv(m_nClientFD, &(aPackets[0]), nToRead * nPktSize, 0);
	if (nBytesReceived < 0) {
		KeyPacket oDummyPacket;
		bContinue = closeConnection(p0Slot, false, oDummyPacket
									, std::string("BlueServerReceiveSource::dispatch error: recv failed!\n  ") + strerror(errno));
		return bContinue; //----------------------------------------------------
	}
//std::cout << "BlueServerReceiveSource::dispatch() nBytesReceived=" << nBytesReceived << '\n';
	int32_t nPacket = 0;
	int32_t nBufPos = 0;
	// process full packets
	while (nBufPos + static_cast<int32_t>(nPktSize) <= nBytesReceived) {
		auto& oPacket = aPackets[nPacket];
		oPacket.m_nHardwareKey = btohl(oPacket.m_nHardwareKey);
		if ((oPacket.m_nMagic1 != s_nMagic1) || (oPacket.m_nMagic2 != s_nMagic2)) {
			bContinue = closeConnection(p0Slot, false, oPacket, "BlueServerReceiveSource::dispatch error: magic numbers check failed!");
			return bContinue; //------------------------------------------------
		}
		if (oPacket.m_nCmd == PACKET_CMD_REMOVE_DEVICE) {
//std::cout << "BlueServerReceiveSource::dispatch()  PACKET_CMD_REMOVE_DEVICE" << '\n';
			bContinue = closeConnection(p0Slot, true, oPacket, "");
			return bContinue; //------------------------------------------------
		}
		//if (oPacket.m_nCmd == PACKET_CMD_DISCONNECT_DEVICE) {
		//	bContinue = closeConnection(p0Slot, false, oPacket, "");
		//	return bContinue; //------------------------------------------------
		//}
		if (oPacket.m_nCmd != PACKET_CMD_NOOP) {
			if (oPacket.m_nCmd != PACKET_CMD_KEY) {
				bContinue = closeConnection(p0Slot, false, oPacket, "BlueServerReceiveSource::dispatch error: bad cmd field!");
				return bContinue; //--------------------------------------------
				
			}
			bContinue = (*static_cast<sigc::slot<bool, int32_t, bool, const KeyPacket&>*>(p0Slot))
														(m_nBackendId, false, oPacket);
			if (!bContinue) {
				// listener requests to stop processing
				bContinue = closeConnection(nullptr, false, oPacket, "");
				return bContinue; //--------------------------------------------
			}
		}
		//
		++nPacket;
		nBufPos += sizeof(KeyPacket);
	}
	if (nBufPos < nBytesReceived) {
		KeyPacket oDummyPacket;
		bContinue = closeConnection(p0Slot, false, oDummyPacket, "BlueServerReceiveSource::dispatch error: pkt < 8 bytes!");
		return bContinue; //----------------------------------------------------
	}
	return bContinue;
}

} // namespace Bt
} // namespace Private

} // namespace stmi
