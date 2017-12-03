/*
 * Copyright © 2017  Stefano Marsili, <stemars@gmx.ch>
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
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, see <http://www.gnu.org/licenses/>
 */
/*
 * File:   inputstrings.h
 */

#ifndef STMI_BTTEST_INPUT_STRINGS_H
#define STMI_BTTEST_INPUT_STRINGS_H

#include "hardwarekey.h"

#include <cassert>
#include <string>
#include <algorithm>
#include <unordered_map>

namespace stmi
{

class InputStrings
{
public:
	InputStrings()
	: m_bInitialAdd(true)
	{
		initKeyMap();
		m_bInitialAdd = false;
	}
	/** Get the name string of a key.
	 * @param eKey The key.
	 * @return The name of the key or empty string if not found.
	 */
	inline const std::string& getKeyString(hk::HARDWARE_KEY eKey) const
	{
		auto itFind = m_oKeyStringMap.find(static_cast<int32_t>(eKey));
		if (itFind == m_oKeyStringMap.end()) {
			static const std::string s_sEmpty = "";
			return s_sEmpty;
		}
		return itFind->second;
	}
	const std::vector<hk::HARDWARE_KEY>& getAllKeys() const
	{
		if (m_aAllKeys.empty()) {
			auto p0This = const_cast<InputStrings*>(this);
			p0This->initKeyMap();
		}
		return m_aAllKeys;
	}
	/** Get the name corresponding to a key.
	 * @param sName The name of the key.
	 * @return The (true, key) or (false, hk::HK_NULL) if not found.
	 */
	std::pair<bool, hk::HARDWARE_KEY> getStringKey(const std::string& sName) const
	{
		auto itFind = std::find_if(m_oKeyStringMap.begin(), m_oKeyStringMap.end()
									, [&](const std::pair<int32_t, std::string>& oPair)
		{
			return (oPair.second == sName);
		});
		if (itFind == m_oKeyStringMap.end()) {
			return std::make_pair(false, hk::HK_NULL);
		}
		return std::make_pair(true, static_cast<hk::HARDWARE_KEY>(itFind->first));
	}
private:
	inline void addKey(hk::HARDWARE_KEY eKey, const std::string& sStr)
	{
		if (m_bInitialAdd) {
			m_oKeyStringMap.emplace(static_cast<int32_t>(eKey), sStr);
		} else {
			m_aAllKeys.push_back(eKey);
		}
	}

	void initKeyMap();
private:
	bool m_bInitialAdd;
	std::unordered_map<int32_t, std::string> m_oKeyStringMap;
	std::vector<hk::HARDWARE_KEY> m_aAllKeys;
};

} // namespace stmi

#endif /* STMI_BTTEST_INPUT_STRINGS_H */
