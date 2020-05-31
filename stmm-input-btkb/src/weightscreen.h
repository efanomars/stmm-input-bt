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
 * File:   weightdialog.h
 */

#ifndef STMI_WEIGHT_SCREEN_H
#define STMI_WEIGHT_SCREEN_H

#include <gtkmm.h>

#include <stdint.h>

namespace stmi
{

class BtkbWindow;

class WeightScreen
{
public:
	explicit WeightScreen(BtkbWindow& oBtkbWindow, int32_t nMinWeight, int32_t nMaxWeight) noexcept;

	// returns widget to add to container
	Gtk::Widget* init() noexcept;

	// returns false if failed
	bool changeTo(int32_t nWeight, bool bColumn, int32_t nColRow, const Gtk::TreeModel::Path& oPath) noexcept;
private:
	void onSpinWeightChanged() noexcept;
	void onButtonAnswerOk() noexcept;
	void onButtonAnswerCancel() noexcept;

private:
	Glib::RefPtr<Gtk::Adjustment> m_refAdjustmentWeight;

	BtkbWindow& m_oBtkbWindow;
	Gtk::Box* m_p0VBoxWeight;
		Gtk::Label* m_p0LabelWeightDesc;
		Gtk::SpinButton* m_p0SpinWeight;
		Gtk::Button* m_p0ButtonAnswerOk = nullptr;
		Gtk::Button* m_p0ButtonAnswerCancel = nullptr;

	////////////
	Gtk::TreeModel::Path m_oPath; // just stored for callback
	bool m_bColumn; // just stored for callback
	int32_t m_nColRow; // just stored for callback
	//
	int32_t m_nOldWeight; 
	int32_t m_nWeight;
	const int32_t m_nMinWeight;
	const int32_t m_nMaxWeight;
private:
	WeightScreen() = delete;
};

} // namespace stmi

#endif /* STMI_WEIGHT_SCREEN_H */

