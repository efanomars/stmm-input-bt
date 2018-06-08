/*
 * Copyright © 2017-2018  Stefano Marsili, <stemars@gmx.ch>
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
 * File:   btkeyservers.cc
 */
#include "btkeyservers.h"
#include "keypacket.h"

#include <chrono>
#include <cassert>
#include <iostream>

#include <bluetooth/l2cap.h>
#include <sys/socket.h>
#include <unistd.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>
#include <errno.h>

namespace stmi
{

static constexpr int32_t s_nMaxServers = 255;


BtKeyServers::BtKeyServers()
: BtKeyServers(s_nDefault1s28PeriodsAddr, s_nDefaultL2capPort, false, ::bdaddr_t{}, false)
{
}
BtKeyServers::BtKeyServers(int32_t n1s28PeriodsAddr, int32_t nL2capPort, const ::bdaddr_t& oExtraAddr, bool bFlush)
: BtKeyServers(n1s28PeriodsAddr, nL2capPort, true, oExtraAddr, bFlush)
{
	
}
BtKeyServers::BtKeyServers(int32_t n1s28PeriodsAddr, int32_t nL2capPort
							, bool bExtraServer, const ::bdaddr_t& oExtraAddr, bool bFlush)
: m_n1s28PeriodsAddr(n1s28PeriodsAddr)
, m_nL2capPort(nL2capPort)
, m_oExtraAddr(oExtraAddr)
, m_bFlush(bFlush)
, m_oPersistentFunction([&](bool bFlush, int32_t n1s28PeriodsAddr)
{
	return refreshServers(bFlush, n1s28PeriodsAddr);
})
, m_nRefreshStarted(-1)
, m_bHungThread(false)
{
	if (!bExtraServer) {
		m_oExtraAddr = getEmptyAddr();
	}
	if (! isEmptyAddr(m_oExtraAddr)) {
		addExtraServer(m_aServers);
	}
}
void BtKeyServers::addExtraServer(std::vector<ServerInfo>& aServers)
{
//std::cout << "BtKeyServers::addExtraServer()" << '\n';
	aServers.push_back(ServerInfo{});
	ServerInfo& oInfo = aServers.back();
	oInfo.m_oBtAddr = getAddrCopy(m_oExtraAddr);
	oInfo.m_nL2capPort = m_nL2capPort;
	oInfo.m_bSDP = false;
	oInfo.m_sName = "Unknown";
}
bool BtKeyServers::isValidPort(int32_t nL2capPort)
{
	if (! ((nL2capPort >= 4097) && (nL2capPort <= 32765))) {
		return false; //--------------------------------------------------------
	}
	if (nL2capPort % 2 == 0) {
		return false; //--------------------------------------------------------
	}
	return true;
}
bool BtKeyServers::isValid(const ServerInfo& oInfo)
{
	if (! isValidPort(oInfo.m_nL2capPort)) {
		return false;
	}
	return isValidAddr(oInfo.m_oBtAddr);
}
std::string BtKeyServers::getStringFromAddr(const bdaddr_t& oBtAddr)
{
	char sName[20];
	memset(sName, 0, sizeof(sName));
	::ba2str(&oBtAddr, sName);
	return sName;
}
bool BtKeyServers::isValidAddr(const bdaddr_t& oBtAddr)
{
	// FF:FF:FF:FF:FF:FF and 00:00:00:00:00:00 are not valid addresses
	const auto cFirst = oBtAddr.b[0];
	for (int32_t nIdx = 1; nIdx < 6; ++nIdx) {
		if (oBtAddr.b[nIdx] != cFirst) {
			return true; //-----------------------------------------------------
		}
	}
	return ((cFirst != 0) && (cFirst != 0xFF));
}
bool BtKeyServers::isValidStringAddr(const std::string& sBtAddr)
{
	bdaddr_t oAddr;
	const auto nRet = str2ba(sBtAddr.c_str(), &oAddr);
	if (nRet != 0) {
		return false;
	}
	return isValidAddr(oAddr);
}
bdaddr_t BtKeyServers::getAddrFromString(const std::string& sBtAddr)
{
	bdaddr_t oAddr;
	#ifndef NDEBUG
	const auto nRet = 
	#endif //NDEBUG
	str2ba(sBtAddr.c_str(), &oAddr);
	assert(nRet == 0);
	return oAddr;
}
bdaddr_t BtKeyServers::getAddrCopy(const bdaddr_t& oBtAddr)
{
	bdaddr_t oAddr;
	bacpy(&oAddr, &oBtAddr);
	return oAddr;
}
bdaddr_t BtKeyServers::getEmptyAddr()
{
	bdaddr_t oAddr;
	memset(&oAddr, 0, sizeof(oAddr));
	return oAddr;
}
bool BtKeyServers::isEmptyAddr(const bdaddr_t& oBtAddr)
{
	static const bdaddr_t s_oEmpty = getEmptyAddr();
	return equalAddrs(s_oEmpty, oBtAddr);
}
bool BtKeyServers::equalAddrs(const bdaddr_t& oBtAddr1, const bdaddr_t& oBtAddr2)
{
	return (::bacmp(&oBtAddr1, &oBtAddr2) == 0);
}
void BtKeyServers::addLocalAddress()
{
	// This doesn't seem to work
	//m_aServers.push_back(ServerInfo{});
	//ServerInfo& oLocalInfo = m_aServers.back();
	//auto& oLocalAddr = oLocalInfo.m_oBtAddr;
	//oLocalAddr.b[0] = 0;
	//oLocalAddr.b[1] = 0;
	//oLocalAddr.b[2] = 0;
	//oLocalAddr.b[3] = 255;
	//oLocalAddr.b[4] = 255;
	//oLocalAddr.b[5] = 255;
	//oLocalInfo.m_nL2capPort = m_nL2capPort;
	//oLocalInfo.m_bSDP = false;
	//oLocalInfo.m_sName = "Local computer";
}
bool BtKeyServers::startRefreshServers()
{
//std::cout << "BtKeyServers::startRefreshServers() m_nRefreshStarted=" << m_nRefreshStarted << '\n';
	if (m_nRefreshStarted >= 0) {
		// already started
		return false;
	}
	m_sLastError = "";

	if (m_aServers.empty()) {
		// while refreshing at least provide the local address
		addLocalAddress();
	}

	if (!m_refIntervalTimeout) {
		m_refIntervalTimeout = Glib::RefPtr<IntervalTimeoutSource>{ new IntervalTimeoutSource(1000) };
		m_refIntervalTimeout->connectSlot(sigc::mem_fun(this, &BtKeyServers::doIntervalTimeout));
		m_refIntervalTimeout->attach();
	}

	m_nRefreshStarted = m_refIntervalTimeout->getElapsed();

	m_oRefreshFuture = std::async(std::launch::async, m_oPersistentFunction, m_bFlush, m_n1s28PeriodsAddr);
	return true;
}
std::pair<std::string, std::vector<BtKeyServers::ServerInfo>> BtKeyServers::refreshServers(bool bFlush, int32_t n1s28PeriodsAddr)
{
//std::cout << "BtKeyServers::refreshServers() ASYNC STARTED" << '\n';
	std::vector<ServerInfo> aServers;

	const int32_t nDevId = ::hci_get_route(nullptr);
	const auto nHciFD = ::hci_open_dev( nDevId );

	if ((nDevId < 0) || (nHciFD < 0)) {
		addExtraServer(aServers);
		const auto oPair = std::make_pair(std::string("Error opening hci socket: ") + strerror(errno), aServers);
		return oPair; //--------------------------------------------------------
	}
	
	int nFlags = (bFlush ? IREQ_CACHE_FLUSH : 0);

	inquiry_info aInquiryInfos[s_nMaxServers];
	inquiry_info* p0InquiryInfos = &(aInquiryInfos[0]);

	bool bExtraAddrFound = false;
//std::cout << "BtKeyServers::refreshServers() BEFORE hci_inquiry" << '\n';
	const auto nFoundServers = ::hci_inquiry(nDevId, n1s28PeriodsAddr, s_nMaxServers, nullptr, &p0InquiryInfos, nFlags);
//std::cout << "BtKeyServers::refreshServers() AFTER  hci_inquiry" << '\n';
	if (nFoundServers < 0) {
		::close(nHciFD);
		addExtraServer(aServers);
		const auto oPair = std::make_pair(std::string("Error hci_inquiry: ") + strerror(errno), aServers);
		return oPair; //--------------------------------------------------------
	}
	for (auto nIdx = nFoundServers - nFoundServers; nIdx < nFoundServers; ++nIdx) {
		//
		aServers.push_back(ServerInfo{});
		ServerInfo& oInfo = aServers.back();
		//
		const inquiry_info& oIi = aInquiryInfos[nIdx];
		oInfo.m_oBtAddr = getAddrCopy(oIi.bdaddr);
		if ((!isEmptyAddr(m_oExtraAddr)) && equalAddrs(m_oExtraAddr, oInfo.m_oBtAddr)) {
			bExtraAddrFound = true;
		}
		oInfo.m_nL2capPort = m_nL2capPort;
		oInfo.m_bSDP = false;
		char sName[248];
		assert(sizeof(sName) == 248);
		memset(sName, 0, sizeof(sName));
		auto nRes = ::hci_read_remote_name(nHciFD, &(oIi.bdaddr), sizeof(sName), sName, 0);
		if (nRes < 0) {
			// error ignored
			memset(sName, 0, sizeof(sName));
			::ba2str(&(oInfo.m_oBtAddr), sName);
		}
		oInfo.m_sName = sName;
	}
	if ((!isEmptyAddr(m_oExtraAddr)) && !bExtraAddrFound) {
		addExtraServer(aServers);
	}
	::close(nHciFD);
	return std::make_pair("", aServers);
}
bool BtKeyServers::doIntervalTimeout()
{
//std::cout << "BtKeyServers::doIntervalTimeout()" << '\n';
	const bool bContinue = true;
	if (m_nRefreshStarted < 0) {
		return bContinue;
	}

	assert(m_oRefreshFuture.valid());
	std::future_status oStatus = m_oRefreshFuture.wait_for(std::chrono::seconds(0));
	if (oStatus == std::future_status::ready) {
//std::cout << "BtKeyServers::doIntervalTimeout() READY" << '\n';
		m_aServers.clear();
		addLocalAddress();
		auto oPair = m_oRefreshFuture.get();
		const std::string& sError = oPair.first;
		// refresh ended
		m_nRefreshStarted = -1;
		if (!sError.empty()) {
			m_sLastError = sError;
		}
		std::vector<BtKeyServers::ServerInfo>& aServers = oPair.second;
		for (auto& oInfo : aServers) {
			m_aServers.push_back(std::move(oInfo));
		}
		m_oServersChangedSignal.emit();
	} else {
//std::cout << "BtKeyServers::doIntervalTimeout() NOT READY" << '\n';
		// not ready yet, inform of progress
		int64_t nElapsedMillisec = m_refIntervalTimeout->getElapsed() - m_nRefreshStarted;
		m_oRefreshProgressSignal.emit(nElapsedMillisec / 1000);
		if (nElapsedMillisec > std::max(14000, 2 * 1280 * m_n1s28PeriodsAddr)) {
			// something is wrong with the driver
			// since the thread probably won't stop termination is needed on exit
			m_bHungThread = true;
			// refresh ended
			m_nRefreshStarted = -1;
			m_sLastError = "Refresh takes too long: hci driver error\nPlease restart the application.";
			m_oServersChangedSignal.emit();
		}
	}
	return bContinue;
}
const std::vector<BtKeyServers::ServerInfo>& BtKeyServers::getServers()
{
	if (isEmptyAddr(m_oExtraAddr) && !m_refIntervalTimeout) {
		startRefreshServers();
	}
	return m_aServers;
}


} // namespace stmi
