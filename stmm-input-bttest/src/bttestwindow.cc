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
 * File:   bttestwindow.cc
 */

#include "bttestwindow.h"

#include "config.h"

#include <cassert>
#include <iostream>

namespace stmi
{

BttestWindow::BttestWindow(const std::string& sTitle, BtKeyClient& oBtKeyClient, BtKeyServers& oBtKeyServers)
: m_oBtKeyClient(oBtKeyClient)
, m_oBtKeyServers(oBtKeyServers)
//, m_p0VBoxMain(nullptr)
, m_p0NotebookChoices(nullptr)
//, m_p0TabLabelServers(nullptr)
//, m_p0VBoxServers(nullptr)
, m_p0TreeViewServers(nullptr)
//, m_p0HBoxServersCmds(nullptr)
, m_p0ButtonChooseServer(nullptr)
, m_p0ButtonRefreshServers(nullptr)
, m_p0LabelProgress(nullptr)
//, m_p0TabLabelKeysOptions(nullptr)
//, m_p0HBoxKeysOptions(nullptr)
, m_p0GridKeys(nullptr)
//, m_p0VBoxLogClick(nullptr)
//, m_p0ScrolledLog(nullptr)
, m_p0TextViewLog(nullptr)
, m_p0CheckButtonWeirdClick(nullptr)
//, m_p0TabLabelInfo(nullptr)
//, m_p0ScrolledInfo(nullptr)
//, m_p0LabelInfoText(nullptr)
//, m_p0TextViewInfos(nullptr)
//, m_p0VBoxConnection(nullptr)
, m_p0LabelServer(nullptr)
//, m_p0HBoxConnectionCmds(nullptr)
, m_p0ButtonConnect(nullptr)
, m_p0ButtonDisconnect(nullptr)
, m_p0ButtonRemove(nullptr)
, m_nTextBufferLogTotLines(0)
, m_nSelectedPort(0)
, m_bKeysInButtonMode(false)
, m_bAvoidNestedToggled(false)
{
	std::string sInfoText = std::string("") + 
			"stmm-input-bttest\n"
			"=================\n"
			"\n"
			"Version: " + Config::getVersionString() + "\n"
			"\n"
			"Copyright © 2017   Stefano Marsili, <stemars@gmx.ch>\n"
			"\n"
			"This is a test client application for the btkeys bluetooth\n"
			"protocol to connect to the server defined in the\n"
			"libstmm-input-gtk-bt library.\n"
			"\n"
			"To detect the present devices the 'Refresh' button can be pressed.\n"
			"Per default detection takes up to 15 seconds.\n"
			"After choosing a server by clicking on it in the created list,\n"
			"press the 'Select' and then the 'Connect' button.\n"
			"If the connection succeeds you can press and release the\n"
			"virtual keys with your mouse.\n";
	//
	set_title(sTitle);
	set_default_size(s_nInitialWindowSizeW, s_nInitialWindowSizeH);
	set_resizable(true);

	Glib::RefPtr<Gtk::TreeSelection> refTreeSelection;
	Pango::FontDescription oMonoFont("Mono");

	Gtk::Box* m_p0VBoxMain = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_VERTICAL));
	Gtk::Window::add(*m_p0VBoxMain);

	m_p0NotebookChoices = Gtk::manage(new Gtk::Notebook());
	m_p0VBoxMain->pack_start(*m_p0NotebookChoices, true, true);
		m_p0NotebookChoices->signal_switch_page().connect(
						sigc::mem_fun(*this, &BttestWindow::onNotebookSwitchPage) );

	Gtk::Label* m_p0TabLabelServers = Gtk::manage(new Gtk::Label("\"Servers\""));
	Gtk::Box* m_p0VBoxServers = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_VERTICAL));
	m_aPageIndex[s_nTabServers] = m_p0NotebookChoices->append_page(*m_p0VBoxServers, *m_p0TabLabelServers);

		//Create the Servers tree model
		m_refTreeModelServers = Gtk::TreeStore::create(m_oServersColumns);
		m_p0TreeViewServers = Gtk::manage(new Gtk::TreeView(m_refTreeModelServers));
		m_p0VBoxServers->pack_start(*m_p0TreeViewServers, true, true);
			m_p0TreeViewServers->append_column("Addr", m_oServersColumns.m_oColAddr);
			m_p0TreeViewServers->append_column("Port", m_oServersColumns.m_oColPort);
			m_p0TreeViewServers->append_column("Name", m_oServersColumns.m_oColName);
			refTreeSelection = m_p0TreeViewServers->get_selection();
			refTreeSelection->signal_changed().connect(
							sigc::mem_fun(*this, &BttestWindow::onServerSelectionChanged));

		Gtk::Box* m_p0HBoxServersCmds = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_HORIZONTAL));
		m_p0VBoxServers->pack_start(*m_p0HBoxServersCmds, false, false);
			m_p0ButtonChooseServer = Gtk::manage(new Gtk::Button("Select"));
			m_p0HBoxServersCmds->pack_start(*m_p0ButtonChooseServer, true, true);
				m_p0ButtonChooseServer->signal_clicked().connect(
								sigc::mem_fun(*this, &BttestWindow::onChooseServer) );
			m_p0ButtonRefreshServers = Gtk::manage(new Gtk::Button("Refresh"));
			m_p0HBoxServersCmds->pack_start(*m_p0ButtonRefreshServers, true, true);
				m_p0ButtonRefreshServers->signal_clicked().connect(
								sigc::mem_fun(*this, &BttestWindow::onRefreshServers) );
			m_p0LabelProgress = Gtk::manage(new Gtk::Label("Progress"));
			m_p0HBoxServersCmds->pack_start(*m_p0LabelProgress);

	Gtk::Label* m_p0TabLabelKeysOptions = Gtk::manage(new Gtk::Label("Keys"));
	Gtk::Box* m_p0HBoxKeysOptions = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_HORIZONTAL));
	m_aPageIndex[s_nTabKeys] = m_p0NotebookChoices->append_page(*m_p0HBoxKeysOptions, *m_p0TabLabelKeysOptions);

		m_p0GridKeys = Gtk::manage(new Gtk::Grid());
		m_p0HBoxKeysOptions->pack_start(*m_p0GridKeys, true, true);
			m_p0GridKeys->set_row_homogeneous(true);
			m_p0GridKeys->set_column_homogeneous(true);
			assert(s_nKeysGridW * s_nKeysGridH == 9);
			m_aKeysNames[0] = "LCtrl";
			m_aKeysNames[1] = "Up";
			m_aKeysNames[2] = "LShift";
			m_aKeysNames[3] = "Left";
			m_aKeysNames[4] = "Space";
			m_aKeysNames[5] = "Right";
			m_aKeysNames[6] = "A";
			m_aKeysNames[7] = "Down";
			m_aKeysNames[8] = "B";
			m_aKeysHardware[0] = hk::HK_LEFTCTRL;
			m_aKeysHardware[1] = hk::HK_UP;
			m_aKeysHardware[2] = hk::HK_LEFTSHIFT;
			m_aKeysHardware[3] = hk::HK_LEFT;
			m_aKeysHardware[4] = hk::HK_SPACE;
			m_aKeysHardware[5] = hk::HK_RIGHT;
			m_aKeysHardware[6] = hk::HK_A;
			m_aKeysHardware[7] = hk::HK_DOWN;
			m_aKeysHardware[8] = hk::HK_B;
			for (int32_t nY = 0; nY < s_nKeysGridH; ++nY) {
				for (int32_t nX = 0; nX < s_nKeysGridW; ++nX) {
					Gtk::ToggleButton*& p0ToggleButton = m_aToggleButtonKeys[nX + nY * s_nKeysGridW];
					p0ToggleButton = Gtk::manage(new Gtk::ToggleButton(m_aKeysNames[nX + nY * s_nKeysGridW]));
					m_p0GridKeys->attach(*p0ToggleButton, nX, nY, 1, 1);
					p0ToggleButton->signal_toggled().connect(
									sigc::bind( sigc::mem_fun(*this, &BttestWindow::onKeyToggled), nX, nY) );
					//p0ToggleButton->signal_button_press_event().connect(
					//				sigc::bind( sigc::mem_fun(*this, &BttestWindow::onPressKey), nX, nY) );
					p0ToggleButton->signal_button_release_event().connect(
									sigc::bind( sigc::mem_fun(*this, &BttestWindow::onReleaseKey), nX, nY, false) );
				}
			}

		Gtk::Box* m_p0VBoxLogClick = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_VERTICAL));
		m_p0HBoxKeysOptions->pack_start(*m_p0VBoxLogClick, true, true);

			Gtk::ScrolledWindow* m_p0ScrolledLog = Gtk::manage(new Gtk::ScrolledWindow());
			m_p0VBoxLogClick->pack_start(*m_p0ScrolledLog, true, true);
				m_p0ScrolledLog->set_border_width(5);
				m_p0ScrolledLog->set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_ALWAYS);

				m_p0TextViewLog = Gtk::manage(new Gtk::TextView());
				m_p0ScrolledLog->add(*m_p0TextViewLog);
					m_refTextBufferLog = Gtk::TextBuffer::create();
					m_refTextBufferMarkBottom = Gtk::TextBuffer::Mark::create("Bottom");
					m_p0TextViewLog->set_editable(false);
					m_p0TextViewLog->set_buffer(m_refTextBufferLog);
					m_p0TextViewLog->override_font(oMonoFont);
					m_refTextBufferLog->add_mark(m_refTextBufferMarkBottom, m_refTextBufferLog->end());

			m_p0CheckButtonWeirdClick = Gtk::manage(new Gtk::CheckButton("Double click"));
			m_p0VBoxLogClick->pack_start(*m_p0CheckButtonWeirdClick, false, false);
				m_p0CheckButtonWeirdClick->signal_toggled().connect(
								sigc::mem_fun(*this, &BttestWindow::onWeirdClickToggled) );

	Gtk::Label* m_p0TabLabelInfo = Gtk::manage(new Gtk::Label("Info"));
	Gtk::ScrolledWindow* m_p0ScrolledInfo = Gtk::manage(new Gtk::ScrolledWindow());
//	m_p0TextViewInfos = Gtk::manage(new Gtk::TextView());
	m_aPageIndex[s_nTabInfo] = m_p0NotebookChoices->append_page(*m_p0ScrolledInfo, *m_p0TabLabelInfo);
		m_p0ScrolledInfo->set_border_width(5);
		m_p0ScrolledInfo->set_policy(Gtk::POLICY_NEVER, Gtk::POLICY_AUTOMATIC);

		Gtk::Label* m_p0LabelInfoText = Gtk::manage(new Gtk::Label(sInfoText));
		m_p0ScrolledInfo->add(*m_p0LabelInfoText);
			m_p0LabelInfoText->set_line_wrap_mode(Pango::WRAP_WORD_CHAR);
//		m_refTextBufferInfo  = Gtk::TextBuffer::create();
//		m_p0TextViewInfos->set_editable(false);
//		m_p0TextViewInfos->set_buffer(m_refTextBufferInfo);
//		m_refTextBufferInfo->set_text(sInfoText);
//		m_p0TextViewInfos->override_font(oMonoFont);

	Gtk::Box* m_p0VBoxConnection = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_VERTICAL));
	m_p0VBoxMain->pack_start(*m_p0VBoxConnection, false, false);

		m_p0LabelServer = Gtk::manage(new Gtk::Label("Server"));
		m_p0VBoxConnection->pack_start(*m_p0LabelServer, false, false);
			m_p0LabelServer->set_margin_top(5);
			m_p0LabelServer->set_margin_bottom(5);

		Gtk::Box* m_p0HBoxConnectionCmds = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_HORIZONTAL));
		m_p0VBoxConnection->pack_start(*m_p0HBoxConnectionCmds, false, false);
			m_p0ButtonConnect = Gtk::manage(new Gtk::Button("Connect"));
			m_p0HBoxConnectionCmds->pack_start(*m_p0ButtonConnect, true, true);
				m_p0ButtonConnect->signal_clicked().connect(
								sigc::mem_fun(*this, &BttestWindow::onButtonConnect) );
			m_p0ButtonDisconnect = Gtk::manage(new Gtk::Button("Disconnect"));
			m_p0HBoxConnectionCmds->pack_start(*m_p0ButtonDisconnect, true, true);
				m_p0ButtonDisconnect->signal_clicked().connect(
								sigc::mem_fun(*this, &BttestWindow::onButtonDisconnect) );
			m_p0ButtonRemove = Gtk::manage(new Gtk::Button("Remove"));
			m_p0HBoxConnectionCmds->pack_start(*m_p0ButtonRemove, true, true);
				m_p0ButtonRemove->signal_clicked().connect(
								sigc::mem_fun(*this, &BttestWindow::onButtonRemove) );

	show_all_children();

	m_oBtKeyClient.m_oStateChangedSignal.connect( sigc::mem_fun(this, &BttestWindow::onClientStateChanged) );
	m_oBtKeyClient.m_oErrorSignal.connect( sigc::mem_fun(this, &BttestWindow::onClientError) );

	m_oBtKeyServers.m_oServersChangedSignal.connect( sigc::mem_fun(this, &BttestWindow::onServersChanged) );
	m_oBtKeyServers.m_oRefreshProgressSignal.connect( sigc::mem_fun(this, &BttestWindow::onServersProgress) );

	assert(! BtKeyServers::isValid(m_oCurServer));
	m_p0LabelServer->set_text("Server: -----------------  Port: -----");

	recreateServersList();
	setSensitivityForState();
}
BttestWindow::~BttestWindow()
{
}
void BttestWindow::setSensitivityForState()
{
	const auto eState = m_oBtKeyClient.getState();
	const bool bDisconnected = (eState == BtKeyClient::STATE_DISCONNECTED);
	const bool bConnected = (eState == BtKeyClient::STATE_CONNECTED);
	const bool bSending = (eState == BtKeyClient::STATE_SENDING);
	const bool bRemoving = (eState == BtKeyClient::STATE_REMOVING);
	m_p0ButtonConnect->set_sensitive(bDisconnected);
	m_p0ButtonDisconnect->set_sensitive((!bDisconnected) && !bRemoving);
	m_p0ButtonRemove->set_sensitive(bConnected || bSending);
	const bool bRefreshing = m_oBtKeyServers.isRefreshing();
	m_p0ButtonChooseServer->set_sensitive(bDisconnected);
	m_p0ButtonRefreshServers->set_sensitive(!bRefreshing);

	m_p0GridKeys->set_sensitive(bConnected || bSending);
}
void BttestWindow::onNotebookSwitchPage(Gtk::Widget*, guint nPageNum)
{
	if (nPageNum >= s_nTotPages) {
		return;
	}
}
void BttestWindow::onServerSelectionChanged()
{
//std::cout << "BttestWindow::onPlayerSelectionChanged()" << '\n';
	Glib::RefPtr<Gtk::TreeSelection> refTreeSelection = m_p0TreeViewServers->get_selection();
	Gtk::TreeModel::iterator it = refTreeSelection->get_selected();
	if (it) {
		Gtk::TreeModel::Row oRow = *it;
		m_sSelectedAddr = oRow[m_oServersColumns.m_oColAddr];
		m_nSelectedPort = oRow[m_oServersColumns.m_oColPort];
		if (m_oCurServer.m_nL2capPort == 0) {
			onChooseServer();
		}
	} else {
		m_nSelectedPort = 0;
	}
	
}
void BttestWindow::onChooseServer()
{
	if (m_nSelectedPort == 0) {
		return;
	}
	BtKeyServers::ServerInfo oTempInfo{};
	oTempInfo.m_oBtAddr = BtKeyServers::getAddrFromString(m_sSelectedAddr);
	oTempInfo.m_nL2capPort = m_nSelectedPort;
	if (! BtKeyServers::isValid(oTempInfo)) {
		return; //--------------------------------------------------------------
	}
	// look if still among servers
	bool bFound = false;
	const auto& aServerInfos = m_oBtKeyServers.getServers();
	for (const auto& oInfo : aServerInfos) {
		if ((BtKeyServers::getStringFromAddr(oInfo.m_oBtAddr) == m_sSelectedAddr) 
				&& (m_nSelectedPort == oInfo.m_nL2capPort)) {
			m_oCurServer = std::move(oInfo);
			bFound = true;
			break; // for-----
		}
	}
	if (bFound) {
		m_p0LabelServer->set_text("Server: " + BtKeyServers::getStringFromAddr(m_oCurServer.m_oBtAddr)
								+ "  Port: " + std::to_string(m_oCurServer.m_nL2capPort));
		m_p0NotebookChoices->set_current_page(s_nTabKeys);
	}
}
void BttestWindow::onRefreshServers()
{
//std::cout << "BttestWindow::onRefreshServers()" << '\n';
	assert(! m_oBtKeyServers.isRefreshing());
	m_oBtKeyServers.startRefreshServers();
	setSensitivityForState();
}
void BttestWindow::printStringToLog(const std::string& sStr)
{
	if (m_nTextBufferLogTotLines >= s_nTextBufferLogMaxLines) {
		auto itLine1 = m_refTextBufferLog->get_iter_at_line(1);
		m_refTextBufferLog->erase(m_refTextBufferLog->begin(), itLine1);
		--m_nTextBufferLogTotLines;
	}
	m_refTextBufferLog->insert(m_refTextBufferLog->end(), sStr + "\n");
	++m_nTextBufferLogTotLines;

	m_refTextBufferLog->place_cursor(m_refTextBufferLog->end());
	m_refTextBufferLog->move_mark(m_refTextBufferMarkBottom, m_refTextBufferLog->end());
	m_p0TextViewLog->scroll_to(m_refTextBufferMarkBottom, 0.1);
}

void BttestWindow::onKeyToggled(int32_t nX, int32_t nY)
{
	if (m_bAvoidNestedToggled) {
		return;
	}
	const int32_t nIdx = nX + nY * s_nKeysGridW;
	const bool bActive = m_aToggleButtonKeys[nIdx]->get_active();
	if (bActive) {
		pressKey(nIdx);
	} else {
		assert(!m_bKeysInButtonMode);
		releaseKey(nIdx);
	}
}
void BttestWindow::pressKey(int32_t nIdx)
{
//std::cout << "BttestWindow::pressKey()  nIdx=" << nIdx << " key=" << m_aKeysNames[nIdx] << '\n';
	printStringToLog(std::string("Sending key ") + m_aKeysNames[nIdx] + " pressed");
	m_oBtKeyClient.sendKeyToServer(hk::KEY_PRESS, m_aKeysHardware[nIdx]);
}
void BttestWindow::releaseKey(int32_t nIdx)
{
//std::cout << "BttestWindow::releaseKey()  nIdx=" << nIdx << " key=" << m_aKeysNames[nIdx] << '\n';
	printStringToLog(std::string("Sending key ") + m_aKeysNames[nIdx] + " released");
	m_oBtKeyClient.sendKeyToServer(hk::KEY_RELEASE, m_aKeysHardware[nIdx]);
}
void BttestWindow::releaseCancelKey(int32_t nIdx)
{
//std::cout << "BttestWindow::releaseCancelKey()  nIdx=" << nIdx << " key=" << m_aKeysNames[nIdx] << '\n';
	printStringToLog(std::string("Sending key ") + m_aKeysNames[nIdx] + " canceled");
	m_oBtKeyClient.sendKeyToServer(hk::KEY_RELEASE_CANCEL, m_aKeysHardware[nIdx]);
}
bool BttestWindow::onReleaseKey(GdkEventButton* /*p0ButtonEvent*/, int32_t nX, int32_t nY, bool bCancel)
{
//std::cout << "BttestWindow::onReleaseKey()  nX=" << nX << " nY=" << nY << " key=" << m_aKeysNames[nX + nY * s_nKeysGridW] << '\n';
	if (m_bKeysInButtonMode) {
		m_bAvoidNestedToggled = true;
		const int32_t nIdx = nX + nY * s_nKeysGridW;
		m_aToggleButtonKeys[nIdx]->set_active(false);
		m_bAvoidNestedToggled = false;
		if (bCancel) {
			releaseCancelKey(nIdx);
		} else {
			releaseKey(nIdx);
		}
	}
	return true;
}
void BttestWindow::onButtonConnect()
{
	m_oBtKeyClient.connectToServer(m_oCurServer.m_oBtAddr, m_oCurServer.m_nL2capPort);
	setSensitivityForState();
}
void BttestWindow::onButtonDisconnect()
{
	m_oBtKeyClient.disconnectFromServer();
	setSensitivityForState();
}
void BttestWindow::onButtonRemove()
{
//	finalizeKeys();
	printStringToLog("Sending removal request ...");
	m_oBtKeyClient.sendRemoveToServer();
	setSensitivityForState();
}
void BttestWindow::onWeirdClickToggled()
{
	const bool bOldKeysInButtonMode = m_bKeysInButtonMode;
	const bool bNewKeysInButtonMode = m_p0CheckButtonWeirdClick->get_active();
	if (bOldKeysInButtonMode == bNewKeysInButtonMode) {
		return; //--------------------------------------------------------------
	}
	m_bKeysInButtonMode = bNewKeysInButtonMode;
	if (!bOldKeysInButtonMode) {
		// was in toggle mode: raise all "pressed in" keys and send a cancel to the server for each
		finalizeKeys();
	} else {
		// was in button mode, all keys should be raised
	}
}
bool BttestWindow::finalizeKeys()
{
	bool bCanceled = false;
	for (int32_t nY = 0; nY < s_nKeysGridH; ++nY) {
		for (int32_t nX = 0; nX < s_nKeysGridW; ++nX) {
			Gtk::ToggleButton*& p0ToggleButton = m_aToggleButtonKeys[nX + nY * s_nKeysGridW];
			const bool bKeyPressed = p0ToggleButton->get_active();
			if (bKeyPressed) {
				bCanceled = true;
				onReleaseKey(nullptr, nX, nY, true);
			}
		}
	}
	return bCanceled;
}
void BttestWindow::onClientStateChanged()
{
	const BtKeyClient::STATE eState = m_oBtKeyClient.getState();
	const std::string sState = [](BtKeyClient::STATE eState)
	{
		switch (eState) {
			case BtKeyClient::STATE_DISCONNECTED: return "Disconnected";
			case BtKeyClient::STATE_CONNECTING: return "Connecting";
			case BtKeyClient::STATE_CONNECTED: return "Connected";
			case BtKeyClient::STATE_SENDING: return "Sending";
//			case BtKeyClient::STATE_DISCONNECTING: return "Disconnecting";
			case BtKeyClient::STATE_REMOVING: return "Removing";
			default : return "Unknown";
		}
	}(eState);
//std::cout << "BttestWindow::onClientStateChanged()  " << sState << '\n';
	printStringToLog( std::string("New state: ") + sState );
	setSensitivityForState();
}
void BttestWindow::onClientError()
{
//std::cout << "BttestWindow::onClientError()  " << m_oBtKeyClient.getError() << '\n';
	printStringToLog(m_oBtKeyClient.getError());
	//setSensitivityForState();
}

void BttestWindow::onServersChanged()
{
	setSensitivityForState();
	const auto& sError = m_oBtKeyServers.getError();
	if (!sError.empty()) {
		m_p0LabelProgress->set_text(sError);
		return;
	}
	recreateServersList();
	m_p0LabelProgress->set_text("Refreshing: Done");
}
void BttestWindow::recreateServersList()
{
	m_refTreeModelServers->clear();

	const auto& aServers = m_oBtKeyServers.getServers();
	for (auto& oInfo : aServers) {
		Gtk::TreeStore::iterator itListServer = m_refTreeModelServers->append();
		(*itListServer)[m_oServersColumns.m_oColAddr] = BtKeyServers::getStringFromAddr(oInfo.m_oBtAddr);
		(*itListServer)[m_oServersColumns.m_oColPort] = oInfo.m_nL2capPort;
		(*itListServer)[m_oServersColumns.m_oColName] = oInfo.m_sName;
	}
	m_p0LabelProgress->set_text("----------");
}
void BttestWindow::onServersProgress(int32_t nSecs)
{
	m_p0LabelProgress->set_text(std::string("Refreshing: ") + std::to_string(nSecs) + " sec");
}

} // namespace stmi

