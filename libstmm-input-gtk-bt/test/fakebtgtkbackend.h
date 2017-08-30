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
 * File:   fakebtgtkbackend.h
 */

#ifndef _STMI_TESTING_FAKE_BT_GTK_BACKEND_H_
#define _STMI_TESTING_FAKE_BT_GTK_BACKEND_H_

#include "btgtkbackend.h"

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
	FakeGtkBackend(::stmi::BtGtkDeviceManager* p0Owner);
	//
	int32_t simulateNewDevice(const bdaddr_t& oBdAddr)
	{
		int32_t nBackendId = -1;
		#ifndef NDEBUG
		nBackendId = getBackendId(m_aDeviceAddrs, oBdAddr);
		assert(nBackendId < 0);
		#endif //NDEBUG
		nBackendId = static_cast<int32_t>(m_aDeviceAddrs.size());
		m_aDeviceAddrs.push_back(oBdAddr);
		onDeviceAdded(getBdAddrAsString(oBdAddr), nBackendId);
		return nBackendId;
	}
	void simulateRemoveDevice(int32_t nBackendId)
	{
		assert((nBackendId >= 0) && (nBackendId < static_cast<int32_t>(m_aDeviceAddrs.size())));
		assert(! isEmptyBdAddr(m_aDeviceAddrs[nBackendId]));
		onDeviceRemoved(nBackendId);
	}
	// returns bContinue
	bool simulateKeyEvent(int32_t nBackendId, KeyEvent::KEY_INPUT_TYPE eType, HARDWARE_KEY eHK)
	{
		return onBlueKey(nBackendId, eType, eHK);
	}

	static bdaddr_t getBdAddrFromString(const std::string& sBtAddr);
private:
	std::vector< bdaddr_t > m_aDeviceAddrs; // Index: nBackendId
};

} // namespace Bt
} // namespace testing

} // namespace stmi

#endif /* _STMI_TESTING_FAKE_BT_GTK_BACKEND_H_ */
