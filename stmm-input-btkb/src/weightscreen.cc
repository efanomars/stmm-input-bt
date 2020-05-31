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
 * File:   weightscreen.cc
 */

#include "weightscreen.h"

#include "btkbwindow.h"

#include "gtkutilpriv.h"

#include <cassert>
//#include <iostream>
#include <string>


namespace stmi
{

static constexpr const int32_t s_nButtonLeftRightMargin = 20;

WeightScreen::WeightScreen(BtkbWindow& oBtkbWindow, int32_t nMinWeight, int32_t nMaxWeight) noexcept
: m_oBtkbWindow(oBtkbWindow)
, m_nOldWeight(nMinWeight)
, m_nWeight(nMinWeight)
, m_nMinWeight(nMinWeight)
, m_nMaxWeight(nMaxWeight)
{
}
Gtk::Widget* WeightScreen::init() noexcept
{
	m_refAdjustmentWeight = Gtk::Adjustment::create(m_nMinWeight, m_nMinWeight, m_nMaxWeight, 1, 1, 0);

	m_p0VBoxWeight = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_VERTICAL));
		m_p0VBoxWeight->set_spacing(5);
		addBigSeparator(m_p0VBoxWeight, true);
		m_p0LabelWeightDesc = Gtk::manage(new Gtk::Label("Weight of column/row"));
		m_p0VBoxWeight->pack_start(*m_p0LabelWeightDesc, false, false);
			m_p0LabelWeightDesc->set_alignment(Gtk::Align::ALIGN_START, m_p0LabelWeightDesc->property_valign());
			m_p0LabelWeightDesc->set_margin_left(s_nButtonLeftRightMargin);
			m_p0LabelWeightDesc->set_margin_right(s_nButtonLeftRightMargin);
		m_p0SpinWeight = Gtk::manage(new Gtk::SpinButton(m_refAdjustmentWeight));
		m_p0VBoxWeight->pack_start(*m_p0SpinWeight, false, false);
			m_p0SpinWeight->set_activates_default(true);
			m_p0SpinWeight->set_margin_left(s_nButtonLeftRightMargin);
			m_p0SpinWeight->set_margin_right(s_nButtonLeftRightMargin);
			m_p0SpinWeight->signal_value_changed().connect(
							sigc::mem_fun(*this, &WeightScreen::onSpinWeightChanged) );
		addBigSeparator(m_p0VBoxWeight);
		Gtk::Button* m_p0ButtonAnswerOk = Gtk::manage(new Gtk::Button("Ok"));
		m_p0VBoxWeight->pack_start(*m_p0ButtonAnswerOk, false, false);
			m_p0ButtonAnswerOk->set_margin_left(s_nButtonLeftRightMargin);
			m_p0ButtonAnswerOk->set_margin_right(s_nButtonLeftRightMargin);
			m_p0ButtonAnswerOk->set_margin_top(5);
			m_p0ButtonAnswerOk->set_margin_bottom(5);
			m_p0ButtonAnswerOk->signal_clicked().connect(
							sigc::mem_fun(*this, &WeightScreen::onButtonAnswerOk) );
		Gtk::Button* m_p0ButtonAnswerCancel = Gtk::manage(new Gtk::Button("Cancel"));
		m_p0VBoxWeight->pack_start(*m_p0ButtonAnswerCancel, false, false);
			m_p0ButtonAnswerCancel->set_margin_left(s_nButtonLeftRightMargin);
			m_p0ButtonAnswerCancel->set_margin_right(s_nButtonLeftRightMargin);
			m_p0ButtonAnswerCancel->set_margin_top(5);
			m_p0ButtonAnswerCancel->set_margin_bottom(5);
			m_p0ButtonAnswerCancel->signal_clicked().connect(
							sigc::mem_fun(*this, &WeightScreen::onButtonAnswerCancel) );
		addBigSeparator(m_p0VBoxWeight, true);

	return m_p0VBoxWeight;
}
bool WeightScreen::changeTo(int32_t nWeight, bool bColumn, int32_t nColRow, const Gtk::TreeModel::Path& oPath) noexcept
{
	assert((nWeight >= m_nMinWeight) && (nWeight <= m_nMaxWeight));
	assert(nColRow >= 0);
	m_bColumn = bColumn;
	m_nColRow = nColRow;
	m_oPath = oPath;
	//
	m_nOldWeight = nWeight;
	m_nWeight = nWeight;
	m_refAdjustmentWeight->set_value(nWeight);
	m_p0LabelWeightDesc->set_label(std::string("Weight of ") + (bColumn ? "column" : "row") + " " + std::to_string(nColRow));
	return true;
}
void WeightScreen::onButtonAnswerCancel() noexcept
{
	m_oBtkbWindow.changeScreen(m_oBtkbWindow.s_nScreenMain, "");
}
void WeightScreen::onButtonAnswerOk() noexcept
{
	m_oBtkbWindow.setWeight(m_nWeight, m_bColumn, m_nColRow, m_oPath);
	m_oBtkbWindow.changeScreen(m_oBtkbWindow.s_nScreenMain, "");
}
void WeightScreen::onSpinWeightChanged() noexcept
{
//std::cout << "WeightScreen::onSpinWeightChanged()" << '\n';
	m_nWeight = m_p0SpinWeight->get_value_as_int();
}

} // namespace stmi

