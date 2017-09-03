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

#include "btkeyclient.h"
#include "btkeyservers.h"

#include <gtkmm.h>

#include <string>

namespace stmi
{

class BttestWindow : public Gtk::Window
{
public:
	BttestWindow(const std::string& sTitle, BtKeyClient& oBtKeyClient, BtKeyServers& oBtKeyServers);
	virtual ~BttestWindow();
private:
	void onNotebookSwitchPage(Gtk::Widget*, guint nPageNum);
	void onServerSelectionChanged();
	void onChooseServer();
	void onRefreshServers();
	void onKeyToggled(int32_t nX, int32_t nY);
	void pressKey(int32_t nIdx);
	void releaseKey(int32_t nIdx);
	void releaseCancelKey(int32_t nIdx);
	bool onReleaseKey(GdkEventButton* p0ButtonEvent, int32_t nX, int32_t nY, bool bCancel);
	void onButtonConnect();
	void onButtonDisconnect();
	void onButtonRemove();
	void onWeirdClickToggled();

	void onClientStateChanged();
	void onClientError();

	void onServersChanged();
	void onServersProgress(int32_t nSecs);

	void setSensitivityForState();

	void recreateServersList();

	void printStringToLog(const std::string& sStr);

	bool finalizeKeys();

private:
	BtKeyClient& m_oBtKeyClient;
	BtKeyServers& m_oBtKeyServers;

	static constexpr int32_t s_nKeysGridW = 3;
	static constexpr int32_t s_nKeysGridH = 3;

	static constexpr int32_t s_nTotPages = 3;
	int32_t m_aPageIndex[s_nTotPages];

	//Gtk::Box* m_p0VBoxMain;

	Gtk::Notebook* m_p0NotebookChoices;

		static const int32_t s_nTabServers = 0;
		//Gtk::Label* m_p0TabLabelServers;
		//Gtk::Box* m_p0VBoxServers;
			Gtk::TreeView* m_p0TreeViewServers;
			//Gtk::Box* m_p0HBoxServersCmds;
				Gtk::Button* m_p0ButtonChooseServer;
				Gtk::Button* m_p0ButtonRefreshServers;
				Gtk::Label* m_p0LabelProgress;

		static const int32_t s_nTabKeys = 1;
		//Gtk::Label* m_p0TabLabelKeysOptions;
		//Gtk::Box* m_p0HBoxKeysOptions;
			Gtk::Grid* m_p0GridKeys; //TODO create arrays of boxes and toggles
				std::string m_aKeysNames[s_nKeysGridW * s_nKeysGridH];
				stmi::hk::HARDWARE_KEY m_aKeysHardware[s_nKeysGridW * s_nKeysGridH];
				Gtk::ToggleButton* m_aToggleButtonKeys[s_nKeysGridW * s_nKeysGridH];
			//Gtk::Box* m_p0VBoxLogClick;
				Gtk::ScrolledWindow* m_p0ScrolledLog;
					Gtk::TextView* m_p0TextViewLog;
				Gtk::CheckButton* m_p0CheckButtonWeirdClick;

		static const int32_t s_nTabInfo = 2;
		//Gtk::Label* m_p0TabLabelInfo;
		//Gtk::ScrolledWindow* m_p0ScrolledInfo;
			//Gtk::Label* m_p0LabelInfoText;
//		Gtk::TextView* m_p0TextViewInfos;

	//Gtk::Box* m_p0VBoxConnection;
		Gtk::Label* m_p0LabelServer;
		//Gtk::Box* m_p0HBoxConnectionCmds;
			Gtk::Button* m_p0ButtonConnect;
			Gtk::Button* m_p0ButtonDisconnect;
			Gtk::Button* m_p0ButtonRemove;

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

	Glib::RefPtr<Gtk::TextBuffer> m_refTextBufferLog;
	Glib::RefPtr<Gtk::TextBuffer::Mark> m_refTextBufferMarkBottom;
	int32_t m_nTextBufferLogTotLines;
	static constexpr int32_t s_nTextBufferLogMaxLines = 2000;

	Glib::RefPtr<Gtk::TextBuffer> m_refTextBufferInfo;

	std::string m_sSelectedAddr;
	int32_t m_nSelectedPort;

	// m_bKeysInButtonMode == false
	//   key toggle button is raised
	//      mouse click (press) sends a key press to the server
	//   key toggle button is "pressed in"
	//      mouse click (press!) sends a key release to the server
	// m_bKeysInButtonMode == true
	//   key toggle button is raised
	//      mouse press sends a key press to the server
	//   key toggle button is "pressed in"
	//      mouse release sends a key release to the server
	// When switching from (m_bKeysInButtonMode == false) to (m_bKeysInButtonMode == true)
	// for all the keys that are "pressed in": raise and send key release to the server
	bool m_bKeysInButtonMode;
	// When the toggle state is set to raised programmatically make sure that
	// the toggled callback does nothing
	bool m_bAvoidNestedToggled;

	BtKeyServers::ServerInfo m_oCurServer;

	static constexpr int32_t s_nInitialWindowSizeW = 550;
	static constexpr int32_t s_nInitialWindowSizeH = 400;
private:
	BttestWindow() = delete;
};

} // namespace stmi

#endif /* STMI_BTTEST_WINDOW_H */

