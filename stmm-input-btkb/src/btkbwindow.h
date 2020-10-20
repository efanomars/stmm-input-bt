/*
 * Copyright © 2017-2020  Stefano Marsili, <stemars@gmx.ch>
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
 * File:   btkbwindow.h
 */

#ifndef STMI_BTKB_WINDOW_H
#define STMI_BTKB_WINDOW_H

#include "addrscreen.h"
#include "weightscreen.h"
#include "keyscreen.h"
#include "keyboardscreen.h"
#include "hardwarekey.h"

#include "btkeyclient.h"
#include "btkeyservers.h"
#include "inputstrings.h"

#include <gtkmm.h>

#include <string>
#include <vector>
#include <memory>

#include <stdint.h>

namespace stmi { class BtKeyClient; }

namespace stmi
{

using std::unique_ptr;

class BtkbWindow : public Gtk::Window
{
public:
	BtkbWindow(const std::string& sTitle, BtKeyClient& oBtKeyClient, BtKeyServers& oBtKeyServers) noexcept;

	void saveStateToConfig() noexcept;
private:
	void addWindowEventTypes(Gdk::EventMask oBitsToAdd) noexcept;

	void changeScreen(int32_t nToScreen, const std::string& sMsg) noexcept;
	void setAddr(const std::string& sAddr) noexcept;
	void setWeight(int32_t nWeight, bool bColumn, int32_t nColRow, const Gtk::TreeModel::Path& oPath) noexcept;
	void setKey(hk::HARDWARE_KEY eKey, int32_t nColumn, int32_t nRow, const Gtk::TreeModel::Path& oPath) noexcept;

	bool onWindowStateEventChanged(GdkEventWindowState* p0WindowStateEvent) noexcept;
	bool isFullscreen() const noexcept;
	bool on_key_press_event(GdkEventKey* p0Event)override;

	void onNotebookSwitchPage(Gtk::Widget*, guint nPageNum) noexcept;
	void onServerSelectionChanged() noexcept;
	void onChooseServer() noexcept;
	void onRefreshServers() noexcept;
	void onInputStagedServer() noexcept;
	void onButtonConnect() noexcept;
	void onButtonDisconnect() noexcept;
	void onButtonRemove() noexcept;

	void onClientStateChanged() noexcept;
	void onClientError() noexcept;

	void onServersChanged() noexcept;
	void onServersProgress(int32_t nSecs) noexcept;

	void onButtonKeyboard() noexcept;

	void setSensitivityForState() noexcept;

	void recreateServersList() noexcept;
	void recreateWeightsList() noexcept;
	void recreateCellsKeyList() noexcept;
	void updateInCellColumn() noexcept;
	void updateInCellRow() noexcept;

	void setStagedServerAndPort() noexcept;

	// is the key pressed (regardless of the type of input)
	bool isKeyPressed(hk::HARDWARE_KEY eKey) const noexcept;
	// is the key pressed (regardless of the type of input)
	bool isCellKeyPressed(int32_t nColumn, int32_t nRow) const noexcept;
	bool pressKeyboardKey(hk::HARDWARE_KEY eKey) noexcept;
	bool pressMouseKey(hk::HARDWARE_KEY eKey) noexcept;
	bool pressFingerKey(hk::HARDWARE_KEY eKey, const GdkEventSequence* p0Finger) noexcept;
	bool releaseKeyboardKey(hk::HARDWARE_KEY eKey) noexcept;
	bool releaseMouseKey() noexcept;
	bool releaseFingerKey(const GdkEventSequence* p0Finger, bool bCancel) noexcept;
	enum PRESS_TYPE {
		PRESS_TYPE_KEYBOARD = 0
		, PRESS_TYPE_MOUSE = 1
		, PRESS_TYPE_TOUCH = 2
	};
	bool pressKey(hk::HARDWARE_KEY eKey, PRESS_TYPE eType, const GdkEventSequence* p0Finger) noexcept;
	bool releaseKey(hk::HARDWARE_KEY eKey, PRESS_TYPE eType, const GdkEventSequence* p0Finger, bool bCancel) noexcept;

	void printStringToLog(const std::string& sStr) noexcept;

	void finalizeKeys() noexcept;

	void onSpinNrColumnsChanged() noexcept;
	void onSpinNrRowsChanged() noexcept;
	void onColumnWeightActivated(const Gtk::TreeModel::Path& oPath, Gtk::TreeViewColumn* p0Column) noexcept;
	void onRowWeightActivated(const Gtk::TreeModel::Path& oPath, Gtk::TreeViewColumn* p0Column) noexcept;
	void onCellNameActivated(const Gtk::TreeModel::Path& oPath, Gtk::TreeViewColumn* p0Column) noexcept;
	void onKeysFullscreenChanged() noexcept;
	void onSpinExitPositionXChanged() noexcept;
	void onSpinExitPositionYChanged() noexcept;
	void onKeysExitInCellChanged() noexcept;
	void onSpinExitInCellColumnChanged() noexcept;
	void onSpinExitInCellRowChanged() noexcept;
	void onKeysLogSentKeysChanged() noexcept;

	void loadStateFromConfig() noexcept;

private:
	BtKeyClient& m_oBtKeyClient;
	BtKeyServers& m_oBtKeyServers;
	bool m_bIsFullscreen;

	static constexpr const int32_t s_nEditInitialGridW = 3;
	static constexpr const int32_t s_nEditInitialGridH = 3;
	static constexpr const int32_t s_nEditMaxGridW = 5;
	static constexpr const int32_t s_nEditMaxGridH = 5;

	static const int32_t s_nEditDefaultWeight;
	static const int32_t s_nEditMinWeight;
	static const int32_t s_nEditMaxWeight;

	static constexpr const int32_t s_nTotPages = 5;
	int32_t m_aPageIndex[s_nTotPages];

	Glib::RefPtr<Gtk::Adjustment> m_refAdjustmentNrColumns;
	Glib::RefPtr<Gtk::Adjustment> m_refAdjustmentNrRows;
	Glib::RefPtr<Gtk::Adjustment> m_refAdjustmentExitPositionX;
	Glib::RefPtr<Gtk::Adjustment> m_refAdjustmentExitPositionY;
	Glib::RefPtr<Gtk::Adjustment> m_refAdjustmentExitInCellC;
	Glib::RefPtr<Gtk::Adjustment> m_refAdjustmentExitInCellR;

	//Gtk::Box* m_p0VBoxMain = nullptr;
	int32_t m_nCurrentScreen;
	static constexpr const int32_t s_nScreenMain = 0;
	static constexpr const int32_t s_nScreenEnterAddr = 1;
	static constexpr const int32_t s_nScreenEnterWeight = 2;
	static constexpr const int32_t s_nScreenEnterKey = 3;
	static constexpr const int32_t s_nScreenKeyboard = 4;
	static constexpr const int32_t s_nTotScreens = 5;

	Gtk::Stack* m_p0StackScreens = nullptr;

	Gtk::Notebook* m_p0NotebookChoices = nullptr;

		static const int32_t s_nTabServers = 0;
		//Gtk::Label* m_p0TabLabelServers = nullptr;
		//Gtk::Box* m_p0VBoxServers = nullptr;
			Gtk::TreeView* m_p0TreeViewServers = nullptr;
			//Gtk::Box* m_p0HBoxServersCmds; = nullptr
				Gtk::Button* m_p0ButtonChooseServer = nullptr;
				Gtk::Button* m_p0ButtonRefreshServers = nullptr;
				Gtk::Label* m_p0LabelProgress = nullptr;
			//Gtk::Box* m_p0VBoxConnection = nullptr;
				//Gtk::Box* m_p0HBoxStagedServer = nullptr;
					Gtk::Button* m_p0ButtonInputServer = nullptr;
					Gtk::Label* m_p0LabelServer = nullptr;
				//Gtk::Box* m_p0HBoxConnectionCmds = nullptr;
					Gtk::Button* m_p0ButtonConnect = nullptr;
					Gtk::Button* m_p0ButtonDisconnect = nullptr;
					Gtk::Button* m_p0ButtonRemove = nullptr;

		static const int32_t s_nTabEdit = 1;
		//Gtk::Label* m_p0TabLabelEdit = nullptr;
		//Gtk::Box* m_p0VBoxEdit = nullptr;
			//Gtk::Box* m_p0HBoxSize = nullptr;
				//Gtk::Box* m_p0VBoxColumns = nullptr;
					//Gtk::Label* m_p0LabelColumns = nullptr;
					Gtk::SpinButton* m_p0SpinColumns = nullptr;
					Gtk::TreeView* m_p0TreeViewColumnWeights = nullptr;
				//Gtk::Box* m_p0VBoxRows = nullptr;
					//Gtk::Label* m_p0LabelRows = nullptr;
					Gtk::SpinButton* m_p0SpinRows = nullptr;
					Gtk::TreeView* m_p0TreeViewRowWeights = nullptr;
			//Gtk::ScrolledWindow* m_p0ScrolledCellKey = nullptr;
				Gtk::TreeView* m_p0TreeViewCellKey = nullptr;

		static const int32_t s_nTabKeys = 2;
		//Gtk::Label* m_p0TabLabelKeys = nullptr;
		//Gtk::Box* m_p0VBoxKeys = nullptr;
			Gtk::CheckButton* m_p0CheckButtonFullscreen = nullptr;
			//Gtk::Box* m_p0VBoxExitPosition = nullptr;
				//Gtk::Label* m_p0LabelExitPosition = nullptr;
				//Gtk::Box* m_p0HBoxExitPositionXY = nullptr;
					//Gtk::Label* m_p0LabelExitPositionX = nullptr;
					Gtk::SpinButton* m_p0SpinExitPositionX = nullptr;
					//Gtk::Label* m_p0LabelExitPositionY = nullptr;
					Gtk::SpinButton* m_p0SpinExitPositionY = nullptr;
			Gtk::CheckButton* m_p0CheckButtonInCell = nullptr;
			//Gtk::Box* m_p0HBoxInCellCR = nullptr;
				//Gtk::Label* m_p0LabelInCellC = nullptr;
				Gtk::SpinButton* m_p0SpinExitInCellC = nullptr;
				//Gtk::Label* m_p0LabelInCellR = nullptr;
				Gtk::SpinButton* m_p0SpinExitInCellR = nullptr;
			Gtk::CheckButton* m_p0CheckButtonLogSentKeys = nullptr;
			//Gtk::Button* m_p0ButtonKeyboard = nullptr;

		static const int32_t s_nTabLog = 3;
		//Gtk::Label* m_p0TabLabelLog = nullptr;
		//Gtk::ScrolledWindow* m_p0ScrolledLog = nullptr;
			Gtk::TextView* m_p0TextViewLog = nullptr;

		static const int32_t s_nTabInfo = 4;
		//Gtk::Label* m_p0TabLabelInfo = nullptr;
		//Gtk::ScrolledWindow* m_p0ScrolledInfo = nullptr;
			//Gtk::Label* m_p0LabelInfoText = nullptr;

	Gtk::Widget* m_p0ScreenBoxEnterAddr = nullptr;
		//Gtk::Box* m_p0BoxEnterAddr = nullptr;

	Gtk::Widget* m_p0ScreenBoxEnterWeight = nullptr;
		//Gtk::Box* m_p0BoxEnterWeight = nullptr;

	Gtk::Widget* m_p0ScreenBoxEnterKey = nullptr;
		//Gtk::Box* m_p0BoxEnterKey = nullptr;

	Gtk::Widget* m_p0ScreenBoxKeyboard = nullptr;
		//Gtk::Box* m_p0BoxKeyboard = nullptr;

	Gtk::Widget* m_aScreens[s_nTotScreens]; // non owning pointers

	////////////
	class ServersColumns : public Gtk::TreeModel::ColumnRecord
	{
	public:
		ServersColumns() noexcept
		{
			add(m_oColAddr);
			add(m_oColPort);
			add(m_oColName);
		}
		Gtk::TreeModelColumn<std::string> m_oColAddr;
		Gtk::TreeModelColumn<int32_t> m_oColPort;
		Gtk::TreeModelColumn<Glib::ustring> m_oColName;
	};
	ServersColumns m_oServersColumns;
	Glib::RefPtr<Gtk::TreeStore> m_refTreeModelServers;

	////////////
	class WeightColumns : public Gtk::TreeModel::ColumnRecord
	{
	public:
		WeightColumns() noexcept
		{
			add(m_oIdx); add(m_oWeight);
		}
		Gtk::TreeModelColumn<int32_t> m_oIdx;
		Gtk::TreeModelColumn<int32_t> m_oWeight;
	};
	WeightColumns m_oWeightColumns;
	Glib::RefPtr<Gtk::TreeStore> m_refTreeModelColumnWeight;
	Glib::RefPtr<Gtk::TreeStore> m_refTreeModelRowWeight;

	////////////
	class CellKeyColumns : public Gtk::TreeModel::ColumnRecord
	{
	public:
		CellKeyColumns() noexcept
		{
			add(m_oColX); add(m_oColY); add(m_oColKeyName);
			add(m_oColHiddenHardwareKey);
		}
		Gtk::TreeModelColumn<int32_t> m_oColX;
		Gtk::TreeModelColumn<int32_t> m_oColY;
		Gtk::TreeModelColumn<Glib::ustring> m_oColKeyName;
		Gtk::TreeModelColumn<int32_t> m_oColHiddenHardwareKey;
	};
	CellKeyColumns m_oCellKeyColumns;
	Glib::RefPtr<Gtk::TreeStore> m_refTreeModelCellKey;

	friend class AddrScreen;
	unique_ptr<AddrScreen> m_refAddrScreen;
	friend class WeightScreen;
	unique_ptr<WeightScreen> m_refWeightScreen;
	friend class KeyScreen;
	unique_ptr<KeyScreen> m_refKeyScreen;
	friend class KeyboardScreen;
	unique_ptr<KeyboardScreen> m_refKeyboardScreen;

	////////////
	friend class KeyboardDialog;
	// Following: the data stored in the config
	int32_t m_nTotColumns = s_nEditInitialGridW;
	int32_t m_nTotRows = s_nEditInitialGridH;
	std::vector<int32_t> m_aEditColumnsWeight; // Size: m_nTotColumns
	std::vector<int32_t> m_aEditRowsWeight; // Size: m_nTotRows
	std::vector<hk::HARDWARE_KEY> m_aEditHK; // Size: m_nTotColumns * m_nTotRows, Index: nColumn + nRow * m_nTotColumns
	std::vector<std::string> m_aEditNames; // Size: m_nTotColumns * m_nTotRows, Index: nColumn + nRow * m_nTotColumns
	bool m_bKeysFullscreen = false;
	int32_t m_nKeysExitButtonPercX = 50;
	int32_t m_nKeysExitButtonPercY = 50;
	bool m_bKeysExitInCell = false;
	int32_t m_nKeysExitButtonInCellColumn = s_nEditInitialGridW / 2; // Value: 0 .. m_nTotColumns - 1
	int32_t m_nKeysExitButtonInCellRow = s_nEditInitialGridH / 2; // Value: 0 .. m_nTotRows - 1
	bool m_bKeysLogSentKeys = true;

	// Following: run time and static data
	struct PressedKey
	{
		hk::HARDWARE_KEY m_eKey = hk::HK_NULL;
		PRESS_TYPE m_eType = PRESS_TYPE_TOUCH;
		GdkEventSequence const* m_p0Finger = nullptr; // only non null if m_eType == PRESS_TYPE_TOUCH
	};
	std::vector<PressedKey> m_aPressedKeys; // Size: <= m_nTotColumns * m_nTotRows, Values: unique for tuple (key,type,finger)

	Glib::RefPtr<Gtk::TextBuffer> m_refTextBufferLog;
	Glib::RefPtr<Gtk::TextBuffer::Mark> m_refTextBufferMarkBottom;
	int32_t m_nTextBufferLogTotLines = 0;
	static constexpr const int32_t s_nTextBufferLogMaxLines = 2000;

	Glib::RefPtr<Gtk::TextBuffer> m_refTextBufferInfo;

	std::string m_sSelectedAddr;
	int32_t m_nSelectedPort = 0;

	BtKeyServers::ServerInfo m_oCurServer;

	InputStrings m_oInputStrings;

	static constexpr const int32_t s_nInitialWindowSizeW = 550;
	static constexpr const int32_t s_nInitialWindowSizeH = 400;
	static constexpr const int32_t s_nMinWindowSizeW = 100;
	static constexpr const int32_t s_nMinWindowSizeH = 100;

private:
	BtkbWindow() = delete;
};

} // namespace stmi

#endif /* STMI_BTKB_WINDOW_H */

