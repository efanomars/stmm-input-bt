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
 * File:   bluetoothsources.h
 */

#ifndef STMI_BLUETOOTH_SOURCES_H
#define STMI_BLUETOOTH_SOURCES_H

#include "keypacket.h"

#include <stmm-input-ev/keyevent.h>
#include <stmm-input/hardwarekey.h>

#include <glibmm.h>

#include <cassert>
#include <vector>
#include <string>

#include <bluetooth/bluetooth.h>
#include <bluetooth/l2cap.h>

namespace stmi
{

namespace Private
{
namespace Bt
{

////////////////////////////////////////////////////////////////////////////////
/** Server.
 */
class BlueServerAcceptSource : public Glib::Source
{
public:
	BlueServerAcceptSource(int32_t nL2capPort);
	virtual ~BlueServerAcceptSource();

	/** Set source's callback function.
	 * The callback has the following signature:
	 *
	 *     bRet = oCallback(nIOFlags, nClientFD, oRemoteAddr);
	 *
	 * nClientFD: The connection file descriptor to the new client.
	 * oRemoteAddr: The bluetooth remote address of the client.
	 * bRet: whether the source should go on listening.
	 *
	 * Note that the file descriptor owner is transferred to the callback.
	 * @param slot The slot.
	 * @return The connection. Is empty if not connected.
	 */
	sigc::connection connect(const sigc::slot<bool, int32_t, const struct ::sockaddr_l2&>& oSlot);

	/** The error string.
	 * @return The error string or empty if server running.
	 */
	const std::string& getErrorStr() const
	{
		return m_sErrorStr;
	}
protected:
	bool prepare(int& nTimeout) override;
	bool check() override;
	bool dispatch(sigc::slot_base* oSlot) override;
private:
	//
	int32_t m_nL2capPort;
	int32_t m_nListenerFD;
	std::string m_sErrorStr;
	//
	Glib::PollFD m_oAcceptPollFD;
	//
private:
	BlueServerAcceptSource(const BlueServerAcceptSource& oSource) = delete;
	BlueServerAcceptSource& operator=(const BlueServerAcceptSource& oSource) = delete;
};

////////////////////////////////////////////////////////////////////////////////
class BlueServerReceiveSource : public Glib::Source
{
public:
	BlueServerReceiveSource(int32_t nBackendId, int32_t nClientFD);
	virtual ~BlueServerReceiveSource();

	// int32_t nBackendId, KeyPacket oPkt
	sigc::connection connect(const sigc::slot<bool, int32_t, bool, const KeyPacket&>& oSlot);
protected:
	bool prepare(int& nTimeout) override;
	bool check() override;
	bool dispatch(sigc::slot_base* oSlot) override;
private:
	bool closeConnection(sigc::slot_base* p0Slot, bool bRemove, KeyPacket& oPkt, const std::string& sErr);
private:
	int32_t m_nBackendId;
	int32_t m_nClientFD;

	Glib::PollFD m_oClientPollFD;
private:
	BlueServerReceiveSource(const BlueServerReceiveSource& oSource) = delete;
	BlueServerReceiveSource& operator=(const BlueServerReceiveSource& oSource) = delete;
};

//////////////////////////////////////////////////////////////////////////////////
//class DevInitTimeoutSource : public Glib::TimeoutSource
//{
//public:
//	DevInitTimeoutSource(int32_t nInterval, const std::string& sDevFileName);
//	virtual ~DevInitTimeoutSource();
//
//	// cover the non-virtual superclass member function which shouldn't be used
//	sigc::connection connect(const sigc::slot<bool>& /*oSlot*/) //override
//	{
//		assert(false);
//		return sigc::connection();
//	}
//	sigc::connection connectSlot(const sigc::slot<bool, const std::string&, int32_t>& oSlot);
//protected:
//	bool callback();
//private:
//	// The signature of the callback means:
//	// bContinue = m_oCallback(sFileName, nTotMillisec)
//	sigc::slot<bool, const std::string&, int32_t> m_oCallback;
//	//
//	const std::string m_sDevFileName;
//	//
//	int32_t m_nInterval;
//	int32_t m_nTotMillisec;
//private:
//	DevInitTimeoutSource() = delete;
//};
//
//
//////////////////////////////////////////////////////////////////////////////////
///* For polling joystick events */
//class JoystickInputSource : public Glib::Source
//{
//public:
//	JoystickInputSource(int32_t nFD, const std::string& sPathName, int64_t nFileSysDeviceId, int32_t nDeviceId);
//	// Closes file descriptor on destruction
//	virtual ~JoystickInputSource();
//
//	// A source can have only one callback type, that is the slot given as parameter
//	sigc::connection connect(const sigc::slot<bool, const struct ::js_event*>& oSlot);
//
//	inline const std::string& getJoystickPathName() const { return m_sPathName; }
//	inline int32_t getJoystickFD() const { return m_oPollFD.get_fd(); }
//	inline int64_t getJoystickFileSysDeviceId() const { return m_nFileSysDeviceId; }
//	inline int32_t getJoystickId() const { return m_nDeviceId; }
//protected:
//	bool prepare(int& timeout) override;
//	bool check() override;
//	bool dispatch(sigc::slot_base* slot) override;
//
//private:
//	//
//	Glib::PollFD m_oPollFD; // The file descriptor is open until destructor is called
//	std::string m_sPathName; // "/dev/input/jsN"
//	//
//	const int64_t m_nFileSysDeviceId; // The stat rdev field (should be unique)
//	const int32_t m_nDeviceId;
//	//
//private:
//	JoystickInputSource(const JoystickInputSource& oSource) = delete;
//	JoystickInputSource& operator=(const JoystickInputSource& oSource) = delete;
//};

} // namespace Bt
} // namespace Private

} // namespace stmi

#endif /* STMI_BLUETOOTH_SOURCES_H */
