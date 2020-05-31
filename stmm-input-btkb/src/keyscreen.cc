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
 * File:   keyscreen.cc
 */

#include "keyscreen.h"
#include "inputstrings.h"

#include "btkbwindow.h"

#include "gtkutilpriv.h"

#include <cassert>
//#include <iostream>
#include <algorithm>
#include <vector>


namespace stmi
{

static constexpr const int32_t s_nButtonLeftRightMargin = 20;

KeyScreen::KeyScreen(BtkbWindow& oBtkbWindow, InputStrings& oInputStrings) noexcept
: m_oBtkbWindow(oBtkbWindow)
, m_p0LabelChooseKey(nullptr)
, m_oInputStrings(oInputStrings)
{
}
Gtk::Widget* KeyScreen::init() noexcept
{
	Gtk::Box* m_p0VBoxChooseKey = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_VERTICAL));
		m_p0VBoxChooseKey->set_spacing(5);
		addBigSeparator(m_p0VBoxChooseKey);

		Glib::RefPtr<Gtk::TreeStore> refTreeStoreKeys = Gtk::TreeStore::create(m_oKeyColumns);
			refTreeStoreKeys->clear();
			int32_t nMaxKeyNameLen = 0;
			for (const auto eKey : m_oInputStrings.getAllKeys()) {
				auto itKey = refTreeStoreKeys->append();
				const std::string& sKeyName = m_oInputStrings.getKeyString(eKey);
				(*itKey)[m_oKeyColumns.m_oName] = sKeyName;
				(*itKey)[m_oKeyColumns.m_oHK] = static_cast<int32_t>(eKey);
				const int32_t nKeyNameLen = static_cast<int32_t>(sKeyName.size());
				if (nKeyNameLen > nMaxKeyNameLen) {
					nMaxKeyNameLen = nKeyNameLen;
				}
			}
		Gtk::Label* m_p0LabelKeys = Gtk::manage(new Gtk::Label("Available keys"));
		m_p0VBoxChooseKey->pack_start(*m_p0LabelKeys, false, false);
		Gtk::ScrolledWindow* m_p0ScrolledKey = Gtk::manage(new Gtk::ScrolledWindow());
		m_p0VBoxChooseKey->pack_start(*m_p0ScrolledKey, true, true);
			m_p0ScrolledKey->set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_ALWAYS);
			m_p0ScrolledKey->set_margin_top(3);
			m_p0ScrolledKey->set_margin_left(s_nButtonLeftRightMargin);
			m_p0ScrolledKey->set_margin_right(s_nButtonLeftRightMargin);
			//Create the Keys tree model
			m_refTreeModelKeys = refTreeStoreKeys;
			m_p0TreeViewKey = Gtk::manage(new Gtk::TreeView(m_refTreeModelKeys));
			m_p0ScrolledKey->add(*m_p0TreeViewKey);
				Glib::RefPtr<Gtk::TreeSelection> refTreeSelection;
				m_p0TreeViewKey->append_column("Key", m_oKeyColumns.m_oName);
				m_p0TreeViewKey->append_column("Numeric", m_oKeyColumns.m_oHK);
				refTreeSelection = m_p0TreeViewKey->get_selection();
				refTreeSelection->signal_changed().connect(
								sigc::mem_fun(*this, &KeyScreen::onKeySelectionChanged));
		addBigSeparator(m_p0VBoxChooseKey);
		m_p0LabelChooseKey = Gtk::manage(new Gtk::Label("Key of column/row"));
		m_p0VBoxChooseKey->pack_start(*m_p0LabelChooseKey, false, false);
			m_p0LabelChooseKey->set_alignment(Gtk::Align::ALIGN_START, m_p0LabelChooseKey->property_valign());
			m_p0LabelChooseKey->set_margin_left(s_nButtonLeftRightMargin);
			m_p0LabelChooseKey->set_margin_right(s_nButtonLeftRightMargin);
		m_p0EntryKey = Gtk::manage(new Gtk::Entry());
		m_p0VBoxChooseKey->pack_start(*m_p0EntryKey, false, false);
			m_p0EntryKey->set_max_length(nMaxKeyNameLen + 7);
			m_p0EntryKey->set_margin_end(5);
			m_p0EntryKey->set_margin_left(s_nButtonLeftRightMargin);
			m_p0EntryKey->set_margin_right(s_nButtonLeftRightMargin);

		addBigSeparator(m_p0VBoxChooseKey);
		Gtk::Button* m_p0ButtonAnswerOk = Gtk::manage(new Gtk::Button("Ok"));
		m_p0VBoxChooseKey->pack_start(*m_p0ButtonAnswerOk, false, false);
			m_p0ButtonAnswerOk->set_margin_left(s_nButtonLeftRightMargin);
			m_p0ButtonAnswerOk->set_margin_right(s_nButtonLeftRightMargin);
			m_p0ButtonAnswerOk->set_margin_top(5);
			m_p0ButtonAnswerOk->set_margin_bottom(5);
			m_p0ButtonAnswerOk->signal_clicked().connect(
							sigc::mem_fun(*this, &KeyScreen::onButtonAnswerOk) );
		Gtk::Button* m_p0ButtonAnswerCancel = Gtk::manage(new Gtk::Button("Cancel"));
		m_p0VBoxChooseKey->pack_start(*m_p0ButtonAnswerCancel, false, false);
			m_p0ButtonAnswerCancel->set_margin_left(s_nButtonLeftRightMargin);
			m_p0ButtonAnswerCancel->set_margin_right(s_nButtonLeftRightMargin);
			m_p0ButtonAnswerCancel->set_margin_top(5);
			m_p0ButtonAnswerCancel->set_margin_bottom(5);
			m_p0ButtonAnswerCancel->signal_clicked().connect(
							sigc::mem_fun(*this, &KeyScreen::onButtonAnswerCancel) );
		addBigSeparator(m_p0VBoxChooseKey);
	return m_p0VBoxChooseKey;
}
bool KeyScreen::changeTo(hk::HARDWARE_KEY eKey, int32_t nColumn, int32_t nRow, const Gtk::TreeModel::Path& oPath) noexcept
{
	assert(! m_oInputStrings.getKeyString(eKey).empty());
	assert((nColumn >= 0) && (nRow >= 0));
	m_oPath = oPath;
	m_nColumn = nColumn;
	m_nRow = nRow;
	m_sBaseLabelString = std::string("Key of column ") + std::to_string(nColumn) + "  row " + std::to_string(nRow);
	m_p0LabelChooseKey->set_label(m_sBaseLabelString);

	const auto oPair = findKeyRow(eKey);
	#ifndef NDEBUG
	const bool bFound = oPair.first;
	#endif //NDEBUG
	assert(bFound);
	const auto itFind = oPair.second;
	const std::string& sKeyName = (*itFind)[m_oKeyColumns.m_oName];
	m_p0EntryKey->set_text(sKeyName);

	return true;
}
bool KeyScreen::onKeyPressed(GdkEventKey* p0Event) noexcept
{
//std::cout << "KeyScreen::onKeyPressed" << '\n';
	if (m_p0EntryKey->is_focus() && m_p0EntryKey->has_focus()
			&& (p0Event->keyval == GDK_KEY_Return) && ((p0Event->state & GDK_MODIFIER_MASK) == 0)) {
		onButtonAnswerOk();
		return true; // don't propagate to device manager
	}
	// standard: tab, control and so on are used by Gtk
	return false;
}
void KeyScreen::onButtonAnswerCancel() noexcept
{
	m_oBtkbWindow.changeScreen(m_oBtkbWindow.s_nScreenMain, "");
}
void KeyScreen::onButtonAnswerOk() noexcept
{
	std::string sKeyName = m_p0EntryKey->get_text();
	const auto oPair = m_oInputStrings.getStringKey(sKeyName);
	if (! oPair.first) {
		// error
		m_p0LabelChooseKey->set_text(m_sBaseLabelString + "\nInvalid key!");
		Glib::signal_timeout().connect_once(sigc::mem_fun(*this, &KeyScreen::onErrorOneSecond), 1000);
		return; //--------------------------------------------------------------
	}
	const hk::HARDWARE_KEY eKey = oPair.second;
	m_oBtkbWindow.setKey(eKey, m_nColumn, m_nRow, m_oPath);
	m_oBtkbWindow.changeScreen(m_oBtkbWindow.s_nScreenMain, "");
}
void KeyScreen::onErrorOneSecond() noexcept
{
	m_p0LabelChooseKey->set_text(m_sBaseLabelString);
}
std::pair<bool, Gtk::TreeModel::Children::iterator> KeyScreen::findKeyRow(hk::HARDWARE_KEY eKey) const noexcept
{
	auto oChildren = m_refTreeModelKeys->children();
	const auto itFind = std::find_if(oChildren.begin(), oChildren.end(), [&](Gtk::TreeModel::iterator it) {
		if ((*it)[m_oKeyColumns.m_oHK] == eKey) {
			return true;
		}
		return false;
	});
	return std::make_pair((itFind != oChildren.end()), itFind);
}
void KeyScreen::onKeySelectionChanged() noexcept
{
//std::cout << "onKeySelectionChanged()" << '\n';
	Glib::RefPtr<Gtk::TreeSelection> refTreeSelection = m_p0TreeViewKey->get_selection();
	Gtk::TreeModel::iterator it = refTreeSelection->get_selected();
	if (! it)	{
		return; //--------------------------------------------------------------
	}
	Gtk::TreeModel::Row oRow = *it;
	const std::string sKeyName = oRow[m_oKeyColumns.m_oName];
	m_p0EntryKey->set_text(sKeyName);
//std::cout << "onKeySelectionChanged() m_nSelectedTeam=" << m_nSelectedTeam << "  m_nSelectedMate=" << m_nSelectedMate << '\n';
}

} // namespace stmi

