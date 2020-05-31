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
 * File:   fakebtgtkbackend.cc
 */

#include "fakebtgtkbackend.h"

//#include "fakebtgtkwindowdata.h"
namespace stmi { class BtGtkDeviceManager; }

namespace stmi
{

namespace testing
{
namespace Bt
{

FakeGtkBackend::FakeGtkBackend(::stmi::BtGtkDeviceManager* p0Owner) noexcept
: Private::Bt::GtkBackend(p0Owner, "")
{
}

bdaddr_t FakeGtkBackend::getBdAddrFromString(const std::string& sBtAddr) noexcept
{
	bdaddr_t oResBdAddr;
	#ifndef NDEBUG
	const auto nRes =
	#endif //NDEBUG
	str2ba(sBtAddr.c_str(), &oResBdAddr);
	assert(nRes == 0);
	return oResBdAddr;
}

} // namespace Bt
} // namespace Private

} // namespace stmi
