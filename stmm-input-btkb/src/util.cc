/*
 * Copyright © 2016-2019  Stefano Marsili, <stemars@gmx.ch>
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
 * File:   util.cc
 */

#include "util.h"

#include <cassert>
#include <iostream>
#include <type_traits>
#include <utility>
#include <sstream>

namespace stmi
{

std::string vectorToString(const std::vector<int32_t>& aArr) noexcept
{
	if (aArr.empty()) {
		return "";
	}
	const int32_t nArrSize = static_cast<int32_t>(aArr.size());
	std::string sRes = std::to_string(aArr[0]);
	for (int32_t nIdx = 1; nIdx < nArrSize; ++nIdx) {
		sRes += "," + std::to_string(aArr[nIdx]);
	}
	return sRes;
}

/** Separates a string into tokens and passes them to a handler.
 * The tokens are stripped of leading and trailing spaces and can be empty.
 * @param sStr The string to tokenize.
 * @param sSeparator The separator string. Cannot be empty.
 * @param oTokenHandler The function object taking a string as parameter.
 */
template<class TokenHandler>
static void tokenizer(const std::string& sStr, const std::string& sSeparator, TokenHandler oTokenHandler)
{
	std::string sToken;
	std::size_t nCurPos = 0;
	const std::size_t nStrLen = sStr.length();
	const std::size_t nSeparatorLen = sSeparator.length();
	assert(nSeparatorLen > 0);
	while (nCurPos < nStrLen) {
		const std::size_t nSepPos = sStr.find(sSeparator, nCurPos);
		if (nSepPos == std::string::npos) {
			sToken = sStr.substr(nCurPos, nStrLen - nCurPos);
			nCurPos = nStrLen;
		} else {
			sToken = sStr.substr(nCurPos, nSepPos - nCurPos);
			nCurPos = nSepPos + nSeparatorLen;
		}
		oTokenHandler(sToken);
	}
}
/** Converts string to a number.
 * If both bMin and bMax are true then oMin must be \<= oMax.
 * @param sNr The string to convert.
 * @param bMin Whether the number has a minimum.
 * @param oMin The minimum.
 * @param bMax Whether the number has a maximum.
 * @param oMax The maximum.
 * @return The number and true if succeeded or any and false if failed.
 */
template<typename T>
static std::pair<T, bool> strToNumber(const std::string& sNr, bool bMin, T oMin, bool bMax, T oMax)
{
//std::cout << "Util::strToNumber sNr=" << sNr << '\n';
	static_assert(std::is_arithmetic<T>::value, "");
	if (sNr.empty()) {
		return std::make_pair(oMin, false); //----------------------------------
	}
	std::istringstream oISS(sNr);
	T oNr;
	if (!(oISS >> std::skipws >> std::dec >> oNr)) {
		return std::make_pair(oMin, false); //----------------------------------
	}
	if (!oISS.eof()) {
		// at least trailing spaces are present
		std::string sRest;
		// read
		if (!(oISS >> std::skipws >> sRest)) {
			// error or no string (only trailing spaces)
			if (!oISS.eof()) {
				// error
				return std::make_pair(oMin, false); //--------------------------
			}
		} else {
			// string found
			// can't have string after number
			return std::make_pair(oMin, false); //------------------------------
		}
	}
	#ifndef NDEBUG
	if (bMin && bMax) {
		assert(oMin <= oMax);
	}
	#endif //NDEBUG
	if (bMin && (oNr < oMin)) {
		return std::make_pair(oMin, false); //----------------------------------
	}
	if (bMax && (oNr > oMax)) {
		return std::make_pair(oMin, false); //----------------------------------
	}
	return std::make_pair(oNr, true);
}

void stringToVector(const std::string& sStr, int32_t nMinSize, int32_t nMaxSize
							, int32_t nMinValue, int32_t nMaxValue, int32_t nDefaultValue
							, std::vector<int32_t>& aArr) noexcept
{
	assert(nMinSize <= nMaxSize);
	assert(nMaxSize >= 1);
	assert(nMinValue <= nMaxValue);
	assert((nDefaultValue >= nMinValue) && (nDefaultValue <= nMaxValue));
	aArr.clear();
	tokenizer(sStr, ",", [&](const std::string& sToken)
	{
		auto oPair = stmi::strToNumber<int32_t>(sToken, true, nMinValue, true, nMaxValue);
		if (oPair.second) {
			if (static_cast<int32_t>(aArr.size()) < nMaxSize) {
				int32_t nVal = oPair.first;
				if (nVal < nMinValue) {
					nVal = nMinValue;
				} else if (nVal > nMaxValue) {
					nVal = nMaxValue;
				}
				aArr.push_back(nVal);
			}
		}
	});
	while (static_cast<int32_t>(aArr.size()) < nMinSize) {
		aArr.push_back(nDefaultValue);
	}
}

} // namespace stmi
