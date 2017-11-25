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
 * File:   keyboarddialog.h
 */

#ifndef STMI_KEYBOARD_DIALOG_H
#define STMI_KEYBOARD_DIALOG_H

#include "hardwarekey.h"

#include <gtkmm.h>

#include <string>

namespace stmi
{

class BttestWindow;

class KeyboardDialog : public Gtk::Dialog
{
public:
	KeyboardDialog(BttestWindow& oBttestWindow, bool bFullScreen);
	virtual ~KeyboardDialog();

	int run(bool bFullscreen);

	void connectionStateChanged();
protected:
		// To receive most of the keys, like cursor keys, TAB, ENTER, etc. override default handlers
	bool on_key_press_event(GdkEventKey* /*p0Event*/) override
	{
		return false; // propagate
	}
	bool on_key_release_event(GdkEventKey* /*p0Event*/) override
	{
		return false; // propagate
	}

private:
	bool isWindowActive() const;
	void onSigIsActiveChanged();
	bool onCustomDraw(const Cairo::RefPtr<Cairo::Context>& refCc);
	void drawTextInRect(const Cairo::RefPtr<Cairo::Context>& refCc, int32_t nPixW, int32_t nPixH
						, double fMinRatio, double fMaxRatio, bool bPressed, int32_t nColumn, int32_t nRow);
	void textToLayout(const std::string& sFont, int32_t nFontSize
					, bool bSetWeight, Pango::Weight eWight, bool bSetStyle, Pango::Style eStyle
					, const std::string& sText
					, int32_t& nTextW, int32_t& nTextH);
	void textToLayout(const std::string& sFont, int32_t nFontSize
					, const std::string& sText
					, int32_t& nTextW, int32_t& nTextH);
	double calcRatio(int32_t nPixW, int32_t nPixH, const std::string& sText
					, int32_t& nTextW, int32_t& nTextH);

	void calcCellPositions(int32_t nPixW, int32_t nPixH);
	hk::HARDWARE_KEY getKeyFromCoords(double fX, double fY);

	bool onMouseButtonPressed(GdkEventButton* p0ButtonEvent);
	bool onMouseButtonReleased(GdkEventButton* p0ButtonEvent);
	bool onTouchEvent(GdkEventTouch* p0TouchEvent);
	bool onKeyPressEvent(GdkEventKey* p0KeyEvent);
	bool onKeyReleaseEvent(GdkEventKey* p0KeyEvent);

	void redrawCanvas();

	bool convertKeyCodeToHardwareKey(guint16 nGdkKeycode, hk::HARDWARE_KEY& eHardwareKey) const;

private:
	BttestWindow& m_oBttestWindow;

	sigc::connection m_oIsActiveConn;

	Gtk::DrawingArea* m_p0Canvas;

	std::vector<int32_t> m_aColumnPosX; // Size: m_oBttestWindow.m_nTotColumns + 1
	std::vector<int32_t> m_aRowPosY; // Size: m_oBttestWindow.m_nTotRows + 1

	int32_t m_nCanvasX;
	int32_t m_nCanvasY;
	int32_t m_nExitButtonCenterX;
	int32_t m_nExitButtonCenterY;

	Glib::RefPtr<Pango::Context> m_refPaCtx;
	Glib::RefPtr<Pango::Layout> m_refFontLayout;

	static const std::string s_sSansFontDesc;

	static constexpr int32_t s_nMargin = 3;

	static constexpr int32_t s_nExitButtonPixW = 16;
	static constexpr int32_t s_nExitButtonPixH = 16;

	static constexpr double s_nColorExitButtonConnectedR = 0.0;
	static constexpr double s_nColorExitButtonConnectedG = 0.9;
	static constexpr double s_nColorExitButtonConnectedB = 0.0;

	static constexpr double s_nColorExitButtonDisconnectedR = 0.9;
	static constexpr double s_nColorExitButtonDisconnectedG = 0.0;
	static constexpr double s_nColorExitButtonDisconnectedB = 0.0;

	static constexpr int32_t s_nTextMinPixW = 70;
	static constexpr int32_t s_nTextMinPixH = 15;

	static constexpr double s_fAlign = 0.5;

	static constexpr double s_nColorKeyNormalR = 0.8;
	static constexpr double s_nColorKeyNormalG = 0.8;
	static constexpr double s_nColorKeyNormalB = 0.8;

	static constexpr double s_nColorKeyPressedR = 0.3;
	static constexpr double s_nColorKeyPressedG = 0.3;
	static constexpr double s_nColorKeyPressedB = 0.3;

	static constexpr double s_nColorTextNormalR = 0.0;
	static constexpr double s_nColorTextNormalG = 0.0;
	static constexpr double s_nColorTextNormalB = 0.0;

	static constexpr double s_nColorTextPressedR = 1.0;
	static constexpr double s_nColorTextPressedG = 1.0;
	static constexpr double s_nColorTextPressedB = 1.0;

	//static constexpr int32_t s_nInitialWindowSizeW = 400;
	//static constexpr int32_t s_nInitialWindowSizeH = 400;
private:
	KeyboardDialog() = delete;
};

} // namespace stmi

#endif /* STMI_KEYBOARD_DIALOG_H */

