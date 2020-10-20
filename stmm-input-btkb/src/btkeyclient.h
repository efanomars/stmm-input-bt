/*
 * Copyright © 2017-2020  Stefano Marsili, <stemars@gmx.ch>
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
 * You should have received a copy of the GNU General Public
 * License along with this program; if not, see <http://www.gnu.org/licenses/>
 */
/*
 * File:   btkeyclient.h
 */

#ifndef STMI_BT_KEY_CLIENT_H
#define STMI_BT_KEY_CLIENT_H

#include "btclientsources.h"
#include "circularbuffer.h"
#include "hardwarekey.h"
#include "keypacket.h"

#include <sigc++/signal.h>

#include <string>
#include <utility>

#include <bluetooth/bluetooth.h>

#include <stdint.h>

namespace stmi
{

class BtKeyClient
{
public:
	/** Default constructor.
	 */
	BtKeyClient() noexcept;
	/** Constructor.
	 * @param nTimeoutConnect The timeout in milliseconds for connecting to server.
	 * @param nTimeoutSend The timeout in milliseconds for sending a packet.
	 * @param nInterval The interval on which timeouts are checked.
	 * @param nNoopAfter The time in milliseconds without activity after which a NOOP packet is sent. Zero means never.
	 */
	BtKeyClient(int32_t nTimeoutConnect, int32_t nTimeoutSend, int32_t nInterval, int32_t nNoopAfter) noexcept;

	int32_t getTimeoutConnect() const noexcept { return m_nTimeoutConnect; }
	int32_t getTimeoutSend() const noexcept { return m_nTimeoutSend; }
	int32_t getInterval() const noexcept { return m_nInterval; }
	int32_t getL2capPort() const noexcept { return m_nL2capPort; }

	// state machine states
	enum STATE {
		STATE_DISCONNECTED = 0
		, STATE_CONNECTING = 1
		, STATE_CONNECTED = 2
		, STATE_SENDING = 3
		, STATE_REMOVING = 10 // Removing the virtual plug
	};

	STATE getState() const noexcept { return m_eState; }
	const std::string& getError() const noexcept { return m_sLastError; }
	/** Connect to a BtKey server.
	 *
	 * @param oBtAddr The address.
	 */
	void connectToServer(const bdaddr_t& oBtAddr, int32_t nL2capPort) noexcept;
	void disconnectFromServer() noexcept;
	void sendRemoveToServer() noexcept;

	void sendKeyToServer(hk::KEY_INPUT_TYPE eType, hk::HARDWARE_KEY eKey) noexcept;

	sigc::signal<void> m_oStateChangedSignal;
	sigc::signal<void> m_oErrorSignal;

	static constexpr int32_t s_nDefaultInterval = 500;
	static constexpr int32_t s_nDefaultTimeoutConnect = 10 * 1000;
	static constexpr int32_t s_nDefaultTimeoutSend = 1 * 1000;
	static constexpr int32_t s_nDefaultNoopAfter = 5 * 1000;
private:
	bool doPendingConnect(int32_t nSourceId, bool bError) noexcept;
	bool doPendingSend(int32_t nSourceId, bool bError) noexcept;
	bool doIntervalTimeout() noexcept;
	//
	void disconnectInternal(const std::string& sErrorString) noexcept;
	void sendPacketsFromBufferedKeys() noexcept;
private:
	const int32_t m_nTimeoutConnect;
	const int32_t m_nTimeoutSend;
	const int32_t m_nInterval;
	const int32_t m_nNoopAfter;
	STATE m_eState;
	std::string m_sLastError;
	int32_t m_nClientFD; // The socket
	bdaddr_t m_oBtAddr; // The address of the server
	int32_t m_nL2capPort; // The port of the server
	int64_t m_nStartedConnectingTime;
	int64_t m_nStartedSendingTime;
	CircularBuffer<std::pair<hk::KEY_INPUT_TYPE, hk::HARDWARE_KEY>> m_aBufferedKeys;
	int64_t m_nLastSentTime;
	static constexpr int32_t s_nSendBufferSize = 20;
	KeyPacket m_aPackets[s_nSendBufferSize] __attribute__ ((aligned(__alignof__(KeyPacket))));
	int32_t m_nTotPackets; // number of packets in m_aPackets

	Glib::RefPtr<PendingWriteSource> m_refPendingConnect;
	Glib::RefPtr<PendingWriteSource> m_refPendingSend;
	Glib::RefPtr<IntervalTimeoutSource> m_refIntervalTimeout;
private:
	BtKeyClient(const BtKeyClient& oSource) = delete;
	BtKeyClient& operator=(const BtKeyClient& oSource) = delete;
};

} // namespace stmi

#endif /* STMI_BT_KEY_CLIENT_H */

