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
 * File:   util.h
 */

#ifndef STMI_BTKB_UTIL_H
#define STMI_BTKB_UTIL_H


#include <string>
#include <vector>

#include <stdint.h>

namespace stmi
{

std::string vectorToString(const std::vector<int32_t>& aArr) noexcept;


void stringToVector(const std::string& sStr, int32_t nMinSize, int32_t nMaxSize
							, int32_t nMinValue, int32_t nMaxValue, int32_t nDefaultValue
							, std::vector<int32_t>& aArr) noexcept;

} // namespace stmi

#endif /* STMI_BTKB_UTIL_H */
