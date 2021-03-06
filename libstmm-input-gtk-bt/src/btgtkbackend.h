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
 * File:   btgtkbackend.h
 */

#ifndef STMI_BT_GTK_BACKEND_H
#define STMI_BT_GTK_BACKEND_H

#include "btgtkdevicemanager.h"

#include <stmm-input-ev/keyevent.h>
#include <stmm-input/hardwarekey.h>

#include <gtkmm.h>

#include <memory>
#include <string>
#include <utility>
#include <vector>

#include <bluetooth/bluetooth.h>
#include <bluetooth/l2cap.h>

namespace stmi
{

namespace Private
{
namespace Bt
{

using std::unique_ptr;
using std::shared_ptr;
using std::weak_ptr;

class BlueServerAcceptSource;
class BlueServerReceiveSource;
struct KeyPacket;

////////////////////////////////////////////////////////////////////////////////
class GtkBackend
{
public:
	// returns backend and empty string or null and error string.
	static std::pair<unique_ptr<GtkBackend>, std::string> create(BtGtkDeviceManager* p0Owner, const std::string& sAppName) noexcept;

	virtual ~GtkBackend() noexcept;
protected:
	//friend unique_ptr<GtkBackend> create(BtGtkDeviceManager* p0Owner, const std::string& sAppName);
	GtkBackend(BtGtkDeviceManager* p0Owner, const std::string& sAppName) noexcept;
	// returns error string if failed
	std::string initServer() noexcept;

	// For FakeGtkBackend
	void onDeviceRemoved(int32_t nBackendId) noexcept
	{
		m_p0Owner->onDeviceRemoved(nBackendId);
	}
	// For FakeGtkBackend
	void onDeviceAdded(const std::string& sName, int32_t nBackendId) noexcept
	{
		m_p0Owner->onDeviceAdded(sName, nBackendId);
	}
	bool onBlueKey(int32_t nBackendId, KeyEvent::KEY_INPUT_TYPE eType, HARDWARE_KEY eHK) noexcept
	{
		return m_p0Owner->onBlueKey(nBackendId, eType, eHK);
	}

	// -1 if device unknown
	int32_t getBackendId(const bdaddr_t& oBdAddr) const noexcept;
	// -1 if device not in vector, oBdAddr can be [0,0,0,0,0,0]
	static int32_t getBackendId(const std::vector<bdaddr_t>& aAddrs, const bdaddr_t& oBdAddr) noexcept;
	// sets to 0
	static void resetBdAddr(bdaddr_t& oBdAddr) noexcept;
	// tells whether 0
	static bool isEmptyBdAddr(const bdaddr_t& oBdAddr) noexcept;
	// ba2str wrapper
	static std::string getBdAddrAsString(const bdaddr_t& oBdAddr) noexcept;
private:
		// device has connected
	bool doServerAcceptClient(int32_t nClientFD, const struct sockaddr_l2& oClientAddr) noexcept;
	// data received from device
	bool doServerReceive(int32_t nBackendId, bool bRemove, const KeyPacket& oPkt) noexcept;

private:
	BtGtkDeviceManager* m_p0Owner;
	std::string m_sAppName;

	Glib::RefPtr<BlueServerAcceptSource> m_refServerAccept;

	// Bluetooth devices are only removed if a disconnect command is sent by the client
	// If a device gets disconnected the device manager keeps the device alive
	// while the backend waits for it to reconnect (on a different socket!) but
	// with the unique bluetooth address to identify it!
	// This vector never shrinks, at most a value is set to null when a client
	// disconnects (explicitely or because out of range).
	// This allows to use the index as id.
	std::vector< bdaddr_t > m_aPermanentAddrs;
	std::vector< Glib::RefPtr<BlueServerReceiveSource> > m_aInputSources; // Size: m_aPermanentAddrs.size()

private:
	GtkBackend(const GtkBackend& oSource) = delete;
	GtkBackend& operator=(const GtkBackend& oSource) = delete;
};

} // namespace Bt
} // namespace Private

} // namespace stmi

#endif /* STMI_BT_GTK_BACKEND_H */
