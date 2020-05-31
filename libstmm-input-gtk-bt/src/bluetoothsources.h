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
 * File:   bluetoothsources.h
 */

#ifndef STMI_BLUETOOTH_SOURCES_H
#define STMI_BLUETOOTH_SOURCES_H


#include <glibmm.h>

#include <string>

#include <bluetooth/bluetooth.h>
#include <bluetooth/l2cap.h>

namespace stmi { namespace Private { namespace Bt { struct KeyPacket; } } }

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
	explicit BlueServerAcceptSource(int32_t nL2capPort) noexcept;
	virtual ~BlueServerAcceptSource() noexcept;

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
	sigc::connection connect(const sigc::slot<bool, int32_t, const struct ::sockaddr_l2&>& oSlot) noexcept;

	/** The error string.
	 * @return The error string or empty if server running.
	 */
	const std::string& getErrorStr() const noexcept
	{
		return m_sErrorStr;
	}
protected:
	bool prepare(int& nTimeout) noexcept override;
	bool check() noexcept override;
	bool dispatch(sigc::slot_base* oSlot) noexcept override;
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
	BlueServerReceiveSource(int32_t nBackendId, int32_t nClientFD) noexcept;
	virtual ~BlueServerReceiveSource() noexcept;

	// int32_t nBackendId, KeyPacket oPkt
	sigc::connection connect(const sigc::slot<bool, int32_t, bool, const KeyPacket&>& oSlot) noexcept;
protected:
	bool prepare(int& nTimeout) noexcept override;
	bool check() noexcept override;
	bool dispatch(sigc::slot_base* oSlot) noexcept override;
private:
	bool closeConnection(sigc::slot_base* p0Slot, bool bRemove, KeyPacket& oPkt, const std::string& sErr) noexcept;
private:
	int32_t m_nBackendId;
	int32_t m_nClientFD;

	Glib::PollFD m_oClientPollFD;
private:
	BlueServerReceiveSource(const BlueServerReceiveSource& oSource) = delete;
	BlueServerReceiveSource& operator=(const BlueServerReceiveSource& oSource) = delete;
};

} // namespace Bt
} // namespace Private

} // namespace stmi

#endif /* STMI_BLUETOOTH_SOURCES_H */
