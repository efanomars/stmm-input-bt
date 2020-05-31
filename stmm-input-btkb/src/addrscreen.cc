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
 * File:   addrscreen.cc
 */

#include "addrscreen.h"

#include "btkeyservers.h"
#include "btkbwindow.h"

#include "gtkutilpriv.h"

#include <cassert>
//#include <iostream>


namespace stmi
{

const std::string AddrScreen::s_sLabelStringNormal = "Bluetooth address\n(format: 'XX:XX:XX:XX:XX:XX' with X=0-9A-F)";
const std::string AddrScreen::s_sLabelStringError = "Bluetooth address\nSyntax error!";

static constexpr const int32_t s_nButtonLeftRightMargin = 20;

AddrScreen::AddrScreen(BtkbWindow& oBtkbWindow) noexcept
: m_oBtkbWindow(oBtkbWindow)
{
}
Gtk::Widget* AddrScreen::init() noexcept
{
	m_refEntryBuffer = Gtk::EntryBuffer::create();
		m_refEntryBuffer->set_max_length(2 + 1 + 2 + 1 + 2 + 1 + 2 + 1 + 2 + 1 + 2);

	m_p0VBoxAddr = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_VERTICAL));
		m_p0VBoxAddr->set_spacing(5);
		addBigSeparator(m_p0VBoxAddr, true);
		m_p0LabelAddrDesc = Gtk::manage(new Gtk::Label(s_sLabelStringNormal));
		m_p0VBoxAddr->pack_start(*m_p0LabelAddrDesc, false, false);
			m_p0LabelAddrDesc->set_alignment(Gtk::Align::ALIGN_START, m_p0LabelAddrDesc->property_valign());
			m_p0LabelAddrDesc->set_margin_left(s_nButtonLeftRightMargin);
			m_p0LabelAddrDesc->set_margin_right(s_nButtonLeftRightMargin);
		m_p0EntryAddr = Gtk::manage(new Gtk::Entry(m_refEntryBuffer));
		m_p0VBoxAddr->pack_start(*m_p0EntryAddr, false, false);
			m_p0EntryAddr->set_activates_default(true);
			m_p0EntryAddr->set_margin_left(s_nButtonLeftRightMargin);
			m_p0EntryAddr->set_margin_right(s_nButtonLeftRightMargin);
			m_p0EntryAddr->signal_changed().connect(
							sigc::mem_fun(*this, &AddrScreen::onEntryAddrChanged) );
		addBigSeparator(m_p0VBoxAddr);
		Gtk::Button* m_p0ButtonAnswerOk = Gtk::manage(new Gtk::Button("Ok"));
		m_p0VBoxAddr->pack_start(*m_p0ButtonAnswerOk, false, false);
			m_p0ButtonAnswerOk->set_margin_left(s_nButtonLeftRightMargin);
			m_p0ButtonAnswerOk->set_margin_right(s_nButtonLeftRightMargin);
			m_p0ButtonAnswerOk->set_margin_top(5);
			m_p0ButtonAnswerOk->set_margin_bottom(5);
			m_p0ButtonAnswerOk->signal_clicked().connect(
							sigc::mem_fun(*this, &AddrScreen::onButtonAnswerOk) );
		Gtk::Button* m_p0ButtonAnswerCancel = Gtk::manage(new Gtk::Button("Cancel"));
		m_p0VBoxAddr->pack_start(*m_p0ButtonAnswerCancel, false, false);
			m_p0ButtonAnswerCancel->set_margin_left(s_nButtonLeftRightMargin);
			m_p0ButtonAnswerCancel->set_margin_right(s_nButtonLeftRightMargin);
			m_p0ButtonAnswerCancel->set_margin_top(5);
			m_p0ButtonAnswerCancel->set_margin_bottom(5);
			m_p0ButtonAnswerCancel->signal_clicked().connect(
							sigc::mem_fun(*this, &AddrScreen::onButtonAnswerCancel) );
		addBigSeparator(m_p0VBoxAddr, true);
	return m_p0VBoxAddr;
}
bool AddrScreen::changeTo(const std::string& sAddr) noexcept
{
	assert(sAddr.empty() || BtKeyServers::isValidStringAddr(sAddr));
	m_sOldAddr = sAddr;
	m_sAddr = sAddr;
	m_p0EntryAddr->set_text(sAddr);
	return true;
}
void AddrScreen::onButtonAnswerCancel() noexcept
{
	m_oBtkbWindow.changeScreen(m_oBtkbWindow.s_nScreenMain, "");
}
void AddrScreen::onButtonAnswerOk() noexcept
{
	std::string sAddr = m_p0EntryAddr->get_text();
	if (sAddr.empty() || BtKeyServers::isValidStringAddr(sAddr)) {
		m_oBtkbWindow.setAddr(sAddr);
		m_oBtkbWindow.changeScreen(m_oBtkbWindow.s_nScreenMain, "");
		return; //--------------------------------------------------------------
	}
	// error
	m_p0LabelAddrDesc->set_text(s_sLabelStringError);
	Glib::signal_timeout().connect_once(sigc::mem_fun(*this, &AddrScreen::onErrorOneSecond), 1000);
}
void AddrScreen::onErrorOneSecond() noexcept
{
	m_p0LabelAddrDesc->set_text(s_sLabelStringNormal);
}
void AddrScreen::onEntryAddrChanged() noexcept
{
//std::cout << "AddrScreen::onSpinAddrChanged()" << '\n';
	std::string sAddr = m_p0EntryAddr->get_text();
	if (sAddr.empty() || BtKeyServers::isValidStringAddr(sAddr)) {
//std::cout << "AddrScreen::onEntryAddrChanged setting sAddr=" << sAddr << '\n';
		m_sAddr = sAddr;
	}
}

} // namespace stmi

