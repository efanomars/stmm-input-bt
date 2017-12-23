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
 * File:   weightdialog.h
 */

#ifndef STMI_WEIGHT_DIALOG_H
#define STMI_WEIGHT_DIALOG_H

#include <gtkmm.h>

#include <string>

namespace stmi
{

class WeightDialog : public Gtk::Dialog
{
public:
	WeightDialog(Gtk::Window& oParent, int32_t nMinWeight, int32_t nMaxWeight);
	virtual ~WeightDialog();

	// Call this instead of run()
	int run(int32_t nWeight, bool bColumn, int32_t nColRow);

	// Call this after run(...) has returned s_nRetOk
	int32_t getWeight() const;

	static constexpr int s_nRetOk = 1;
	static constexpr int s_nRetCancel = 0;
private:
	void onSpinWeightChanged();

private:
	Glib::RefPtr<Gtk::Adjustment> m_refAdjustmentWeight;

	//Gtk::Box* m_p0VBoxWeight;
		Gtk::Label* m_p0LabelWeightDesc;
		Gtk::SpinButton* m_p0SpinWeight;

	////////////
	int32_t m_nOldWeight; 
	int32_t m_nWeight;
	const int32_t m_nMinWeight;
	const int32_t m_nMaxWeight;

	static constexpr int32_t s_nInitialWindowSizeW = 200;
	static constexpr int32_t s_nInitialWindowSizeH = 100;
private:
	WeightDialog() = delete;
};

} // namespace stmi

#endif /* STMI_WEIGHT_DIALOG_H */

