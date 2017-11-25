/*
 * Copyright © 2017  Stefano Marsili, <stemars@gmx.ch>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, see <http://www.gnu.org/licenses/>
 */
/*
 * File:   btkeyclient.cc
 */
#include "btkeyclient.h"
#include "btkeyservers.h"

#include <cassert>
#include <iostream>

#include <bluetooth/l2cap.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <unistd.h>

namespace stmi
{

constexpr char s_nMagic1 = '7';
constexpr char s_nMagic2 = 'A';

BtKeyClient::BtKeyClient()
: BtKeyClient(s_nDefaultTimeoutConnect, s_nDefaultTimeoutSend, s_nDefaultInterval, s_nDefaultNoopAfter)
{
}
BtKeyClient::BtKeyClient(int32_t nTimeoutConnect, int32_t nTimeoutSend, int32_t nInterval, int32_t nNoopAfter)
: m_nTimeoutConnect(nTimeoutConnect)
, m_nTimeoutSend(nTimeoutSend)
, m_nInterval(nInterval)
, m_nNoopAfter(nNoopAfter)
, m_eState(STATE_DISCONNECTED)
, m_nClientFD(-1)
, m_nStartedConnectingTime(0)
, m_nStartedSendingTime(0)
, m_aBufferedKeys(s_nSendBufferSize)
, m_nLastSentTime(-1)
, m_nTotPackets(0)
{
}

void BtKeyClient::connectToServer(const bdaddr_t& oBtAddr, int32_t nL2capPort)
{
	if (m_eState == STATE_REMOVING) {
		// wait till fully disconnected
		return; //--------------------------------------------------------------
	}
	if (m_nClientFD >= 0) {
		// first disconnect
		return; //--------------------------------------------------------------
	}

	assert(m_eState == STATE_DISCONNECTED);

	// allocate a socket
	m_nClientFD = socket(AF_BLUETOOTH, SOCK_SEQPACKET, BTPROTO_L2CAP);
	if (m_nClientFD < 0) {
		m_sLastError = std::string("Socket failed: ") + strerror(errno);
		m_oErrorSignal();
		return; //--------------------------------------------------------------
	}
	// Set non-blocking 
	auto nArg = fcntl(m_nClientFD, F_GETFL, 0);
	if (nArg < 0) {
		::close(m_nClientFD);
		m_nClientFD = -1;
		m_sLastError = std::string("Socket fcntl get failed: ") + strerror(errno);
		m_oErrorSignal();
		return; //--------------------------------------------------------------
	} 
	nArg |= O_NONBLOCK; 
	nArg = fcntl(m_nClientFD, F_SETFL, nArg);
	if (nArg < 0) {
		::close(m_nClientFD);
		m_nClientFD = -1;
		m_sLastError = std::string("Socket fcntl set failed: ") + strerror(errno);
		m_oErrorSignal();
		return; //--------------------------------------------------------------
	} 

	m_oBtAddr = BtKeyServers::getAddrCopy(oBtAddr);
	m_nL2capPort = nL2capPort;

	::sockaddr_l2 oL2Addr;
	memset(&oL2Addr, 0, sizeof(::sockaddr_l2));

	oL2Addr.l2_family = AF_BLUETOOTH;
	oL2Addr.l2_psm = htobs(m_nL2capPort);
	oL2Addr.l2_bdaddr = BtKeyServers::getAddrCopy(m_oBtAddr);

	// connect to server
	const auto nRes = ::connect(m_nClientFD, reinterpret_cast<sockaddr*>(&oL2Addr), sizeof(oL2Addr));
	if (nRes < 0) {
		if (errno == EINPROGRESS) {
			m_refPendingConnect = Glib::RefPtr<PendingWriteSource>{ new PendingWriteSource(m_nClientFD) };
			m_refPendingConnect->connect(sigc::mem_fun(this, &BtKeyClient::doPendingConnect));
			m_refPendingConnect->attach();
//std::cout << "BtKeyClient::connectToServer  EINPROGRESS  nSourceId=" << m_refPendingConnect->getSourceId() << '\n';
			if (!m_refIntervalTimeout) {
				m_refIntervalTimeout = Glib::RefPtr<IntervalTimeoutSource>{ new IntervalTimeoutSource(m_nInterval) };
				m_refIntervalTimeout->connectSlot(sigc::mem_fun(this, &BtKeyClient::doIntervalTimeout));
				m_refIntervalTimeout->attach();
			}
			m_nStartedConnectingTime = m_refIntervalTimeout->getElapsed();
			m_eState = STATE_CONNECTING;
			m_oStateChangedSignal();
		} else {
			::close(m_nClientFD);
			m_nClientFD = -1;
			m_sLastError = std::string("Failed to connect: ") + strerror(errno);
			m_oErrorSignal();
			return; //----------------------------------------------------------
		}
	} else {
		m_sLastError.clear();
		m_eState = STATE_CONNECTED;
		m_oStateChangedSignal();
	}
}
bool BtKeyClient::doPendingConnect(int32_t nSourceId, bool bError)
{
//std::cout << "BtKeyClient::doPendingConnect  nSourceId=" << nSourceId << '\n';
	const bool bContinue = true;

	if (!m_refPendingConnect) {
		return ! bContinue; //--------------------------------------------------
	}
	if (nSourceId != m_refPendingConnect->getSourceId()) {
		return ! bContinue; //--------------------------------------------------
	}
	assert(m_eState == STATE_CONNECTING);
	if (bError) {
		disconnectInternal("Failed to connect");
		return ! bContinue; //----------------------------------------------
	}
	m_refPendingConnect->removePoll();
	m_refPendingConnect.reset();
	m_sLastError.clear();
	m_eState = STATE_CONNECTED;
	m_oStateChangedSignal();
//std::cout << "BtKeyClient::doPendingConnect  CONNECTED!" << '\n';
	return ! bContinue;
}
bool BtKeyClient::doPendingSend(int32_t nSourceId, bool bError)
{
//std::cout << "BtKeyClient::doPendingWrite  nSourceId=" << nSourceId << '\n';
	const bool bContinue = true;

	if (!m_refPendingSend) {
		return ! bContinue; //--------------------------------------------------
	}
	if (nSourceId != m_refPendingSend->getSourceId()) {
		return ! bContinue; //--------------------------------------------------
	}
	assert(m_eState == STATE_SENDING);
	if (bError) {
		disconnectInternal("Sending packet failed");
		return ! bContinue; //--------------------------------------------------
	}
	const auto nRes = ::send(m_nClientFD, m_aPackets, sizeof(KeyPacket) * m_nTotPackets, 0);
	if (nRes < 0) {
		disconnectInternal(std::string("Sending packet failed: ") + strerror(errno));
		return ! bContinue; //--------------------------------------------------
	}
	assert(m_refIntervalTimeout);
	m_nLastSentTime = m_refIntervalTimeout->getElapsed();
	// sending succeeded
	if (! m_aBufferedKeys.isEmpty()) {
		// but a buffer of keys accumulated in the meantime
		// TODO send it now or issue a timeout of 0?
		sendPacketsFromBufferedKeys();
	} else {
		m_eState = STATE_CONNECTED;
		m_oStateChangedSignal();
	}
	return bContinue;
}
void BtKeyClient::disconnectFromServer()
{
	if (m_eState == STATE_REMOVING) {
		// the server to disconnect anyway
		return;
	}
	disconnectInternal("");
}
void BtKeyClient::disconnectInternal(const std::string& sErrorString)
{
//std::cout << "BtKeyClient::disconnectInternal() sErrorString=" << sErrorString << "  m_nClientFD=" << m_nClientFD << '\n';
	if (m_nClientFD < 0) {
		// already disconnected
		return;
	}
	::close(m_nClientFD);
	m_nClientFD = -1;
	m_refPendingConnect.reset();
	m_refPendingSend.reset();
	m_sLastError = sErrorString;
	m_eState = STATE_DISCONNECTED;
	if (! sErrorString.empty()) {
		m_oErrorSignal();
	}
	m_oStateChangedSignal();
}
void BtKeyClient::sendKeyToServer(hk::KEY_INPUT_TYPE eType, hk::HARDWARE_KEY eKey)
{
	if (m_eState == STATE_SENDING) {
		// buffer the keys while trying to send the preceding batch
		if (m_aBufferedKeys.isFull()) {
			m_aBufferedKeys.read(); // forget oldest key
		}
		m_aBufferedKeys.write(std::make_pair(eType, eKey));
	} else if (m_eState == STATE_CONNECTED) {
		if (m_aBufferedKeys.isFull()) {
			// make place
			m_aBufferedKeys.read();
		}
		m_aBufferedKeys.write(std::make_pair(eType, eKey));
		sendPacketsFromBufferedKeys();
	}
}
void BtKeyClient::sendPacketsFromBufferedKeys()
{
	const auto eOldState = m_eState;

	STATE eNewState = STATE_SENDING;
	m_nTotPackets = 0;
	while (! m_aBufferedKeys.isEmpty()) {
		const auto oPair = m_aBufferedKeys.read();
		hk::KEY_INPUT_TYPE eType = oPair.first;
		hk::HARDWARE_KEY eKey = oPair.second;
//std::cout << "BtKeyClient::sendPacketsFromBufferedKeys  eType=" << static_cast<int32_t>(eType) << "  eKey=" << static_cast<int32_t>(eKey) << '\n';
		auto& oKeyPacket = m_aPackets[m_nTotPackets];
		oKeyPacket.m_nMagic1 = s_nMagic1; // '7';
		oKeyPacket.m_nMagic2 = s_nMagic2; // 'A';
		if (eType == hk::KEY_REMOVE_DEVICE) {
			oKeyPacket.m_nCmd = PACKET_CMD_REMOVE_DEVICE;
			eNewState = STATE_REMOVING;
		} else if (eType == hk::KEY_NOOP) {
			oKeyPacket.m_nCmd = PACKET_CMD_NOOP;
		} else {
			oKeyPacket.m_nCmd = PACKET_CMD_KEY;
			oKeyPacket.m_nKeyType = static_cast<char>(eType);
			oKeyPacket.m_nHardwareKey = static_cast<int32_t>(eKey);
		}
		++m_nTotPackets;
	}
//std::cout << "BtKeyClient::sendPacketsFromBufferedKeys  m_nTotPackets=" << m_nTotPackets << '\n';
	const auto nRes = ::send(m_nClientFD, m_aPackets, sizeof(KeyPacket) * m_nTotPackets, 0);
	if (nRes < 0) {
#if (EAGAIN == EWOULDBLOCK)
		if (errno == EAGAIN) {
#else
		if ((errno == EAGAIN) || (errno == EWOULDBLOCK)) {
#endif
			if (!m_refPendingSend) {
				m_refPendingSend = Glib::RefPtr<PendingWriteSource>{ new PendingWriteSource(m_nClientFD) };
				m_refPendingSend->connect(sigc::mem_fun(this, &BtKeyClient::doPendingSend));
				m_refPendingSend->attach();
			}
			m_nStartedSendingTime = m_refIntervalTimeout->getElapsed();
			m_eState = eNewState;
			if (eOldState != m_eState) {
				m_oStateChangedSignal();
			}
		} else {
			disconnectInternal(std::string("Failed to send: ") + strerror(errno));
			return; //----------------------------------------------------------
		}
	} else {
		assert(m_refIntervalTimeout);
		m_nLastSentTime = m_refIntervalTimeout->getElapsed();
		m_sLastError.clear();
		if (eNewState != STATE_SENDING) {
			// removing or disconnecting
			disconnectInternal("");
			return; //----------------------------------------------------------
		}
		m_eState = STATE_CONNECTED;
		if (eOldState != m_eState) {
			m_oStateChangedSignal();
		}
	}
}
void BtKeyClient::sendRemoveToServer()
{
	if ((m_eState == STATE_CONNECTING) || (m_eState == STATE_DISCONNECTED)) {
		// this command only works when connected
		return;
	}
	if (m_eState == STATE_SENDING) {
		m_aBufferedKeys.clear();
	}
	m_aBufferedKeys.write(std::make_pair(hk::KEY_REMOVE_DEVICE, hk::HK_NULL));
	sendPacketsFromBufferedKeys();
}
//void BtKeyClient::sendDisconnectToServer()
//{
//	if ((m_eState == STATE_CONNECTING) || (m_eState == STATE_DISCONNECTED)) {
//		// this command only works when connected
//		return;
//	}
//	m_aBufferedKeys.write(std::make_pair(hk::KEY_DISCONNECT_DEVICE, hk::HK_NULL));
//	sendPacketsFromBufferedKeys();
//}
bool BtKeyClient::doIntervalTimeout()
{
//std::cout << "BtKeyClient::doIntervalTimeout()  m_eState=" << static_cast<int32_t>(m_eState) << '\n';
	assert(m_refIntervalTimeout);
	const bool bContinue = true;
	const int64_t nElapsed = m_refIntervalTimeout->getElapsed();
	if (m_eState == STATE_CONNECTING) {
		if (nElapsed - m_nStartedConnectingTime > m_nTimeoutConnect) {
			// timed out
			disconnectInternal("Connecting timed out");
		}
	} else if (m_eState == STATE_CONNECTED) {
		if ((m_nNoopAfter > 0) && (nElapsed - m_nLastSentTime > std::max(m_nNoopAfter, m_nTimeoutSend * 2))) {
			assert(m_aBufferedKeys.isEmpty());
			m_aBufferedKeys.write(std::make_pair(hk::KEY_NOOP, hk::HK_NULL));
			sendPacketsFromBufferedKeys();
		}
	} else if (m_eState == STATE_SENDING) {
		if (nElapsed - m_nStartedSendingTime > m_nTimeoutSend) {
			// timed out
			m_sLastError = "Sending timed out";
			m_eState = STATE_CONNECTED;
			m_oErrorSignal();
			m_oStateChangedSignal();
		}
	} else {
		// in the meantime disconnectFromServer was called?
	}
	return bContinue;
}

} // namespace stmi
