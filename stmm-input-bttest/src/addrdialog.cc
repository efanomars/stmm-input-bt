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
 * File:   addrdialog.cc
 */

#include "addrdialog.h"
#include "btkeyservers.h"

#include <cassert>
#include <iostream>


namespace stmi
{

const std::string AddrDialog::s_sLabelStringNormal = "Bluetooth address\n(format: 'XX:XX:XX:XX:XX:XX' with X=0-9A-F)";
const std::string AddrDialog::s_sLabelStringError = "Bluetooth address\nSyntax error!";


AddrDialog::AddrDialog(Gtk::Window& oParent)
: Gtk::Dialog()
, m_p0LabelAddrDesc(nullptr)
, m_p0EntryAddr(nullptr)
{
	set_transient_for(oParent);
	//
	set_title("Server address");
	set_default_size(s_nInitialWindowSizeW, s_nInitialWindowSizeH);
	set_resizable(true);

	add_button("Cancel", s_nRetCancel);
	add_button("Ok", s_nRetOk);
	set_default_response(s_nRetOk);

	m_refEntryBuffer = Gtk::EntryBuffer::create();
		m_refEntryBuffer->set_max_length(2 + 1 + 2 + 1 + 2 + 1 + 2 + 1 + 2 + 1 + 2);

	Gtk::Box* p0ContentArea = get_content_area();
	assert(p0ContentArea != nullptr);
//std::cout << "AddrDialog::AddrDialog() content area children: " << p0ContentArea->get_children().size() << '\n';
	Gtk::Box* m_p0VBoxAddr = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_VERTICAL));
	p0ContentArea->pack_start(*m_p0VBoxAddr, false, false);
		m_p0VBoxAddr->set_spacing(5);
		m_p0LabelAddrDesc = Gtk::manage(new Gtk::Label(s_sLabelStringNormal));
		m_p0VBoxAddr->pack_start(*m_p0LabelAddrDesc, false, false);
			m_p0LabelAddrDesc->set_alignment(Gtk::Align::ALIGN_START, m_p0LabelAddrDesc->property_valign());
		m_p0EntryAddr = Gtk::manage(new Gtk::Entry(m_refEntryBuffer));
		m_p0VBoxAddr->pack_start(*m_p0EntryAddr, false, false);
			m_p0EntryAddr->set_activates_default(true);
			m_p0EntryAddr->signal_changed().connect(
							sigc::mem_fun(*this, &AddrDialog::onEntryAddrChanged) );

	this->show_all();
}
AddrDialog::~AddrDialog()
{
}
int AddrDialog::run(const std::string& sAddr)
{
	assert(sAddr.empty() || BtKeyServers::isValidStringAddr(sAddr));
	m_sOldAddr = sAddr;
	m_sAddr = sAddr;
	m_p0EntryAddr->set_text(sAddr);
	int nRet;
	do {
		nRet = Gtk::Dialog::run();
		if (nRet != s_nRetOk) {
			break; //--------
		}
		std::string sAddr = m_p0EntryAddr->get_text();
		if (sAddr.empty() || BtKeyServers::isValidStringAddr(sAddr)) {
			break; //--------
		}
		// error
		m_p0LabelAddrDesc->set_text(s_sLabelStringError);
		Glib::signal_timeout().connect_once(sigc::mem_fun(*this, &AddrDialog::onErrorOneSecond), 1000);
	} while (true);
	return nRet;
}
void AddrDialog::onErrorOneSecond()
{
	m_p0LabelAddrDesc->set_text(s_sLabelStringNormal);
}
const std::string& AddrDialog::getAddr() const
{
	return m_sAddr;
}
void AddrDialog::onEntryAddrChanged()
{
//std::cout << "AddrDialog::onSpinAddrChanged()" << '\n';
	std::string sAddr = m_p0EntryAddr->get_text();
	if (sAddr.empty() || BtKeyServers::isValidStringAddr(sAddr)) {
//std::cout << "AddrDialog::onEntryAddrChanged setting sAddr=" << sAddr << '\n';
		m_sAddr = sAddr;
	}
}

} // namespace stmi

