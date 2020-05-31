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
 * File:   btclientsources.h
 */

#ifndef STMI_BT_CLIENT_SOURCES_H
#define STMI_BT_CLIENT_SOURCES_H

#include <glibmm.h>

#include <cassert>

#include <stdint.h>


namespace stmi
{

////////////////////////////////////////////////////////////////////////////////
/* Waits for a write operation (connect or a send) to complete.
 */
class PendingWriteSource : public Glib::Source
{
public:
	// nClientFD The socket for which non-blocking connect or send was called
	explicit PendingWriteSource(int32_t nClientFD) noexcept;
	virtual ~PendingWriteSource() noexcept;

	// A source can have only one callback type, that is the slot given as parameter.
	//   bool = m_oCallback(nSourceId, bDisconnect)
	// nSourceId The unique source id 
	// bDisconnect Tells whether the connect succeeded.
	sigc::connection connect(const sigc::slot<bool, int32_t, bool>& oSlot) noexcept;

	int32_t getSourceId() const noexcept { return m_nUniqueId; }

	int32_t removePoll() noexcept;
protected:
	bool prepare(int& nTimeout) noexcept override;
	bool check() noexcept override;
	bool dispatch(sigc::slot_base* oSlot) noexcept override;

private:
	int32_t m_nUniqueId;
	//
	int32_t m_nClientFD;
	//
	Glib::PollFD m_oConnectPollFD;
	//
private:
	PendingWriteSource() = delete;
	PendingWriteSource(const PendingWriteSource& oSource) = delete;
	PendingWriteSource& operator=(const PendingWriteSource& oSource) = delete;
};

////////////////////////////////////////////////////////////////////////////////
class IntervalTimeoutSource : public Glib::TimeoutSource
{
public:
	// nInterval The interval in milliseconds betwwen callbacks.
	explicit IntervalTimeoutSource(int32_t nInterval) noexcept;
	virtual ~IntervalTimeoutSource() noexcept;

	int64_t getElapsed() const noexcept { return m_nTotMillisec; }
	sigc::connection connect(const sigc::slot<bool>& /*oSlot*/) noexcept //override
	{
		assert(false);
		return sigc::connection();
	}
	// The signature of the callback means:
	//   bContinue = m_oCallback(nTotMillisec)
	// nTotMillisec The total elapsed time since the timeout was started
	sigc::connection connectSlot(const sigc::slot<bool>& oSlot) noexcept;
protected:
	bool callback() noexcept;
private:
	sigc::slot<bool> m_oCallback;
	//
	int64_t m_nTotMillisec;
	int32_t m_nInterval;
private:
	IntervalTimeoutSource() = delete;
	IntervalTimeoutSource(const IntervalTimeoutSource& oSource) = delete;
	IntervalTimeoutSource& operator=(const IntervalTimeoutSource& oSource) = delete;
};

} // namespace stmi

#endif /* STMI_BT_CLIENT_SOURCES_H */
