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
 * File:   fakebtgtkbackend.h
 */

#ifndef STMI_TESTING_FAKE_BT_GTK_BACKEND_H
#define STMI_TESTING_FAKE_BT_GTK_BACKEND_H

#include "btgtkbackend.h"

#include <stmm-input-ev/keyevent.h>
#include <stmm-input/hardwarekey.h>

#include <bluetooth/bluetooth.h>

#include <memory>
#include <string>
#include <vector>
#include <cassert>

#include <stdint.h>

namespace stmi { class BtGtkDeviceManager; }

namespace stmi
{

using std::shared_ptr;
using std::weak_ptr;

namespace testing
{
namespace Bt
{

class FakeGtkBackend : public Private::Bt::GtkBackend
{
public:
	explicit FakeGtkBackend(::stmi::BtGtkDeviceManager* p0Owner) noexcept;
	//
	int32_t simulateNewDevice(const bdaddr_t& oBdAddr) noexcept
	{
		#ifndef NDEBUG
		{
		const int32_t nBackendId = getBackendId(m_aDeviceAddrs, oBdAddr);
		assert(nBackendId < 0);
		}
		#endif //NDEBUG
		const int32_t nBackendId = static_cast<int32_t>(m_aDeviceAddrs.size());
		m_aDeviceAddrs.push_back(oBdAddr);
		onDeviceAdded(getBdAddrAsString(oBdAddr), nBackendId);
		return nBackendId;
	}
	void simulateRemoveDevice(int32_t nBackendId) noexcept
	{
		assert((nBackendId >= 0) && (nBackendId < static_cast<int32_t>(m_aDeviceAddrs.size())));
		assert(! isEmptyBdAddr(m_aDeviceAddrs[nBackendId]));
		onDeviceRemoved(nBackendId);
	}
	// returns bContinue
	bool simulateKeyEvent(int32_t nBackendId, KeyEvent::KEY_INPUT_TYPE eType, HARDWARE_KEY eHK) noexcept
	{
		return onBlueKey(nBackendId, eType, eHK);
	}

	static bdaddr_t getBdAddrFromString(const std::string& sBtAddr) noexcept;
private:
	std::vector< bdaddr_t > m_aDeviceAddrs; // Index: nBackendId
};

} // namespace Bt
} // namespace testing

} // namespace stmi

#endif /* STMI_TESTING_FAKE_BT_GTK_BACKEND_H */
