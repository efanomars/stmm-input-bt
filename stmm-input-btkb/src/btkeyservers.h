/*
 * Copyright © 2017-2019  Stefano Marsili, <stemars@gmx.ch>
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
 * File:   btkeyservers.h
 */

#ifndef STMI_BT_KEY_SERVERS_H
#define STMI_BT_KEY_SERVERS_H

#include "btclientsources.h"

#include <sigc++/signal.h>
#include <glibmm/refptr.h>

#include <string>
#include <vector>
#include <future>
#include <functional>
#include <utility>

#include <bluetooth/bluetooth.h>

#include <stdint.h>

namespace stmi
{

class BtKeyServers
{
public:
	/** Default constructor.
	 */
	BtKeyServers() noexcept;
	/** Constructor.
	 * SDP means Service Discovery Protocol. Currently not supported.
	 * @param n1s28PeriodsAddr The number of 1.28 second period for inquiring.
	 * @param nL2capPort The L2CAP port to try if SDP entry not available.
	 * @param oExtraAddr Additional server to be added to the list.
	 * @param bFlush Whether to forget previously found servers.
	 */
	BtKeyServers(int32_t n1s28PeriodsAddr, int32_t nL2capPort, const ::bdaddr_t& oExtraAddr, bool bFlush) noexcept;

	struct ServerInfo
	{
		// The unique key is (m_oBtAddr, m_nL2capPort) even though the current implementation
		// not using SDP always has the same m_nL2capPort port.
		bdaddr_t m_oBtAddr; /**< The address of the server. Can be local computer. */
		int32_t m_nL2capPort = 0; /**< The to be used port to connect to the server. */
		std::string m_sName; /**< The name of the server. */
		bool m_bSDP = false; /**< whether port retrieved through SDP. */
	};

	static bool isValidStringAddr(const std::string& oBtAddr) noexcept;
	static bool isValidAddr(const bdaddr_t& oBtAddr) noexcept;
	static bool isValidPort(int32_t nL2capPort) noexcept;
	static bool isValid(const ServerInfo& oInfo) noexcept;
	static std::string getStringFromAddr(const bdaddr_t& oBtAddr) noexcept;
	static bdaddr_t getAddrFromString(const std::string& sBtAddr) noexcept;
	static bdaddr_t getAddrCopy(const bdaddr_t& oBtAddr) noexcept;
	static bool isEmptyAddr(const bdaddr_t& oBtAddr) noexcept;
	static bdaddr_t getEmptyAddr() noexcept;
	static bool equalAddrs(const bdaddr_t& oBtAddr1, const bdaddr_t& oBtAddr2) noexcept;

	/** Start server inquiry.
	 * This operation can take many seconds, so getServers() should be called
	 * only after the m_oServersChangedSignal is received.
	 *
	 * While refreshing, every 500 millisec the m_oRefreshProgressSignal is called
	 * with the (approximate) time in seconds passed as parameter.
	 *
	 * The current implementation doesn't use SDP.
	 * @return If false if already refreshing.
	 */
	bool startRefreshServers() noexcept;
	/** Tells whether startRefreshServers has terminated yet.
	 * @return Whether it is refreshing.
	 */
	bool isRefreshing() const noexcept { return (m_nRefreshStarted >= 0); }
	/** Returns the servers, if available.
	 * If startRefreshServers() was never called, it calls it first returning
	 * only the local server, if available.
	 *
	 * Currently the servers are just the bluetooth devices in the neighborhood.
	 * @return The servers.
	 */
	const std::vector<ServerInfo>& getServers() noexcept;
	/** The error of the last startRefreshServers result.
	 * This function should be called after receiving the m_oServersChangedSignal call.
	 * @return The error string.
	 */
	const std::string& getError() const noexcept { return m_sLastError; }

	/** Tells whether a thread probably hung.
	 * @return Whether std::terminate should be called on exit.
	 */
	bool isThreadHung() const noexcept { return m_bHungThread; }

	sigc::signal<void, int32_t> m_oRefreshProgressSignal; // Param: nr of seconds since start of refresh
	sigc::signal<void> m_oServersChangedSignal; // Called when refresh ended

	static constexpr int32_t s_nDefault1s28PeriodsAddr = 8;
	static constexpr int32_t s_nDefaultL2capPort = 0x20A1; // 8353
private:
	BtKeyServers(int32_t n1s28PeriodsAddr, int32_t nL2capPort, bool bExtraServer, const ::bdaddr_t& oExtraAddr, bool bFlush) noexcept;
	void addExtraServer(std::vector<ServerInfo>& aServers) noexcept;

	bool doIntervalTimeout() noexcept;
	// std::future function
	std::pair<std::string, std::vector<BtKeyServers::ServerInfo>> refreshServers(bool bFlush, int32_t n1s28PeriodsAddr) noexcept;
	void addLocalAddress() noexcept;
private:
	int32_t m_n1s28PeriodsAddr;
	int32_t m_nL2capPort;
	::bdaddr_t m_oExtraAddr;
	bool m_bFlush;
	std::string m_sLastError;

	std::vector<ServerInfo> m_aServers;

	// if sError is not empty aServers is empty.
	std::function< std::pair<std::string, std::vector<BtKeyServers::ServerInfo>>(bool, int32_t) > m_oPersistentFunction;
	std::future< std::pair<std::string, std::vector<ServerInfo>> > m_oRefreshFuture; // Value: (sError, aServers)
	int64_t m_nRefreshStarted; // time refresh was started

	bool m_bHungThread;

	// This timeout checks if the future has completed
	Glib::RefPtr<IntervalTimeoutSource> m_refIntervalTimeout;
private:
	BtKeyServers(const BtKeyServers& oSource) = delete;
	BtKeyServers& operator=(const BtKeyServers& oSource) = delete;
};

} // namespace stmi

#endif /* STMI_BT_KEY_SERVERS_H */

