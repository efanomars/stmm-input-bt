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
 * File:   weightdialog.cc
 */

#include "weightdialog.h"

#include <cassert>
#include <iostream>


namespace stmi
{

WeightDialog::WeightDialog(int32_t nMinWeight, int32_t nMaxWeight)
: Gtk::Dialog()
, m_p0SpinWeight(nullptr)
, m_nOldWeight(m_nMinWeight)
, m_nWeight(m_nMinWeight)
, m_nMinWeight(nMinWeight)
, m_nMaxWeight(nMaxWeight)
{
	//
	set_title("Choose weight");
	set_default_size(s_nInitialWindowSizeW, s_nInitialWindowSizeH);
	set_resizable(true);

	m_refAdjustmentWeight = Gtk::Adjustment::create(m_nMinWeight, m_nMinWeight, m_nMaxWeight, 1, 1, 0);

	add_button("Cancel", s_nRetCancel);
	add_button("Ok", s_nRetOk);
	set_default_response(s_nRetOk);

	Gtk::Box* p0ContentArea = get_content_area();
	assert(p0ContentArea != nullptr);
//std::cout << "WeightDialog::WeightDialog() content area children: " << p0ContentArea->get_children().size() << '\n';
	Gtk::Box* m_p0VBoxWeight = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_VERTICAL));
	p0ContentArea->pack_start(*m_p0VBoxWeight, false, false);
		m_p0VBoxWeight->set_spacing(5);
		m_p0LabelWeightDesc = Gtk::manage(new Gtk::Label("Weight of column/row"));
		m_p0VBoxWeight->pack_start(*m_p0LabelWeightDesc, false, false);
			m_p0LabelWeightDesc->set_alignment(Gtk::Align::ALIGN_START, m_p0LabelWeightDesc->property_valign());
		m_p0SpinWeight = Gtk::manage(new Gtk::SpinButton(m_refAdjustmentWeight));
		m_p0VBoxWeight->pack_start(*m_p0SpinWeight, false, false);
			m_p0SpinWeight->set_activates_default(true);
			m_p0SpinWeight->signal_value_changed().connect(
							sigc::mem_fun(*this, &WeightDialog::onSpinWeightChanged) );

	this->show_all();
}
WeightDialog::~WeightDialog()
{
}
int WeightDialog::run(int32_t nWeight, bool bColumn, int32_t nColRow)
{
	assert((nWeight >= m_nMinWeight) && (nWeight <= m_nMaxWeight));
	assert(nColRow >= 0);
	m_nOldWeight = nWeight;
	m_nWeight = nWeight;
	m_refAdjustmentWeight->set_value(nWeight);
	m_p0LabelWeightDesc->set_label(std::string("Weight of ") + (bColumn ? "column" : "row") + " " + std::to_string(nColRow));
	return Gtk::Dialog::run();
}
int32_t WeightDialog::getWeight() const
{
	return m_nWeight;
}
void WeightDialog::onSpinWeightChanged()
{
//std::cout << "WeightDialog::onSpinWeightChanged()" << '\n';
	m_nWeight = m_p0SpinWeight->get_value_as_int();
}

} // namespace stmi

