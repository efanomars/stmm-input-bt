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
 * File:   btgtklistenerextradata.h
 */

#ifndef _STMI_BT_GTK_LISTENER_EXTRA_DATA_H_
#define _STMI_BT_GTK_LISTENER_EXTRA_DATA_H_

#include "btgtkdevicemanager.h"

namespace stmi
{

namespace Private
{
namespace Bt
{

class BtGtkListenerExtraData final : public stmi::BtGtkDeviceManager::ListenerExtraData
{
public:
	void reset() override
	{
		m_aCanceledKeys.clear();
	}
	inline bool isKeyCanceled(int32_t nKey) const
	{
		return std::find(m_aCanceledKeys.begin(), m_aCanceledKeys.end(), nKey) != m_aCanceledKeys.end();
	}
	inline void setKeyCanceled(int32_t nKey)
	{
		m_aCanceledKeys.push_back(nKey);
	}
private:
	std::vector<int32_t> m_aCanceledKeys;
};

} // namespace Bt
} // namespace Private

} // namespace stmi

#endif /* _STMI_BT_GTK_LISTENER_EXTRA_DATA_H_ */
