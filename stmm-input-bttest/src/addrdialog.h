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
 * File:   addrdialog.h
 */

#ifndef STMI_ADDR_DIALOG_H
#define STMI_ADDR_DIALOG_H

#include <gtkmm.h>

#include <string>

namespace stmi
{

class AddrDialog : public Gtk::Dialog
{
public:
	AddrDialog();
	virtual ~AddrDialog();

	// Call this instead of run()
	int run(const std::string& sAddr);

	// Call this after run(...) has returned s_nRetOk
	// The returned address is always valid
	const std::string& getAddr() const;

	static constexpr int s_nRetOk = 1;
	static constexpr int s_nRetCancel = 0;
private:
	void onEntryAddrChanged();
	void onErrorOneSecond();
private:
	//Gtk::Box* m_p0VBoxAddr;
		Gtk::Label* m_p0LabelAddrDesc;
		Gtk::Entry* m_p0EntryAddr;

	Glib::RefPtr<Gtk::EntryBuffer> m_refEntryBuffer;
	////////////
	std::string m_sOldAddr; 
	std::string m_sAddr;

	static const std::string s_sLabelStringNormal;
	static const std::string s_sLabelStringError;

	static constexpr int32_t s_nInitialWindowSizeW = 200;
	static constexpr int32_t s_nInitialWindowSizeH = 100;
};

} // namespace stmi

#endif /* STMI_ADDR_DIALOG_H */

