/*
 * Copyright © 2017-2020  Stefano Marsili, <stemars@gmx.ch>
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
 * File:   keypacket.h
 */

#ifndef STMI_BT_KEY_PACKET_H
#define STMI_BT_KEY_PACKET_H

#include <stdint.h>

namespace stmi
{

// Make sure the contents of this file are the same as
// libstmm-input-gtk-bt/src/keypacket.h

enum PACKET_CMD {
	PACKET_CMD_KEY = 0
	, PACKET_CMD_NOOP = 1
//	, PACKET_CMD_DISCONNECT_DEVICE = 2 // unused by this client TODO probably not needed
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

} // namespace stmi

#endif /* STMI_BT_KEY_PACKET_H */
