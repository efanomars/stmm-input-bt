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
 * File:   keypacket.h
 */

#ifndef _STMI_BT_KEY_PACKET_H_
#define _STMI_BT_KEY_PACKET_H_

#include <stdint.h>

namespace stmi
{

namespace Private
{
namespace Bt
{

enum PACKET_CMD {
	PACKET_CMD_KEY = 0
	, PACKET_CMD_NOOP = 1
//	, PACKET_CMD_DISCONNECT_DEVICE = 2
	, PACKET_CMD_REMOVE_DEVICE = 3
};
struct KeyPacket
{
	char m_nMagic1; // = '7'
	char m_nMagic2; // = 'A'
	char m_nCmd; // PACKET_CMD
	char m_nKeyType; // KeyEvent::KEY_INPUT_TYPE
	int32_t m_nHardwareKey; // HARDWARE_KEY
};

} // namespace Bt
} // namespace Private

} // namespace stmi

#endif /* _STMI_BT_KEY_PACKET_H_ */
