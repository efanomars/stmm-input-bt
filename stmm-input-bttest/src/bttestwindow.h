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
 * File:   bttestwindow.h
 */

#ifndef STMI_BTTEST_WINDOW_H
#define STMI_BTTEST_WINDOW_H

#include "weightdialog.h"
#include "keydialog.h"
#include "keyboarddialog.h"

#include "btkeyclient.h"
#include "btkeyservers.h"
#include "inputstrings.h"

#include <gtkmm.h>

#include <string>

namespace stmi
{

class BttestWindow : public Gtk::Window
{
public:
	BttestWindow(const std::string& sTitle, BtKeyClient& oBtKeyClient, BtKeyServers& oBtKeyServers);
	virtual ~BttestWindow();

	void saveStateToConfig();
private:
	void onNotebookSwitchPage(Gtk::Widget*, guint nPageNum);
	void onServerSelectionChanged();
	void onChooseServer();
	void onRefreshServers();
	void onButtonConnect();
	void onButtonDisconnect();
	void onButtonRemove();

	void onClientStateChanged();
	void onClientError();

	void onServersChanged();
	void onServersProgress(int32_t nSecs);

	void onButtonKeyboard();

	void setSensitivityForState();

	void recreateServersList();
	void recreateWeightsList();
	void recreateCellsKeyList();
	void updateInCellColumn();
	void updateInCellRow();

	// is the key pressed (regardless of the type of input)
	bool isKeyPressed(hk::HARDWARE_KEY eKey) const;
	// is the key pressed (regardless of the type of input)
	bool isCellKeyPressed(int32_t nColumn, int32_t nRow) const;
	bool pressKeyboardKey(hk::HARDWARE_KEY eKey);
	bool pressMouseKey(hk::HARDWARE_KEY eKey);
	bool pressFingerKey(hk::HARDWARE_KEY eKey, const GdkEventSequence* p0Finger);
	bool releaseKeyboardKey(hk::HARDWARE_KEY eKey);
	bool releaseMouseKey();
	bool releaseFingerKey(const GdkEventSequence* p0Finger, bool bCancel);
	enum PRESS_TYPE {
		PRESS_TYPE_KEYBOARD = 0
		, PRESS_TYPE_MOUSE = 1
		, PRESS_TYPE_TOUCH = 2
	};
	bool pressKey(hk::HARDWARE_KEY eKey, PRESS_TYPE eType, const GdkEventSequence* p0Finger);
	bool releaseKey(hk::HARDWARE_KEY eKey, PRESS_TYPE eType, const GdkEventSequence* p0Finger, bool bCancel);

	void printStringToLog(const std::string& sStr);

	void finalizeKeys();

	void onSpinNrColumnsChanged();
	void onSpinNrRowsChanged();
	void onColumnWeightActivated(const Gtk::TreeModel::Path& oPath, Gtk::TreeViewColumn* p0Column);
	void onRowWeightActivated(const Gtk::TreeModel::Path& oPath, Gtk::TreeViewColumn* p0Column);
	void onCellNameActivated(const Gtk::TreeModel::Path& oPath, Gtk::TreeViewColumn* p0Column);
	void onKeysFullscreenChanged();
	void onSpinExitPositionXChanged();
	void onSpinExitPositionYChanged();
	void onKeysExitInCellChanged();
	void onSpinExitInCellColumnChanged();
	void onSpinExitInCellRowChanged();
	void onKeysLogSentKeysChanged();

	void loadStateFromConfig();

private:
	BtKeyClient& m_oBtKeyClient;
	BtKeyServers& m_oBtKeyServers;

	static constexpr int32_t s_nEditInitialGridW = 3;
	static constexpr int32_t s_nEditInitialGridH = 3;
	static constexpr int32_t s_nEditMaxGridW = 5;
	static constexpr int32_t s_nEditMaxGridH = 5;

	static const int32_t s_nEditDefaultWeight;
	static constexpr int32_t s_nEditMaxWeight = 1000;

	static constexpr int32_t s_nTotPages = 5;
	int32_t m_aPageIndex[s_nTotPages];

	Glib::RefPtr<Gtk::Adjustment> m_refAdjustmentNrColumns;
	Glib::RefPtr<Gtk::Adjustment> m_refAdjustmentNrRows;
	Glib::RefPtr<Gtk::Adjustment> m_refAdjustmentExitPositionX;
	Glib::RefPtr<Gtk::Adjustment> m_refAdjustmentExitPositionY;
	Glib::RefPtr<Gtk::Adjustment> m_refAdjustmentExitInCellC;
	Glib::RefPtr<Gtk::Adjustment> m_refAdjustmentExitInCellR;

	//Gtk::Box* m_p0VBoxMain = nullptr;

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


	////////////
	class ServersColumns : public Gtk::TreeModel::ColumnRecord
	{
	public:
		ServersColumns()
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
		WeightColumns()
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
		CellKeyColumns()
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

	Glib::RefPtr<WeightDialog> m_refWeightDialog;
	Glib::RefPtr<KeyDialog> m_refKeyDialog;
	Glib::RefPtr<KeyboardDialog> m_refKeyboardDialog;

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
	static constexpr int32_t s_nTextBufferLogMaxLines = 2000;

	Glib::RefPtr<Gtk::TextBuffer> m_refTextBufferInfo;

	std::string m_sSelectedAddr;
	int32_t m_nSelectedPort = -1;

	BtKeyServers::ServerInfo m_oCurServer;

	InputStrings m_oInputStrings;

	static constexpr int32_t s_nInitialWindowSizeW = 550;
	static constexpr int32_t s_nInitialWindowSizeH = 400;

	static const std::string s_sConfAppDir;
	static const std::string s_sConfKeyTotColumns;
	static const std::string s_sConfKeyTotRows;
	static const std::string s_sConfKeyColumnWeights;
	static const std::string s_sConfKeyRowWeights;
	static const std::string s_sConfKeyCellKeys;
	static const std::string s_sConfKeyFullscreen;
	static const std::string s_sConfKeyExitButtonPercX;
	static const std::string s_sConfKeyExitButtonPercY;
	static const std::string s_sConfKeyExitInCell;
	static const std::string s_sConfKeyExitButtonInCellColumn;
	static const std::string s_sConfKeyExitButtonInCellRow;
	static const std::string s_sConfKeyLogSentKeys;
private:
	BttestWindow() = delete;
};

} // namespace stmi

#endif /* STMI_BTTEST_WINDOW_H */

