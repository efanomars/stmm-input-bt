/*
 * Copyright © 2020  Stefano Marsili, <stemars@gmx.ch>
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
 * File:   addrscreen.h
 */

#ifndef STMI_ADDR_SCREEN_H
#define STMI_ADDR_SCREEN_H

#include <gtkmm.h>

#include <string>

#include <stdint.h>

namespace stmi
{

class BtkbWindow;

class AddrScreen
{
public:
	explicit AddrScreen(BtkbWindow& oBtkbWindow) noexcept;

	// returns widget to add to container
	Gtk::Widget* init() noexcept;

	// returns false if failed
	bool changeTo(const std::string& sAddr) noexcept;

	static constexpr int s_nRetOk = 1;
	static constexpr int s_nRetCancel = 0;
private:
	void onEntryAddrChanged() noexcept;
	void onErrorOneSecond() noexcept;
	void onButtonAnswerOk() noexcept;
	void onButtonAnswerCancel() noexcept;
private:
	BtkbWindow& m_oBtkbWindow;
	Gtk::Box* m_p0VBoxAddr = nullptr;
		Gtk::Label* m_p0LabelAddrDesc = nullptr;
		Gtk::Entry* m_p0EntryAddr = nullptr;
		Gtk::Button* m_p0ButtonAnswerOk = nullptr;
		Gtk::Button* m_p0ButtonAnswerCancel = nullptr;

	Glib::RefPtr<Gtk::EntryBuffer> m_refEntryBuffer;
	////////////
	std::string m_sOldAddr;
	std::string m_sAddr;

	static const std::string s_sLabelStringNormal;
	static const std::string s_sLabelStringError;
};

} // namespace stmi

#endif /* STMI_ADDR_SCREEN_H */

