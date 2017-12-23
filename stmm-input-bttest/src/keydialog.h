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
 * File:   keydialog.h
 */

#ifndef STMI_KEY_DIALOG_H
#define STMI_KEY_DIALOG_H

#include "inputstrings.h"

#include <gtkmm.h>

#include <string>

namespace stmi
{

class KeyDialog : public Gtk::Dialog
{
public:
	KeyDialog(Gtk::Window& oParent, InputStrings& oInputStrings);
	virtual ~KeyDialog();

	// Call this instead of run()
	// returns either s_nRetOk or s_nRetCancel
	int run(hk::HARDWARE_KEY eKey, int32_t nColumn, int32_t nRow);

	// Call this after run(...) has returned s_nRetOk
	hk::HARDWARE_KEY getHardwareKey() const;

	static constexpr int s_nRetOk = 1;
	static constexpr int s_nRetCancel = 0;
private:
	void onComboKeyChanged();
	void onErrorOneSecond();
	std::pair<bool, Gtk::TreeModel::Children::iterator> findKeyRow(hk::HARDWARE_KEY eKey) const;

private:
	//Gtk::Box* m_p0VBoxChooseKey;
		Gtk::Label* m_p0LabelChooseKey;
		Gtk::ComboBox* m_p0ComboKey;

	////////////
	class KeyColumns : public Gtk::TreeModel::ColumnRecord
	{
	public:
		KeyColumns()
		{
			add(m_oName); add(m_oHiddenHK);
		}
		Gtk::TreeModelColumn<Glib::ustring> m_oName;
		Gtk::TreeModelColumn<int32_t> m_oHiddenHK;
	};
	KeyColumns m_oKeyColumns;
	Glib::RefPtr<Gtk::TreeModel> m_refTreeModelKeys;

	////////////
	hk::HARDWARE_KEY m_eOldKey; 
	hk::HARDWARE_KEY m_eKey;

	std::string m_sBaseLabelString;

	const InputStrings& m_oInputStrings;

	static constexpr int32_t s_nInitialWindowSizeW = 200;
	static constexpr int32_t s_nInitialWindowSizeH = 100;
private:
	KeyDialog() = delete;
};

} // namespace stmi

#endif /* STMI_KEY_DIALOG_H */

