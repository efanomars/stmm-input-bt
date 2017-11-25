/*
 * Copyright © 2016-2017  Stefano Marsili, <stemars@gmx.ch>
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
 * File:   gdkkeyconverterevdev.cc
 */

#include "gdkkeyconverterevdev.h"

#include <cassert>

/* Evdev formula from Daniel Berrange's https://www.berrange.com/tags/key-codes/ */

namespace stmi
{

const shared_ptr<GdkKeyConverterEvDev>& GdkKeyConverterEvDev::getConverter()
{
	static auto s_refConverter = shared_ptr<GdkKeyConverterEvDev>(new GdkKeyConverterEvDev());
	return s_refConverter;
}

bool GdkKeyConverterEvDev::convertKeyCodeToHardwareKey(guint16 nGdkKeycode, HARDWARE_KEY& eHardwareKey) const
{
	const int32_t nLinuxCode = nGdkKeycode - 8;
	if (nLinuxCode < 0) {
		return false;
	}
	eHardwareKey = static_cast<HARDWARE_KEY>(nLinuxCode);
	return true;
}
bool GdkKeyConverterEvDev::convertEventKeyToHardwareKey(GdkEventKey const* p0GdkEvent, HARDWARE_KEY& eHardwareKey) const
{
	assert(p0GdkEvent != nullptr);
	return convertKeyCodeToHardwareKey(p0GdkEvent->hardware_keycode, eHardwareKey);
}

} // namespace stmi
