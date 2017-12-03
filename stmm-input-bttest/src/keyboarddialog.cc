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
 * File:   keyboarddialog.cc
 */

#include "keyboarddialog.h"

#include "bttestwindow.h"

#include <cassert>
#include <iostream>


namespace stmi
{

const std::string KeyboardDialog::s_sSansFontDesc = "Sans";

KeyboardDialog::KeyboardDialog(BttestWindow& oBttestWindow, bool bFullScreen)
: Gtk::Dialog()
, m_oBttestWindow(oBttestWindow)
, m_p0Canvas(nullptr)
, m_nCanvasX(0)
, m_nCanvasY(0)
, m_nExitButtonCenterX(std::numeric_limits<int32_t>::min())
, m_nExitButtonCenterY(std::numeric_limits<int32_t>::min())
{
	set_transient_for(oBttestWindow);
	if (!bFullScreen) {
		set_default_size(m_oBttestWindow.get_width(), m_oBttestWindow.get_height());
	} else {
		fullscreen();
	}

	Gtk::Box* p0ContentArea = get_content_area();
	assert(p0ContentArea != nullptr);
	p0ContentArea->set_border_width(0);
//std::cout << "WeightDialog::WeightDialog() content area children: " << p0ContentArea->get_children().size() << '\n';
	m_p0Canvas = Gtk::manage(new Gtk::DrawingArea());
	p0ContentArea->pack_start(*m_p0Canvas, true, true);

	m_p0Canvas->signal_draw().connect(sigc::mem_fun(*this, &KeyboardDialog::onCustomDraw));

	signal_button_press_event().connect(sigc::mem_fun(*this, &KeyboardDialog::onMouseButtonPressed));
	signal_button_release_event().connect(sigc::mem_fun(*this, &KeyboardDialog::onMouseButtonReleased));
	signal_touch_event().connect(sigc::mem_fun(*this, &KeyboardDialog::onTouchEvent));
	signal_key_press_event().connect(sigc::mem_fun(*this, &KeyboardDialog::onKeyPressEvent));
	signal_key_release_event().connect(sigc::mem_fun(*this, &KeyboardDialog::onKeyReleaseEvent));

	m_oIsActiveConn = property_is_active().signal_changed().connect(sigc::mem_fun(this, &KeyboardDialog::onSigIsActiveChanged));

	show_all();

	m_refPaCtx = Glib::wrap(gdk_pango_context_get());
	m_refFontLayout = Pango::Layout::create(m_refPaCtx);
}
KeyboardDialog::~KeyboardDialog()
{
}
bool KeyboardDialog::isWindowActive() const
{
	return get_realized() && get_visible() && is_active();
}
void KeyboardDialog::onSigIsActiveChanged()
{
	if (!isWindowActive()) {
		m_oBttestWindow.finalizeKeys();
		redrawCanvas();
	}
}
int KeyboardDialog::run(bool bFullscreen)
{
	m_sNullKey = m_oBttestWindow.m_oInputStrings.getKeyString(hk::HK_NULL);

	Gdk::EventMask oAddEvMask = static_cast<Gdk::EventMask>(0);
	oAddEvMask |= Gdk::KEY_PRESS_MASK;
	oAddEvMask |= Gdk::KEY_RELEASE_MASK;
	//oAddEvMask |= Gdk::POINTER_MOTION_MASK;
	oAddEvMask |= Gdk::BUTTON_PRESS_MASK;
	oAddEvMask |= Gdk::BUTTON_RELEASE_MASK;
	//oAddEvMask |= Gdk::SCROLL_MASK;
	oAddEvMask |= Gdk::TOUCH_MASK;
	if (get_realized()) {
		add_events(oAddEvMask);
	} else {
		const Gdk::EventMask oCurMask = get_events();
		const Gdk::EventMask oNewMask = oCurMask | oAddEvMask;
		if (oNewMask != oCurMask) {
			oAddEvMask = oNewMask;
			set_events(oNewMask);
		}
	}
	int32_t nDiaWidth;
	int32_t nDiaHeight;
	//
	if (bFullscreen) {
		get_size(nDiaWidth, nDiaHeight);
		fullscreen();
	} else {
		unfullscreen();
	}
	auto nRes = Gtk::Dialog::run();
	if (bFullscreen) {
		unfullscreen();
		resize(nDiaWidth, nDiaHeight);
	}
	return nRes;
}
void KeyboardDialog::connectionStateChanged()
{
	redrawCanvas();
}
bool KeyboardDialog::onCustomDraw(const Cairo::RefPtr<Cairo::Context>& refCc)
{
//std::cout << "GameGtkDrawingArea::onCustomDraw() 1 " << '\n';

	// This is where we draw on the window
	Glib::RefPtr<Gdk::Window> refWindow = m_p0Canvas->get_window();
	if (!refWindow) {
		return true; //---------------------------------------------------------
	}

	Gtk::Allocation oAllocation = m_p0Canvas->get_allocation();
	const int32_t nWidth = std::max(oAllocation.get_width(), 50);
	const int32_t nHeight = std::max(oAllocation.get_height(), 50);
	m_nCanvasX = oAllocation.get_x();
	m_nCanvasY = oAllocation.get_y();

	calcCellPositions(nWidth + s_nMargin, nHeight + s_nMargin);

	int32_t nCommonTextW = std::numeric_limits<int32_t>::max();
	int32_t nCommonTextH = std::numeric_limits<int32_t>::max();
	for (int32_t nColumn = 0; nColumn < m_oBttestWindow.m_nTotColumns; ++nColumn) {
		for (int32_t nRow = 0; nRow < m_oBttestWindow.m_nTotRows; ++nRow) {
			const int32_t nX = m_aColumnPosX[nColumn];
			const int32_t nY = m_aRowPosY[nRow];
			const int32_t nX2 = m_aColumnPosX[nColumn + 1] - s_nMargin;
			const int32_t nY2 = m_aRowPosY[nRow + 1] - s_nMargin;
			const int32_t nW = nX2 - nX;
			const int32_t nH = nY2 - nY;
			if (nW < nCommonTextW) {
				nCommonTextW = nW;
			}
			if (nH < nCommonTextH) {
				nCommonTextH = nH;
			}
		}
	}
	if (nCommonTextW < s_nTextMinPixW) {
		nCommonTextW = s_nTextMinPixW;
	}
	if (nCommonTextH < s_nTextMinPixH) {
		nCommonTextH = s_nTextMinPixH;
	}
	// take the ratio of one of the longer key names
	int32_t nDummy1;
	int32_t nDummy2;
	const double fMaxRatio = calcRatio(nCommonTextW, nCommonTextH, "LEFTCTRL", nDummy1, nDummy2);
	const double fMinRatio = calcRatio(s_nTextMinPixW, s_nTextMinPixH, "LEFTCTRL", nDummy1, nDummy2);
	//const double fRatio = calcBestTextRatio(); TODO
	refCc->save();

	for (int32_t nColumn = 0; nColumn < m_oBttestWindow.m_nTotColumns; ++nColumn) {
		for (int32_t nRow = 0; nRow < m_oBttestWindow.m_nTotRows; ++nRow) {
			const int32_t nX = m_aColumnPosX[nColumn];
			const int32_t nY = m_aRowPosY[nRow];
			const int32_t nX2 = m_aColumnPosX[nColumn + 1] - s_nMargin;
			const int32_t nY2 = m_aRowPosY[nRow + 1] - s_nMargin;
			const bool bPressed = m_oBttestWindow.isCellKeyPressed(nColumn, nRow);
			const double fR = (bPressed ? s_nColorKeyPressedR : s_nColorKeyNormalR);
			const double fG = (bPressed ? s_nColorKeyPressedG : s_nColorKeyNormalG);
			const double fB = (bPressed ? s_nColorKeyPressedB : s_nColorKeyNormalB);
			refCc->set_source_rgb(fR, fG, fB);
			const int32_t nW = nX2 - nX;
			const int32_t nH = nY2 - nY;
			refCc->rectangle(nX, nY, nW, nH);
			refCc->fill();

			refCc->translate(nX, nY);
			drawTextInRect(refCc, nW, nH, fMinRatio, fMaxRatio, bPressed, nColumn, nRow);
			refCc->translate(- nX, - nY);
		}
	}

	// draw the exit button
	if (m_oBttestWindow.m_bKeysExitInCell) {
		const int32_t nColumn = m_oBttestWindow.m_nKeysExitButtonInCellColumn;
		const int32_t nRow = m_oBttestWindow.m_nKeysExitButtonInCellRow;
		const int32_t nX = m_aColumnPosX[nColumn];
		const int32_t nY = m_aRowPosY[nRow];
		const int32_t nX2 = m_aColumnPosX[nColumn + 1] - s_nMargin;
		const int32_t nY2 = m_aRowPosY[nRow + 1] - s_nMargin;
		const int32_t nW = nX2 - nX;
		const int32_t nH = nY2 - nY;
		m_nExitButtonCenterX = nX + nW * m_oBttestWindow.m_nKeysExitButtonPercX / 100;
		m_nExitButtonCenterY = nY + nH * m_oBttestWindow.m_nKeysExitButtonPercY / 100;
	} else {
		m_nExitButtonCenterX = nWidth * m_oBttestWindow.m_nKeysExitButtonPercX / 100;
		m_nExitButtonCenterY = nHeight * m_oBttestWindow.m_nKeysExitButtonPercY / 100;
	}
	const auto eState = m_oBttestWindow.m_oBtKeyClient.getState();
	const bool bConnected = (eState == BtKeyClient::STATE_CONNECTED);
	const double fR = (bConnected ? s_nColorExitButtonConnectedR : s_nColorExitButtonDisconnectedR);
	const double fG = (bConnected ? s_nColorExitButtonConnectedG : s_nColorExitButtonDisconnectedG);
	const double fB = (bConnected ? s_nColorExitButtonConnectedB : s_nColorExitButtonDisconnectedB);
	refCc->set_source_rgb(fR, fG, fB);
	refCc->rectangle(m_nExitButtonCenterX - s_nExitButtonPixW / 2, m_nExitButtonCenterY - s_nExitButtonPixH / 2
					, s_nExitButtonPixW, s_nExitButtonPixH);
	refCc->fill();

	refCc->restore();

	return true;
}
double KeyboardDialog::calcRatio(int32_t nPixW, int32_t nPixH, const std::string& sText
								, int32_t& nTextW, int32_t& nTextH)
{
	textToLayout(s_sSansFontDesc, -1, sText, nTextW, nTextH);

	const double fRatioX = (1.0 * nPixW) / nTextW;
	const double fRatioY = (1.0 * nPixH) / nTextH;
	const double fRatioM = std::min<double>(fRatioX, fRatioY);
	const double fRatio = fRatioM * 0.9;
	return fRatio;
}
void KeyboardDialog::drawTextInRect(const Cairo::RefPtr<Cairo::Context>& refCc, int32_t nPixW, int32_t nPixH
									, double fMinRatio, double fMaxRatio, bool bPressed, int32_t nColumn, int32_t nRow)
{
	refCc->save();

	double fTransX = s_fAlign * nPixW;
	double fTransY = 0.5 * nPixH;
	refCc->translate(fTransX, fTransY);
	const int32_t nIdx = nColumn + m_oBttestWindow.m_nTotColumns * nRow;
	int32_t nTextW;
	int32_t nTextH;
	const auto& sKeyName = m_oBttestWindow.m_aEditNames[nIdx];
	const double fTempRatio = calcRatio(nPixW, nPixH
										, ((sKeyName == m_sNullKey) ? "" : sKeyName)
										, nTextW, nTextH);
	const double fRatio = std::max<double>(std::min<double>(fMaxRatio, fTempRatio), fMinRatio);

	refCc->scale(fRatio, fRatio);

	fTransY = - 0.5 * nTextH;
	fTransX = - s_fAlign * nTextW;
	refCc->translate(fTransX, fTransY);

	const double fR = (bPressed ? s_nColorTextPressedR : s_nColorTextNormalR);
	const double fG = (bPressed ? s_nColorTextPressedG : s_nColorTextNormalG);
	const double fB = (bPressed ? s_nColorTextPressedB : s_nColorTextNormalB);
	refCc->set_source_rgba(fR, fG, fB, 1.0);

	m_refFontLayout->show_in_cairo_context(refCc);

	refCc->restore();
}

void KeyboardDialog::textToLayout(const std::string& sFont, int32_t nFontSize
								, bool bSetWeight, Pango::Weight eWight, bool bSetStyle, Pango::Style eStyle
								, const std::string& sText
								, int32_t& nTextW, int32_t& nTextH)
{
	Pango::FontDescription oFont(sFont);
	if (bSetWeight) {
		oFont.set_weight(eWight);
	}
	if (bSetStyle) {
		oFont.set_style(eStyle);
	}
	if (nFontSize >= 0) {
		oFont.set_size(nFontSize * PANGO_SCALE);
	}

	m_refFontLayout->set_text(sText);
	m_refFontLayout->set_font_description(oFont);

	//get the text dimensions
	m_refFontLayout->get_pixel_size(nTextW, nTextH);
}
void KeyboardDialog::textToLayout(const std::string& sFont, int32_t nFontSize
								, const std::string& sText
								, int32_t& nTextW, int32_t& nTextH)
{
	textToLayout(sFont, nFontSize, false, Pango::WEIGHT_NORMAL, false, Pango::STYLE_NORMAL
					, sText, nTextW, nTextH);
}

void KeyboardDialog::calcCellPositions(int32_t nPixW, int32_t nPixH)
{
	m_aColumnPosX.resize(m_oBttestWindow.m_nTotColumns + 1);
	m_aRowPosY.resize(m_oBttestWindow.m_nTotRows + 1);

	int32_t nTotColumnsWeight = 0;
	for (const auto& nWeight : m_oBttestWindow.m_aEditColumnsWeight) {
		nTotColumnsWeight += nWeight;
	}
	int32_t nTotRowsWeight = 0;
	for (const auto& nWeight : m_oBttestWindow.m_aEditRowsWeight) {
		nTotRowsWeight += nWeight;
	}
	m_aColumnPosX[0] = 0;
	for (int32_t nColumn = 1; nColumn <= m_oBttestWindow.m_nTotColumns; ++nColumn) {
		const double fFactor = 1.0 * m_oBttestWindow.m_aEditColumnsWeight[nColumn - 1] / nTotColumnsWeight;
		const int32_t nCellW = static_cast<int32_t>(fFactor * nPixW);
		m_aColumnPosX[nColumn] = m_aColumnPosX[nColumn - 1] + nCellW;
	}
	m_aRowPosY[0] = 0;
	for (int32_t nRow = 1; nRow <= m_oBttestWindow.m_nTotRows; ++nRow) {
		const double fFactor = 1.0 * m_oBttestWindow.m_aEditRowsWeight[nRow - 1] / nTotRowsWeight;
		const int32_t nCellH = static_cast<int32_t>(fFactor * nPixH);
		m_aRowPosY[nRow] = m_aRowPosY[nRow - 1] + nCellH;
	}
}
bool KeyboardDialog::onMouseButtonPressed(GdkEventButton* p0ButtonEvent)
{
	const GdkWindow* p0GdkWindow = p0ButtonEvent->window;
	if (p0GdkWindow != get_window()->gobj()) {
		return false; //--------------------------------------------------------
	}
	const double fX = p0ButtonEvent->x - m_nCanvasX;
	const double fY = p0ButtonEvent->y - m_nCanvasY;
//std::cout << "KeyboardDialog::onMouseButtonPressed()  m_nCanvasX=" << m_nCanvasX << " m_nCanvasY=" << m_nCanvasY << '\n';
//std::cout << "KeyboardDialog::onMouseButtonPressed()  fX=" << fX << " fY=" << fY << '\n';
//std::cout << "KeyboardDialog::onMouseButtonPressed()  m_nExitButtonCenterX=" << m_nExitButtonCenterX << " m_nExitButtonCenterY=" << m_nExitButtonCenterY << '\n';
	if ((fX >= m_nExitButtonCenterX - s_nExitButtonPixW / 2) && (fY >= m_nExitButtonCenterY - s_nExitButtonPixH / 2)
			&& (fX < m_nExitButtonCenterX + s_nExitButtonPixW / 2) && (fY < m_nExitButtonCenterY + s_nExitButtonPixH / 2)) {
		// Exit pressed
		hide();
		return true; //---------------------------------------------------------
	}
	const hk::HARDWARE_KEY eKey = getKeyFromCoords(fX, fY);
	if (eKey != hk::HK_NULL) {
		m_oBttestWindow.pressMouseKey(eKey);
		redrawCanvas();
	}
	return true;
}
bool KeyboardDialog::onMouseButtonReleased(GdkEventButton* p0ButtonEvent)
{
	const GdkWindow* p0GdkWindow = p0ButtonEvent->window;
	if (p0GdkWindow != get_window()->gobj()) {
		return false; //--------------------------------------------------------
	}
	m_oBttestWindow.releaseMouseKey();
	redrawCanvas();
	return true;
}
bool KeyboardDialog::onTouchEvent(GdkEventTouch* p0TouchEvent)
{
	assert(p0TouchEvent != nullptr);
	const GdkWindow* p0GdkWindow = p0TouchEvent->window;
	if (p0GdkWindow != get_window()->gobj()) {
		return false; //--------------------------------------------------------
	}
	const GdkEventSequence *p0Finger = p0TouchEvent->sequence;
	assert(p0Finger != nullptr);
	if (p0TouchEvent->type == GDK_TOUCH_BEGIN) {
		const double fX = p0TouchEvent->x - m_nCanvasX;
		const double fY = p0TouchEvent->y - m_nCanvasY;
		const hk::HARDWARE_KEY eKey = getKeyFromCoords(fX, fY);
		if (eKey != hk::HK_NULL) {
			m_oBttestWindow.pressFingerKey(eKey, p0Finger);
			redrawCanvas();
		}
	} else {
		const bool bRelease = (p0TouchEvent->type == GDK_TOUCH_END);
		const bool bCancel = (p0TouchEvent->type == GDK_TOUCH_CANCEL);
		if (bRelease || bCancel) {
			m_oBttestWindow.releaseFingerKey(p0Finger, bCancel);
			redrawCanvas();
		}
	}
	return true;
}
bool KeyboardDialog::onKeyPressEvent(GdkEventKey* p0KeyEvent)
{
	hk::HARDWARE_KEY eKey;
	if (!convertKeyCodeToHardwareKey(p0KeyEvent->hardware_keycode, eKey)) {
		return false;
	}
	if (eKey != hk::HK_NULL) {
		m_oBttestWindow.pressKeyboardKey(eKey);
		redrawCanvas();
	}
	return true;
}
bool KeyboardDialog::onKeyReleaseEvent(GdkEventKey* p0KeyEvent)
{
	hk::HARDWARE_KEY eKey;
	if (!convertKeyCodeToHardwareKey(p0KeyEvent->hardware_keycode, eKey)) {
		return false;
	}
	if (eKey != hk::HK_NULL) {
		m_oBttestWindow.releaseKeyboardKey(eKey);
		redrawCanvas();
	}
	return true;
}
void KeyboardDialog::redrawCanvas()
{
	gdk_window_invalidate_rect(get_window()->gobj(), nullptr, true);
}
bool KeyboardDialog::convertKeyCodeToHardwareKey(guint16 nGdkKeycode, hk::HARDWARE_KEY& eHardwareKey) const
{
	/* Evdev formula from Daniel Berrange's https://www.berrange.com/tags/key-codes/ */
	const int32_t nLinuxCode = nGdkKeycode - 8;
	if (nLinuxCode < 0) {
		return false;
	}
	eHardwareKey = static_cast<hk::HARDWARE_KEY>(nLinuxCode);
	return true;

}
hk::HARDWARE_KEY KeyboardDialog::getKeyFromCoords(double fX, double fY)
{
	const auto nTotColumns = m_oBttestWindow.m_nTotColumns;
	const auto nTotRows = m_oBttestWindow.m_nTotRows;
	for (int32_t nCol = 0; nCol < nTotColumns; ++nCol) {
		for (int32_t nRow = 0; nRow < nTotRows; ++nRow) {
			const int32_t nX = m_aColumnPosX[nCol];
			const int32_t nY = m_aRowPosY[nRow];
			const int32_t nX2 = m_aColumnPosX[nCol + 1] - s_nMargin;
			const int32_t nY2 = m_aRowPosY[nRow + 1] - s_nMargin;
			const int32_t nIdx = nCol + nTotColumns * nRow;
			const auto nKey = m_oBttestWindow.m_aEditHK[nIdx];
			if ((fX >= nX) && (fX < nX2) && (fY >= nY) && (fY < nY2)) {
				return nKey; //-------------------------------------------------
			}
		}
	}
	return hk::HK_NULL;
}

} // namespace stmi

