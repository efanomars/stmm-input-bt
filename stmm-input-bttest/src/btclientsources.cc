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
 * File:   btclientsources.cc
 */

#include "btclientsources.h"

#include <glibmm.h>

#ifndef NDEBUG
#include <iostream>
#endif //NDEBUG

#include <unistd.h>

namespace stmi
{

PendingWriteSource::PendingWriteSource(int32_t nClientFD)
: Glib::Source()
, m_nClientFD(nClientFD)
{
	assert(nClientFD >= 0);
	static_assert(sizeof(int) <= sizeof(int32_t), "");
	static_assert(false == FALSE, "");
	static_assert(true == TRUE, "");

	static int32_t s_nSourceId = 0;
	m_nUniqueId = s_nSourceId;
	++s_nSourceId;

	m_oConnectPollFD.set_fd(m_nClientFD);
	m_oConnectPollFD.set_events(Glib::IO_OUT | Glib::IO_HUP | Glib::IO_ERR | Glib::IO_NVAL);
	add_poll(m_oConnectPollFD);

	set_priority(Glib::PRIORITY_DEFAULT);
	set_can_recurse(false);
}
PendingWriteSource::~PendingWriteSource()
{
	if (m_nClientFD >= 0) {
		::close(m_nClientFD);
	}
//std::cout << "PendingWriteSource::~PendingWriteSource()" << '\n';
}
int32_t PendingWriteSource::removePoll()
{
	const auto nFD = m_nClientFD;
	m_nClientFD = -1;
	remove_poll(m_oConnectPollFD);
	return nFD;
}
sigc::connection PendingWriteSource::connect(const sigc::slot<bool, int32_t, bool>& oSlot)
{
	if (m_nClientFD < 0) {
		// File error, return an empty connection
		return sigc::connection();
	}
	return connect_generic(oSlot);
}

bool PendingWriteSource::prepare(int& nTimeout)
{
	nTimeout = -1;

	return false;
}
bool PendingWriteSource::check()
{
	bool bRet = false;

	if ((m_oConnectPollFD.get_revents() & (Glib::IO_OUT | Glib::IO_HUP | Glib::IO_ERR | Glib::IO_NVAL)) != 0) {
		bRet = true;
	}

	return bRet;
}
bool PendingWriteSource::dispatch(sigc::slot_base* p0Slot)
{
	bool bContinue = true;

	if (p0Slot == nullptr) {
		return ! bContinue; //--------------------------------------------------
	}
//std::cout << "WaitingConnectSource::dispatch" << '\n';

	auto nIOFlags = m_oConnectPollFD.get_revents();
	const bool bSomeError = ((nIOFlags & (Glib::IO_HUP | Glib::IO_ERR | Glib::IO_NVAL)) != 0);
	if (((nIOFlags & Glib::IO_OUT) == 0) && !bSomeError) {
		// unknown event
		return ! bContinue; //--------------------------------------------------
	}
	bContinue = (*static_cast<sigc::slot<bool, int32_t, bool>*>(p0Slot))(m_nUniqueId, bSomeError);

	return bContinue;
}

////////////////////////////////////////////////////////////////////////////////
IntervalTimeoutSource::IntervalTimeoutSource(int32_t nInterval)
: Glib::TimeoutSource(nInterval)
, m_nTotMillisec(0)
, m_nInterval(nInterval)
{
	assert(nInterval > 0);
}
IntervalTimeoutSource::~IntervalTimeoutSource()
{
//std::cout << "IntervalTimeoutSource::~IntervalTimeoutSource()" << '\n';
}
sigc::connection IntervalTimeoutSource::connectSlot(const sigc::slot<bool>& oSlot)
{
	m_oCallback = oSlot;
	const sigc::slot<bool> oNoParamSlot = sigc::mem_fun(this, &IntervalTimeoutSource::callback);
	return connect_generic(oNoParamSlot);
}
bool IntervalTimeoutSource::callback()
{
	if (!m_oCallback) {
		return false;
	}
	m_nTotMillisec += m_nInterval;
	return m_oCallback();
}

} // namespace stmi
