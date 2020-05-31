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
 * File:   keyscreen.h
 */

#ifndef STMI_KEY_SCREEN_H
#define STMI_KEY_SCREEN_H

#include "hardwarekey.h"

#include <gtkmm.h>

#include <string>
#include <utility>

namespace stmi { class InputStrings; }

namespace stmi
{

class BtkbWindow;

class KeyScreen
{
public:
	KeyScreen(BtkbWindow& oBtkbWindow, InputStrings& oInputStrings) noexcept;

	// returns widget to add to container
	Gtk::Widget* init() noexcept;

	// returns false if failed
	bool changeTo(hk::HARDWARE_KEY eKey, int32_t nColumn, int32_t nRow, const Gtk::TreeModel::Path& oPath) noexcept;

	bool onKeyPressed(GdkEventKey* p0Event) noexcept;

private:
	void onKeySelectionChanged() noexcept;
	void onErrorOneSecond() noexcept;
	std::pair<bool, Gtk::TreeModel::Children::iterator> findKeyRow(hk::HARDWARE_KEY eKey) const noexcept;
	void onButtonAnswerOk() noexcept;
	void onButtonAnswerCancel() noexcept;

private:
	BtkbWindow& m_oBtkbWindow;
	Gtk::Box* m_p0VBoxChooseKey;
		Gtk::Label* m_p0LabelChooseKey;
		Gtk::Entry* m_p0EntryKey;
		//Gtk::ScrolledWindow* m_p0ScrolledKey;
			Gtk::TreeView* m_p0TreeViewKey;

	////////////
	class KeyColumns : public Gtk::TreeModel::ColumnRecord
	{
	public:
		KeyColumns() noexcept
		{
			add(m_oName); add(m_oHK);
		}
		Gtk::TreeModelColumn<std::string> m_oName;
		Gtk::TreeModelColumn<int32_t> m_oHK;
	};
	KeyColumns m_oKeyColumns;
	Glib::RefPtr<Gtk::TreeModel> m_refTreeModelKeys;

	////////////
	Gtk::TreeModel::Path m_oPath; // just stored for callback
	int32_t m_nColumn; // just stored for callback
	int32_t m_nRow; // just stored for callback

	std::string m_sBaseLabelString;

	const InputStrings& m_oInputStrings;
private:
	KeyScreen() = delete;
};

} // namespace stmi

#endif /* STMI_KEY_SCREEN_H */

