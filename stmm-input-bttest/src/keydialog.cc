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
 * File:   keydialog.cc
 */

#include "keydialog.h"

#include <cassert>
#include <iostream>


namespace stmi
{

KeyDialog::KeyDialog(InputStrings& oInputStrings)
: Dialog()
, m_p0LabelChooseKey(nullptr)
, m_p0ComboKey(nullptr)
, m_eOldKey(hk::HK_NULL)
, m_eKey(hk::HK_NULL)
, m_oInputStrings(oInputStrings)
{
	//
	set_title("Choose key");
	set_default_size(s_nInitialWindowSizeW, s_nInitialWindowSizeH);
	set_resizable(true);

	add_button("Cancel", s_nRetCancel);
	add_button("Ok", s_nRetOk);

	Gtk::Box* p0ContentArea = get_content_area();
	assert(p0ContentArea != nullptr);
//std::cout << "KeyDialog::KeyDialog() content area children: " << p0ContentArea->get_children().size() << '\n';
	Gtk::Box* m_p0VBoxChooseKey = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_VERTICAL));
	p0ContentArea->pack_start(*m_p0VBoxChooseKey, false, false);
		m_p0VBoxChooseKey->set_spacing(5);
		m_p0LabelChooseKey = Gtk::manage(new Gtk::Label("Key of column/row"));
		m_p0VBoxChooseKey->pack_start(*m_p0LabelChooseKey, false, false);
			m_p0LabelChooseKey->set_alignment(Gtk::Align::ALIGN_START, m_p0LabelChooseKey->property_valign());

		Glib::RefPtr<Gtk::TreeStore> refTreeStoreKeys = Gtk::TreeStore::create(m_oKeyColumns);
			refTreeStoreKeys->clear();
			for (const auto eKey : m_oInputStrings.getAllKeys()) {
				auto itKey = refTreeStoreKeys->append();
				(*itKey)[m_oKeyColumns.m_oName] = m_oInputStrings.getKeyString(eKey);
				(*itKey)[m_oKeyColumns.m_oHiddenHK] = static_cast<int32_t>(eKey);
			}
		m_refTreeModelKeys = refTreeStoreKeys;
		m_p0ComboKey = Gtk::manage(new Gtk::ComboBox(m_refTreeModelKeys, true));
		m_p0VBoxChooseKey->pack_start(*m_p0ComboKey, true, true);
			m_p0ComboKey->set_entry_text_column(m_oKeyColumns.m_oName);
			//m_p0ComboKey->set_focus_on_click(true);
			m_p0ComboKey->signal_changed().connect(
							sigc::mem_fun(*this, &KeyDialog::onComboKeyChanged) );

	show_all();
}
KeyDialog::~KeyDialog()
{
}
int KeyDialog::run(hk::HARDWARE_KEY eKey, int32_t nColumn, int32_t nRow)
{
	assert(eKey != hk::HK_NULL);
	assert((nColumn >= 0) && (nRow >= 0));
	m_eOldKey = eKey;
	m_eKey = eKey;
	m_p0LabelChooseKey->set_label(std::string("Key of column ") + std::to_string(nColumn) + "  row " + std::to_string(nRow));

	const auto oPair = findKeyRow(m_eKey);
	#ifndef NDEBUG
	const bool bFound = oPair.first;
	#endif //NDEBUG
	assert(bFound);
	const auto itFind = oPair.second;
	m_p0ComboKey->set_active(itFind);
	//
	return Gtk::Dialog::run();
}
std::pair<bool, Gtk::TreeModel::Children::iterator> KeyDialog::findKeyRow(hk::HARDWARE_KEY eKey) const
{
	auto oChildren = m_refTreeModelKeys->children();
	const auto itFind = std::find_if(oChildren.begin(), oChildren.end(), [&](Gtk::TreeModel::iterator it) {
		if ((*it)[m_oKeyColumns.m_oHiddenHK] == eKey) {
			return true;
		}
		return false;
	});
	return std::make_pair((itFind != oChildren.end()), itFind);
}
hk::HARDWARE_KEY KeyDialog::getHardwareKey() const
{
	return m_eKey;
}
void KeyDialog::onComboKeyChanged()
{
//std::cout << "KeyDialog::onComboKeyChanged()" << '\n';
	if (m_p0ComboKey->get_active_row_number() < 0) {
		// no item selected
		// let's see whether the entry string is a key name
		const auto sText = m_p0ComboKey->get_entry_text();
		const auto eKey = m_oInputStrings.getStringKey(sText);
		if (eKey == hk::HK_NULL) {
			return;
		}
		const auto oPair = findKeyRow(m_eKey);
		#ifndef NDEBUG
		const bool bFound = oPair.first;
		#endif //NDEBUG
		assert(bFound);
		const auto itFind = oPair.second;
		m_p0ComboKey->set_active(itFind);
		m_eKey = eKey;
	} else {
		auto it = m_p0ComboKey->get_active();
		const int32_t nKey = (*it)[m_oKeyColumns.m_oHiddenHK];
		m_eKey = static_cast<hk::HARDWARE_KEY>(nKey);
	}
}

} // namespace stmi

