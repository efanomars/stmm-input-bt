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
 * File:   btkbwindow.cc
 */

#include "btkbwindow.h"
#include "btkeyclient.h"

#include "config.h"
#include "util.h"

#include <giomm/settings.h>

#include <cassert>
#include <iostream>
#include <string>
#include <algorithm>
#include <cstdint>
#include <limits>
#include <utility>
#ifdef STMM_SNAP_PACKAGING
#include <array>

#include <errno.h>
#include <stdio.h>
#include <string.h>
#endif //STMM_SNAP_PACKAGING

#include <bluetooth/bluetooth.h>

#include <unistd.h>

namespace stmi
{

const int32_t BtkbWindow::s_nEditDefaultWeight = 10;
const int32_t BtkbWindow::s_nEditMinWeight = 1;
const int32_t BtkbWindow::s_nEditMaxWeight = 1000;

static const std::string s_sSchemaId = "com.efanomars.stmm-input-btkb";
//const std::string BtkbWindow::s_sConfAppDir = "/apps/stmm-input-btkb";
static const std::string s_sConfKeyTotColumns = "tot-columns";
static const std::string s_sConfKeyTotRows = "tot-rows";
static const std::string s_sConfKeyColumnWeights = "column-weights";
static const std::string s_sConfKeyRowWeights = "row-weights";
static const std::string s_sConfKeyCellKeys = "cell-keys";
static const std::string s_sConfKeyFullscreen = "fullscreen";
static const std::string s_sConfKeyExitButtonPercX = "exit-button-perc-x";
static const std::string s_sConfKeyExitButtonPercY = "exit-button-perc-y";
static const std::string s_sConfKeyExitInCell = "exit-button-in-cell";
static const std::string s_sConfKeyExitButtonInCellColumn = "exit-button-in-cell-column";
static const std::string s_sConfKeyExitButtonInCellRow = "exit-button-in-cell-row";
static const std::string s_sConfKeyLogSentKeys = "log-sent-keys";
static const std::string s_sConfKeySelectedServerAddr = "selected-server-addr";
static const std::string s_sConfKeySelectedServerPort = "selected-server-port";
static const std::string s_sConfKeyWindowWidth = "window-width";
static const std::string s_sConfKeyWindowHeight = "window-height";

static const Glib::ustring s_sScreenNameMain = "Main";
static const Glib::ustring s_sScreenNameEnterAddr = "EnterAddr";
static const Glib::ustring s_sScreenNameEnterWeight = "EnterWeight";
static const Glib::ustring s_sScreenNameEnterKey = "EnterKey";
static const Glib::ustring s_sScreenNameKeyboard = "Keyboard";

#ifdef STMM_SNAP_PACKAGING
static std::string getEnvString(const char* p0Name) noexcept
{
	const char* p0Value = ::secure_getenv(p0Name);
	std::string sValue{(p0Value == nullptr) ? "" : p0Value};
	return sValue;
}
static bool execCmd(const char* sCmd, std::string& sResult, std::string& sError) noexcept
{
	::fflush(nullptr);
	sError.clear();
	sResult.clear();
	std::array<char, 128> aBuffer;
	FILE* pFile = ::popen(sCmd, "r");
	if (pFile == nullptr) {
		sError = std::string("Error: popen() failed: ") + ::strerror(errno) + "(" + std::to_string(errno) + ")";
		return false; //--------------------------------------------------------
	}
	while (!::feof(pFile)) {
		if (::fgets(aBuffer.data(), sizeof(aBuffer), pFile) != nullptr) {
			sResult += aBuffer.data();
		}
	}
	const auto nRet = ::pclose(pFile);
	return (nRet == 0);
}
#endif //STMM_SNAP_PACKAGING

std::string checkEnvironment() noexcept
{
	std::string sError;
	#ifdef STMM_SNAP_PACKAGING
	{
	const std::string sSnapName = getEnvString("SNAP_NAME");
	if (sSnapName.empty()) {
		sError = "SNAP_NAME environment variable not defined!"; //----------------
		return sError;
	}
	std::string sResult;
	if (! execCmd("snapctl is-connected bluetooth-control", sResult, sError)) {
		sError = "Not allowed to use bluetooth."
				"\nPlease grant permission with 'sudo snap connect " + sSnapName + ":bluetooth-control :bluetooth-control'";
	}
	}
	//#else
	//sError = "No errors so far";
	#endif //STMM_SNAP_PACKAGING
	return sError;
}

BtkbWindow::BtkbWindow(const std::string& sTitle, BtKeyClient& oBtKeyClient, BtKeyServers& oBtKeyServers) noexcept
: m_oBtKeyClient(oBtKeyClient)
, m_oBtKeyServers(oBtKeyServers)
, m_bIsFullscreen(false)
{
	// Init data
	// Default weights
	for (int32_t nCol = 0; nCol < m_nTotColumns; ++nCol) {
		m_aEditColumnsWeight.push_back(s_nEditDefaultWeight);
	}
	for (int32_t nRow = 0; nRow < m_nTotRows; ++nRow) {
		m_aEditRowsWeight.push_back(s_nEditDefaultWeight);
	}

	// Default keys
	static_assert(s_nEditInitialGridW <= s_nEditMaxGridW, "");
	static_assert(s_nEditInitialGridH <= s_nEditMaxGridH, "");
	static_assert(s_nEditInitialGridW * s_nEditInitialGridH <= 15, "");
	m_aEditHK.push_back(stmi::hk::HK_A);
	m_aEditHK.push_back(stmi::hk::HK_UP);
	m_aEditHK.push_back(stmi::hk::HK_B);
	m_aEditHK.push_back(stmi::hk::HK_LEFT);
	m_aEditHK.push_back(stmi::hk::HK_SPACE);
	m_aEditHK.push_back(stmi::hk::HK_RIGHT);
	m_aEditHK.push_back(stmi::hk::HK_LEFTCTRL);
	m_aEditHK.push_back(stmi::hk::HK_DOWN);
	m_aEditHK.push_back(stmi::hk::HK_LEFTALT);
	m_aEditHK.push_back(stmi::hk::HK_BTN_A);
	m_aEditHK.push_back(stmi::hk::HK_BTN_B);
	m_aEditHK.push_back(stmi::hk::HK_BTN_C);
	m_aEditHK.push_back(stmi::hk::HK_BTN_X);
	m_aEditHK.push_back(stmi::hk::HK_BTN_Y);
	m_aEditHK.push_back(stmi::hk::HK_BTN_Z);
	while (static_cast<int32_t>(m_aEditHK.size()) < s_nEditMaxGridW * s_nEditMaxGridH) {
		m_aEditHK.push_back(stmi::hk::HK_BTN_SELECT);
	}
	for (auto eHK : m_aEditHK) {
		m_aEditNames.push_back(m_oInputStrings.getKeyString(eHK));
	}

	std::string sInfoTitle{"stmm-input-btkb"};
	std::string sInfoText = std::string("") +
			"\n"
			"Version: " + Config::getVersionString() + "\n"
			"\n"
			"Copyright © 2017-2020  Stefano Marsili, <stemars@gmx.ch>\n"
			"\n"
			"This is a client application for the btkeys bluetooth"
			" protocol to connect to the server started by the"
			" libstmm-input-gtk-bt library, which is  usually loaded as a plugin by a game.\n"
			"\n"
			"To detect the present bluetooth peers press the 'Refresh' button."
			" Per default detection takes up to 15 seconds.\n"
			"After choosing a server by clicking on it in the created list,"
			" press the 'Select' and then the 'Connect' button."
			"If the connection succeeds open the keyboard in the keys tab.\n"
			"\n"
			"The keyboard can be closed by pressing"
			" the green (if connected) or red (if disconnected) square.\n"
			;
	//
	set_title(sTitle);
	set_resizable(true);

	loadStateFromConfig();

	Glib::RefPtr<Gtk::TreeSelection> refTreeSelection;
	Pango::FontDescription oMonoFont("Mono");

	m_refAdjustmentNrColumns = Gtk::Adjustment::create(m_nTotColumns, 1, s_nEditMaxGridW, 1, 1, 0);
	m_refAdjustmentNrRows = Gtk::Adjustment::create(m_nTotRows, 1, s_nEditMaxGridH, 1, 1, 0);
	m_refAdjustmentExitPositionX = Gtk::Adjustment::create(m_nKeysExitButtonPercX, 1, 100, 1, 1, 0);
	m_refAdjustmentExitPositionY = Gtk::Adjustment::create(m_nKeysExitButtonPercY, 1, 100, 1, 1, 0);
	m_refAdjustmentExitInCellC = Gtk::Adjustment::create(m_nKeysExitButtonInCellColumn, 0, s_nEditMaxGridW - 1, 1, 1, 0);
	m_refAdjustmentExitInCellR = Gtk::Adjustment::create(m_nKeysExitButtonInCellRow, 0, s_nEditMaxGridH - 1, 1, 1, 0);

	auto oWeightTreeViewAligner = [&](Gtk::TreeView* p0TreeView) {
		auto p0CellRenderer = p0TreeView->get_column_cell_renderer(0);
		assert(p0CellRenderer != nullptr);
		float fAlighX, fAlighY;
		p0CellRenderer->get_alignment(fAlighX, fAlighY);
		p0CellRenderer->set_alignment(1.0, fAlighY);
		//
		p0CellRenderer = p0TreeView->get_column_cell_renderer(1);
		assert(p0CellRenderer != nullptr);
		p0CellRenderer->get_alignment(fAlighX, fAlighY);
		p0CellRenderer->set_alignment(1.0, fAlighY);
		//
		p0TreeView->set_activate_on_single_click(true);
	};

	m_p0StackScreens = Gtk::manage(new Gtk::Stack());
	Gtk::Window::add(*m_p0StackScreens);
		m_p0StackScreens->set_transition_type(Gtk::StackTransitionType::STACK_TRANSITION_TYPE_NONE);

	m_p0NotebookChoices = Gtk::manage(new Gtk::Notebook());
	m_p0StackScreens->add(*m_p0NotebookChoices, s_sScreenNameMain);
		m_p0NotebookChoices->signal_switch_page().connect(
						sigc::mem_fun(*this, &BtkbWindow::onNotebookSwitchPage) );

	Gtk::Label* m_p0TabLabelServers = Gtk::manage(new Gtk::Label("Connect"));
	Gtk::Box* m_p0VBoxServers = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_VERTICAL));
	m_aPageIndex[s_nTabServers] = m_p0NotebookChoices->append_page(*m_p0VBoxServers, *m_p0TabLabelServers);
		m_p0VBoxServers->set_spacing(5);

		//Create the Servers tree model
		m_refTreeModelServers = Gtk::TreeStore::create(m_oServersColumns);
		m_p0TreeViewServers = Gtk::manage(new Gtk::TreeView(m_refTreeModelServers));
		m_p0VBoxServers->pack_start(*m_p0TreeViewServers, true, true);
			m_p0TreeViewServers->append_column("Addr", m_oServersColumns.m_oColAddr);
			m_p0TreeViewServers->append_column("Port", m_oServersColumns.m_oColPort);
			m_p0TreeViewServers->append_column("Name", m_oServersColumns.m_oColName);
			refTreeSelection = m_p0TreeViewServers->get_selection();
			refTreeSelection->signal_changed().connect(
							sigc::mem_fun(*this, &BtkbWindow::onServerSelectionChanged));

		Gtk::Box* m_p0HBoxServersCmds = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_HORIZONTAL));
		m_p0VBoxServers->pack_start(*m_p0HBoxServersCmds, false, false);
			m_p0ButtonChooseServer = Gtk::manage(new Gtk::Button("Select"));
			m_p0HBoxServersCmds->pack_start(*m_p0ButtonChooseServer, true, true);
				m_p0ButtonChooseServer->signal_clicked().connect(
								sigc::mem_fun(*this, &BtkbWindow::onChooseServer) );
			m_p0ButtonRefreshServers = Gtk::manage(new Gtk::Button("Refresh"));
			m_p0HBoxServersCmds->pack_start(*m_p0ButtonRefreshServers, true, true);
				m_p0ButtonRefreshServers->signal_clicked().connect(
								sigc::mem_fun(*this, &BtkbWindow::onRefreshServers) );
			m_p0LabelProgress = Gtk::manage(new Gtk::Label("Progress"));
			m_p0HBoxServersCmds->pack_start(*m_p0LabelProgress, false, true);

		Gtk::Box* m_p0VBoxConnection = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_VERTICAL));
		m_p0VBoxServers->pack_start(*m_p0VBoxConnection, false, false);
			m_p0VBoxConnection->set_spacing(5);

			Gtk::Box* m_p0HBoxStagedServer = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_HORIZONTAL));
			m_p0VBoxConnection->pack_start(*m_p0HBoxStagedServer, false, false);
				m_p0HBoxStagedServer->set_spacing(5);

				m_p0ButtonInputServer = Gtk::manage(new Gtk::Button("Set"));
				m_p0HBoxStagedServer->pack_start(*m_p0ButtonInputServer, false, true);
					m_p0ButtonInputServer->signal_clicked().connect(
									sigc::mem_fun(*this, &BtkbWindow::onInputStagedServer) );
				m_p0LabelServer = Gtk::manage(new Gtk::Label("Server"));
				m_p0HBoxStagedServer->pack_start(*m_p0LabelServer, true, true);
					m_p0LabelServer->set_halign(Gtk::Align::ALIGN_START);
					m_p0LabelServer->set_margin_top(5);
					m_p0LabelServer->set_margin_bottom(5);

			Gtk::Box* m_p0HBoxConnectionCmds = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_HORIZONTAL));
			m_p0VBoxConnection->pack_start(*m_p0HBoxConnectionCmds, false, false);
				m_p0ButtonConnect = Gtk::manage(new Gtk::Button("Connect"));
				m_p0HBoxConnectionCmds->pack_start(*m_p0ButtonConnect, true, true);
					m_p0ButtonConnect->signal_clicked().connect(
									sigc::mem_fun(*this, &BtkbWindow::onButtonConnect) );
				m_p0ButtonDisconnect = Gtk::manage(new Gtk::Button("Disconnect"));
				m_p0HBoxConnectionCmds->pack_start(*m_p0ButtonDisconnect, true, true);
					m_p0ButtonDisconnect->signal_clicked().connect(
									sigc::mem_fun(*this, &BtkbWindow::onButtonDisconnect) );
				m_p0ButtonRemove = Gtk::manage(new Gtk::Button("Remove"));
				m_p0HBoxConnectionCmds->pack_start(*m_p0ButtonRemove, true, true);
					m_p0ButtonRemove->signal_clicked().connect(
									sigc::mem_fun(*this, &BtkbWindow::onButtonRemove) );

	Gtk::Label* m_p0TabLabelEdit = Gtk::manage(new Gtk::Label("Edit"));
	Gtk::Box* m_p0VBoxEdit = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_VERTICAL));
	m_aPageIndex[s_nTabEdit] = m_p0NotebookChoices->append_page(*m_p0VBoxEdit, *m_p0TabLabelEdit);

		Gtk::Box* m_p0HBoxSize = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_HORIZONTAL));
		m_p0HBoxSize->set_spacing(5);
		m_p0VBoxEdit->pack_start(*m_p0HBoxSize, false, false);
			Gtk::Box* m_p0VBoxColumns = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_VERTICAL));
			m_p0HBoxSize->pack_start(*m_p0VBoxColumns, false, false);
				Gtk::Label* m_p0LabelColumns = Gtk::manage(new Gtk::Label("Width:"));
					m_p0LabelColumns->set_alignment(Gtk::Align::ALIGN_START, m_p0LabelColumns->property_valign());
				m_p0VBoxColumns->pack_start(*m_p0LabelColumns, false, false);
				m_p0SpinColumns = Gtk::manage(new Gtk::SpinButton(m_refAdjustmentNrColumns));
				m_p0VBoxColumns->pack_start(*m_p0SpinColumns, false, false);
					m_p0SpinColumns->signal_value_changed().connect(
									sigc::mem_fun(*this, &BtkbWindow::onSpinNrColumnsChanged) );
				m_refTreeModelColumnWeight = Gtk::TreeStore::create(m_oWeightColumns);
				m_p0TreeViewColumnWeights = Gtk::manage(new Gtk::TreeView(m_refTreeModelColumnWeight));
				m_p0VBoxColumns->pack_start(*m_p0TreeViewColumnWeights, false, false);
					m_p0TreeViewColumnWeights->append_column("Col", m_oWeightColumns.m_oIdx);
					m_p0TreeViewColumnWeights->append_column("Weight", m_oWeightColumns.m_oWeight);
					oWeightTreeViewAligner(m_p0TreeViewColumnWeights);
					m_p0TreeViewColumnWeights->signal_row_activated().connect(
									sigc::mem_fun(*this, &BtkbWindow::onColumnWeightActivated) );
						//on_my_row_activated(const TreeModel::Path& path, TreeViewColumn* column)
			Gtk::Box* m_p0VBoxRows = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_VERTICAL));
			m_p0HBoxSize->pack_start(*m_p0VBoxRows, false, false);
				Gtk::Label* m_p0LabelRows = Gtk::manage(new Gtk::Label("Height:"));
					m_p0LabelRows->set_alignment(Gtk::Align::ALIGN_START, m_p0LabelRows->property_valign());
				m_p0VBoxRows->pack_start(*m_p0LabelRows, false, false);
				m_p0SpinRows = Gtk::manage(new Gtk::SpinButton(m_refAdjustmentNrRows));
				m_p0VBoxRows->pack_start(*m_p0SpinRows, false, false);
					m_p0SpinRows->signal_value_changed().connect(
									sigc::mem_fun(*this, &BtkbWindow::onSpinNrRowsChanged) );
				m_refTreeModelRowWeight = Gtk::TreeStore::create(m_oWeightColumns);
				m_p0TreeViewRowWeights = Gtk::manage(new Gtk::TreeView(m_refTreeModelRowWeight));
				m_p0VBoxRows->pack_start(*m_p0TreeViewRowWeights, false, false);
					m_p0TreeViewRowWeights->append_column("Row", m_oWeightColumns.m_oIdx);
					m_p0TreeViewRowWeights->append_column("Weight", m_oWeightColumns.m_oWeight);
					oWeightTreeViewAligner(m_p0TreeViewRowWeights);
					m_p0TreeViewRowWeights->signal_row_activated().connect(
									sigc::mem_fun(*this, &BtkbWindow::onRowWeightActivated) );
						//onRowWeightActivated(const TreeModel::Path& oPath, TreeViewColumn* p0Column)
		Gtk::ScrolledWindow* m_p0ScrolledCellKey = Gtk::manage(new Gtk::ScrolledWindow());
		m_p0VBoxEdit->pack_start(*m_p0ScrolledCellKey, true, true);
			m_refTreeModelCellKey = Gtk::TreeStore::create(m_oCellKeyColumns);
			m_p0TreeViewCellKey = Gtk::manage(new Gtk::TreeView(m_refTreeModelCellKey));
			m_p0ScrolledCellKey->add(*m_p0TreeViewCellKey);
				m_p0TreeViewCellKey->append_column("Col", m_oCellKeyColumns.m_oColX);
				m_p0TreeViewCellKey->append_column("Row", m_oCellKeyColumns.m_oColY);
				m_p0TreeViewCellKey->append_column("Key", m_oCellKeyColumns.m_oColKeyName);
				m_p0TreeViewCellKey->set_activate_on_single_click(true);
				m_p0TreeViewCellKey->signal_row_activated().connect(
								sigc::mem_fun(*this, &BtkbWindow::onCellNameActivated) );

	Gtk::Label* m_p0TabLabelKeys = Gtk::manage(new Gtk::Label("Keys"));
	Gtk::Box* m_p0VBoxKeys = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_VERTICAL));
	m_aPageIndex[s_nTabKeys] = m_p0NotebookChoices->append_page(*m_p0VBoxKeys, *m_p0TabLabelKeys);
			m_p0VBoxKeys->set_spacing(10);
		m_p0CheckButtonFullscreen = Gtk::manage(new Gtk::CheckButton("Fullscreen"));
		m_p0VBoxKeys->pack_start(*m_p0CheckButtonFullscreen, false, false);
			m_p0CheckButtonFullscreen->set_active(m_bKeysFullscreen);
			m_p0CheckButtonFullscreen->signal_clicked().connect(
							sigc::mem_fun(*this, &BtkbWindow::onKeysFullscreenChanged));
		Gtk::Box* m_p0VBoxExitPosition = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_VERTICAL));
		m_p0VBoxKeys->pack_start(*m_p0VBoxExitPosition, false, false);
			Gtk::Label* m_p0LabelExitPosition = Gtk::manage(new Gtk::Label("Exit button position (0..100)"));
				m_p0LabelExitPosition->set_alignment(Gtk::Align::ALIGN_START, m_p0LabelExitPosition->property_valign());
			m_p0VBoxExitPosition->pack_start(*m_p0LabelExitPosition, false, false);
			Gtk::Box* m_p0HBoxExitPositionXY = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_HORIZONTAL));
			m_p0VBoxExitPosition->pack_start(*m_p0HBoxExitPositionXY, false, false);
				Gtk::Label* m_p0LabelExitPositionX = Gtk::manage(new Gtk::Label(" X: "));
				m_p0HBoxExitPositionXY->pack_start(*m_p0LabelExitPositionX, false, false);
				m_p0SpinExitPositionX = Gtk::manage(new Gtk::SpinButton(m_refAdjustmentExitPositionX));
				m_p0HBoxExitPositionXY->pack_start(*m_p0SpinExitPositionX, false, false);
					m_p0SpinExitPositionX->signal_value_changed().connect(
								sigc::mem_fun(*this, &BtkbWindow::onSpinExitPositionXChanged) );
				Gtk::Label* m_p0LabelExitPositionY = Gtk::manage(new Gtk::Label("  Y: "));
				m_p0HBoxExitPositionXY->pack_start(*m_p0LabelExitPositionY, false, false);
				m_p0SpinExitPositionY = Gtk::manage(new Gtk::SpinButton(m_refAdjustmentExitPositionY));
				m_p0HBoxExitPositionXY->pack_start(*m_p0SpinExitPositionY, false, false);
					m_p0SpinExitPositionY->signal_value_changed().connect(
								sigc::mem_fun(*this, &BtkbWindow::onSpinExitPositionYChanged) );
		m_p0CheckButtonInCell = Gtk::manage(new Gtk::CheckButton("Within cell"));
		m_p0VBoxKeys->pack_start(*m_p0CheckButtonInCell, false, false);
			m_p0CheckButtonInCell->set_active(m_bKeysExitInCell);
			m_p0CheckButtonInCell->signal_clicked().connect(
							sigc::mem_fun(*this, &BtkbWindow::onKeysExitInCellChanged));
		Gtk::Box* m_p0HBoxInCellCR = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_HORIZONTAL));
		m_p0VBoxKeys->pack_start(*m_p0HBoxInCellCR, false, false);
			Gtk::Label* m_p0LabelInCellC = Gtk::manage(new Gtk::Label(" C: "));
			m_p0HBoxInCellCR->pack_start(*m_p0LabelInCellC, false, false);
			m_p0SpinExitInCellC = Gtk::manage(new Gtk::SpinButton(m_refAdjustmentExitInCellC));
			m_p0HBoxInCellCR->pack_start(*m_p0SpinExitInCellC, false, false);
				m_p0SpinExitInCellC->signal_value_changed().connect(
							sigc::mem_fun(*this, &BtkbWindow::onSpinExitInCellColumnChanged) );
			Gtk::Label* m_p0LabelInCellR = Gtk::manage(new Gtk::Label("  R: "));
			m_p0HBoxInCellCR->pack_start(*m_p0LabelInCellR, false, false);
			m_p0SpinExitInCellR = Gtk::manage(new Gtk::SpinButton(m_refAdjustmentExitInCellR));
			m_p0HBoxInCellCR->pack_start(*m_p0SpinExitInCellR, false, false);
				m_p0SpinExitInCellR->signal_value_changed().connect(
							sigc::mem_fun(*this, &BtkbWindow::onSpinExitInCellRowChanged) );
		m_p0CheckButtonLogSentKeys = Gtk::manage(new Gtk::CheckButton("Log sent keys"));
		m_p0VBoxKeys->pack_start(*m_p0CheckButtonLogSentKeys, false, false);
			m_p0CheckButtonLogSentKeys->set_active(m_bKeysLogSentKeys);
			m_p0CheckButtonLogSentKeys->signal_clicked().connect(
							sigc::mem_fun(*this, &BtkbWindow::onKeysLogSentKeysChanged));
		Gtk::Button* m_p0ButtonKeyboard = Gtk::manage(new Gtk::Button("Keyboard"));
		m_p0VBoxKeys->pack_start(*m_p0ButtonKeyboard, true, true);
			m_p0ButtonKeyboard->signal_clicked().connect(
							sigc::mem_fun(*this, &BtkbWindow::onButtonKeyboard) );

	Gtk::Label* m_p0TabLabelLog = Gtk::manage(new Gtk::Label("Log"));
	Gtk::ScrolledWindow* m_p0ScrolledLog = Gtk::manage(new Gtk::ScrolledWindow());
	m_aPageIndex[s_nTabLog] = m_p0NotebookChoices->append_page(*m_p0ScrolledLog, *m_p0TabLabelLog);
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
				const std::string sInitialLog = checkEnvironment();
				printStringToLog(sInitialLog);

	Gtk::Label* m_p0TabLabelInfo = Gtk::manage(new Gtk::Label("Info"));
	Gtk::ScrolledWindow* m_p0ScrolledInfo = Gtk::manage(new Gtk::ScrolledWindow());
	m_aPageIndex[s_nTabInfo] = m_p0NotebookChoices->append_page(*m_p0ScrolledInfo, *m_p0TabLabelInfo);
		m_p0ScrolledInfo->set_border_width(5);
		m_p0ScrolledInfo->set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);

		Gtk::Box* m_p0VBoxInfoText = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_VERTICAL));
		m_p0ScrolledInfo->add(*m_p0VBoxInfoText);
			Gtk::Label* m_p0LabelInfoTitle = Gtk::manage(new Gtk::Label(sInfoTitle));
			m_p0VBoxInfoText->pack_start(*m_p0LabelInfoTitle, false, false);
				m_p0LabelInfoTitle->set_halign(Gtk::Align::ALIGN_START);
				m_p0LabelInfoTitle->set_margin_top(3);
				m_p0LabelInfoTitle->set_margin_bottom(3);
				{
				Pango::AttrList oAttrList;
				Pango::AttrInt oAttrWeight = Pango::Attribute::create_attr_weight(Pango::WEIGHT_HEAVY);
				oAttrList.insert(oAttrWeight);
				m_p0LabelInfoTitle->set_attributes(oAttrList);
				}

			Gtk::Label* m_p0LabelInfoText = Gtk::manage(new Gtk::Label(sInfoText));
			m_p0VBoxInfoText->pack_start(*m_p0LabelInfoText, false, false);
				m_p0LabelInfoText->set_halign(Gtk::Align::ALIGN_START);
				m_p0LabelInfoText->set_line_wrap(true);
				m_p0LabelInfoText->set_line_wrap_mode(Pango::WRAP_WORD_CHAR);
	m_aScreens[s_nScreenMain] = m_p0NotebookChoices;

	m_refAddrScreen = std::make_unique<AddrScreen>(*this);
	m_p0ScreenBoxEnterAddr = m_refAddrScreen->init();
	m_p0StackScreens->add(*m_p0ScreenBoxEnterAddr, s_sScreenNameEnterAddr);
	m_aScreens[s_nScreenEnterAddr] = m_p0ScreenBoxEnterAddr;

	m_refWeightScreen = std::make_unique<WeightScreen>(*this, s_nEditMinWeight, s_nEditMaxWeight);
	m_p0ScreenBoxEnterWeight = m_refWeightScreen->init();
	m_p0StackScreens->add(*m_p0ScreenBoxEnterWeight, s_sScreenNameEnterWeight);
	m_aScreens[s_nScreenEnterWeight] = m_p0ScreenBoxEnterWeight;

	m_refKeyScreen = std::make_unique<KeyScreen>(*this, m_oInputStrings);
	m_p0ScreenBoxEnterKey = m_refKeyScreen->init();
	m_p0StackScreens->add(*m_p0ScreenBoxEnterKey, s_sScreenNameEnterKey);
	m_aScreens[s_nScreenEnterKey] = m_p0ScreenBoxEnterKey;

	m_refKeyboardScreen = std::make_unique<KeyboardScreen>(*this, m_oInputStrings);
	m_p0ScreenBoxKeyboard = m_refKeyboardScreen->init();
	m_p0StackScreens->add(*m_p0ScreenBoxKeyboard, s_sScreenNameKeyboard);
	m_aScreens[s_nScreenKeyboard] = m_p0ScreenBoxKeyboard;

	show_all_children();

	m_oBtKeyClient.m_oStateChangedSignal.connect( sigc::mem_fun(this, &BtkbWindow::onClientStateChanged) );
	m_oBtKeyClient.m_oErrorSignal.connect( sigc::mem_fun(this, &BtkbWindow::onClientError) );

	m_oBtKeyServers.m_oServersChangedSignal.connect( sigc::mem_fun(this, &BtkbWindow::onServersChanged) );
	m_oBtKeyServers.m_oRefreshProgressSignal.connect( sigc::mem_fun(this, &BtkbWindow::onServersProgress) );

	//assert(BtKeyServers::isValid(m_oCurServer));
	setStagedServerAndPort();

	m_p0CheckButtonFullscreen->set_active(m_bKeysFullscreen);
	m_p0CheckButtonInCell->set_active(m_bKeysExitInCell);
	m_p0CheckButtonLogSentKeys->set_active(m_bKeysLogSentKeys);
	recreateServersList();
	recreateWeightsList();
	recreateCellsKeyList();
	updateInCellColumn();
	updateInCellRow();
	onKeysFullscreenChanged();
	onKeysExitInCellChanged();

	setSensitivityForState();

	// Track fullscreen changes
	addWindowEventTypes(Gdk::STRUCTURE_MASK);
	signal_window_state_event().connect(sigc::mem_fun(*this, &BtkbWindow::onWindowStateEventChanged));
}
void BtkbWindow::addWindowEventTypes(const Gdk::EventMask oBitsToAdd) noexcept
{
	if (get_realized()) {
		add_events(oBitsToAdd);
	} else {
		const Gdk::EventMask oCurMask = get_events();
		const Gdk::EventMask oNewMask = oCurMask | oBitsToAdd;
		if (oNewMask != oCurMask) {
			set_events(oNewMask);
		}
	}
}
bool BtkbWindow::onWindowStateEventChanged(GdkEventWindowState* p0WindowStateEvent) noexcept
{
	const GdkWindowState oBitsChanges = p0WindowStateEvent->changed_mask; //mask specifying what flags have changed.
	if ((oBitsChanges & ::GDK_WINDOW_STATE_FULLSCREEN) == 0) {
		// propagate
		return false; // -------------------------------------------------------
	}
	const GdkWindowState oNewState = p0WindowStateEvent->new_window_state; //The new window state, a combination of GdkWindowState bits.
	m_bIsFullscreen = ((oNewState & ::GDK_WINDOW_STATE_FULLSCREEN) != 0);
	return false;
}
bool BtkbWindow::isFullscreen() const noexcept
{
	return m_bIsFullscreen;
}
void BtkbWindow::setSensitivityForState() noexcept
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
	m_p0ButtonInputServer->set_sensitive(bDisconnected);
	m_p0ButtonRefreshServers->set_sensitive(!bRefreshing);
	if (m_refKeyboardScreen) {
		m_refKeyboardScreen->connectionStateChanged();
	}
}
void BtkbWindow::changeScreen(int32_t nToScreen, const std::string& /*sMsg*/) noexcept
{
	if (m_nCurrentScreen == nToScreen) {
		return;
	}
	m_nCurrentScreen = nToScreen;
	m_p0StackScreens->set_visible_child(*m_aScreens[m_nCurrentScreen]);
}
void BtkbWindow::onNotebookSwitchPage(Gtk::Widget*, guint nPageNum) noexcept
{
	if (nPageNum >= s_nTotPages) {
		return;
	}
	//if (nPageNum == m_aPageIndex[s_nTabEdit]) {
	//}
}
bool BtkbWindow::on_key_press_event(GdkEventKey* p0Event)
{
//std::cout << "BtkbWindow::on_key_press_event()  key=" << p0Event->keyval << '\n';
	if (m_nCurrentScreen == s_nScreenEnterKey) {
		const bool bConsumed = m_refKeyScreen->onKeyPressed(p0Event);
		if (bConsumed) {
			return true; //-----------------------------------------------------
		}
	} else if (m_nCurrentScreen == s_nScreenKeyboard) {
		return false; //--------------------------------------------------------
	}
	return Gtk::Window::on_key_press_event(p0Event);
}
void BtkbWindow::onServerSelectionChanged() noexcept
{
//std::cout << "BtkbWindow::onServerSelectionChanged()" << '\n';
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
void BtkbWindow::onChooseServer() noexcept
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
		setStagedServerAndPort();
		//m_p0NotebookChoices->set_current_page(s_nTabKeys);
	}
}
void BtkbWindow::setStagedServerAndPort() noexcept
{
//std::cout << "BtkbWindow::setStagedServerAndPort() server=" << BtKeyServers::getStringFromAddr(m_oCurServer.m_oBtAddr) << '\n';
//std::cout << "BtkbWindow::setStagedServerAndPort()   port=" << m_oCurServer.m_nL2capPort << '\n';
	if (BtKeyServers::isValidPort(m_oCurServer.m_nL2capPort)) {
		m_p0LabelServer->set_text("Server: " + BtKeyServers::getStringFromAddr(m_oCurServer.m_oBtAddr)
								+ "  Port: " + std::to_string(m_oCurServer.m_nL2capPort));
	} else {
		m_p0LabelServer->set_text("Server: -----------------  Port: -----");
	}
}
void BtkbWindow::onRefreshServers() noexcept
{
//std::cout << "BtkbWindow::onRefreshServers()" << '\n';
	assert(! m_oBtKeyServers.isRefreshing());
	m_oBtKeyServers.startRefreshServers();
	setSensitivityForState();
}
void BtkbWindow::onInputStagedServer() noexcept
{
	changeScreen(s_nScreenEnterAddr, "");
	std::string sAddr = (BtKeyServers::isValidPort(m_oCurServer.m_nL2capPort) && BtKeyServers::isValidAddr(m_oCurServer.m_oBtAddr)
							? BtKeyServers::getStringFromAddr(m_oCurServer.m_oBtAddr)
							: "");
//std::cout << "-------> sAddr=" << sAddr << '\n';
	m_refAddrScreen->changeTo(sAddr);
}
void BtkbWindow::setAddr(const std::string& sAddr) noexcept
{
	if (sAddr.empty()) {
		m_oCurServer.m_oBtAddr = BtKeyServers::getEmptyAddr();
		m_oCurServer.m_nL2capPort = 0;
	} else {
		m_oCurServer.m_oBtAddr = BtKeyServers::getAddrFromString(sAddr);
		if (! BtKeyServers::isValidPort(m_oCurServer.m_nL2capPort)) {
			m_oCurServer.m_nL2capPort = BtKeyServers::s_nDefaultL2capPort;
		}
	}
	setStagedServerAndPort();
}
void BtkbWindow::setWeight(int32_t nWeight, bool bColumn, int32_t nColRow, const Gtk::TreeModel::Path& oPath) noexcept
{
	if (bColumn) {
		const int32_t nCol = nColRow;
		assert((nCol >= 0) && (nCol < static_cast<int32_t>(m_aEditColumnsWeight.size())));
		Gtk::TreeStore::iterator itListWeight = m_refTreeModelColumnWeight->get_iter(oPath);
		m_aEditColumnsWeight[nCol] = nWeight;
		(*itListWeight)[m_oWeightColumns.m_oWeight] = nWeight;
	} else {
		const int32_t nRow = nColRow;
		assert((nRow >= 0) && (nRow < static_cast<int32_t>(m_aEditRowsWeight.size())));
		Gtk::TreeStore::iterator itListWeight = m_refTreeModelRowWeight->get_iter(oPath);
		m_aEditRowsWeight[nRow] = nWeight;
		(*itListWeight)[m_oWeightColumns.m_oWeight] = nWeight;
	}
}
void BtkbWindow::setKey(hk::HARDWARE_KEY eKey, int32_t nColumn, int32_t nRow, const Gtk::TreeModel::Path& oPath) noexcept
{
	Gtk::TreeStore::iterator itListKeyName = m_refTreeModelCellKey->get_iter(oPath);
	const int32_t nIdx = nColumn + nRow * m_aEditColumnsWeight.size();
	m_aEditHK[nIdx] = eKey;
//std::cout << "BtkbWindow::setKey() nRow=" << nRow << " nColumn=" << nColumn << " eKey=" << static_cast<int32_t>(eKey) << " nIdx=" << nIdx << '\n';
	m_aEditNames[nIdx] = m_oInputStrings.getKeyString(eKey);
	(*itListKeyName)[m_oCellKeyColumns.m_oColHiddenHardwareKey] = eKey;
	(*itListKeyName)[m_oCellKeyColumns.m_oColKeyName] = m_aEditNames[nIdx];

}
void BtkbWindow::printStringToLog(const std::string& sStr) noexcept
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
void BtkbWindow::onSpinNrColumnsChanged() noexcept
{
	const int32_t nNewTotColumns = m_p0SpinColumns->get_value_as_int();
	while (m_nTotColumns < nNewTotColumns) {
		m_aEditColumnsWeight.push_back(s_nEditDefaultWeight);
		++m_nTotColumns;
	}
	while (m_nTotColumns > nNewTotColumns) {
		m_aEditColumnsWeight.pop_back();
		--m_nTotColumns;
	}
	recreateWeightsList();
	recreateCellsKeyList();
	if (m_nKeysExitButtonInCellColumn >= m_nTotColumns) {
		m_nKeysExitButtonInCellColumn = m_nTotColumns - 1;
	}
	updateInCellColumn();
}
void BtkbWindow::onSpinNrRowsChanged() noexcept
{
	const int32_t nNewTotRows = m_p0SpinRows->get_value_as_int();
	while (m_nTotRows < nNewTotRows) {
		m_aEditRowsWeight.push_back(s_nEditDefaultWeight);
		++m_nTotRows;
	}
	while (m_nTotRows > nNewTotRows) {
		m_aEditRowsWeight.pop_back();
		--m_nTotRows;
	}
	recreateWeightsList();
	recreateCellsKeyList();
	if (m_nKeysExitButtonInCellRow >= m_nTotRows) {
		m_nKeysExitButtonInCellRow = m_nTotRows - 1;
	}
	updateInCellRow();
}
void BtkbWindow::onColumnWeightActivated(const Gtk::TreeModel::Path& oPath, Gtk::TreeViewColumn* /*p0Column*/) noexcept
{
//std::cout << " onColumnWeightActivated()" << '\n';
	changeScreen(s_nScreenEnterWeight, "");
	Gtk::TreeStore::iterator itListWeight = m_refTreeModelColumnWeight->get_iter(oPath);
	const int32_t nWeight = (*itListWeight)[m_oWeightColumns.m_oWeight];
	const int32_t nCol = (*itListWeight)[m_oWeightColumns.m_oIdx];
	assert((nCol >= 0) && (nCol < static_cast<int32_t>(m_aEditColumnsWeight.size())));
	assert(m_aEditColumnsWeight[nCol] == nWeight);
	//
	m_refWeightScreen->changeTo(nWeight, true, nCol, oPath);
}
void BtkbWindow::onRowWeightActivated(const Gtk::TreeModel::Path& oPath, Gtk::TreeViewColumn* /*p0Column*/) noexcept
{
//std::cout << " onRowWeightActivated()" << '\n';
	changeScreen(s_nScreenEnterWeight, "");
	Gtk::TreeStore::iterator itListWeight = m_refTreeModelRowWeight->get_iter(oPath);
	const int32_t nWeight = (*itListWeight)[m_oWeightColumns.m_oWeight];
	const int32_t nRow = (*itListWeight)[m_oWeightColumns.m_oIdx];
	assert((nRow >= 0) && (nRow < static_cast<int32_t>(m_aEditRowsWeight.size())));
	assert(m_aEditRowsWeight[nRow] == nWeight);
	//
	m_refWeightScreen->changeTo(nWeight, false, nRow, oPath);
}
void BtkbWindow::onCellNameActivated(const Gtk::TreeModel::Path& oPath, Gtk::TreeViewColumn* /*p0Column*/) noexcept
{
	changeScreen(s_nScreenEnterKey, "");
	Gtk::TreeStore::iterator itListKeyName = m_refTreeModelCellKey->get_iter(oPath);
	const int32_t nColumn = (*itListKeyName)[m_oCellKeyColumns.m_oColX];
	const int32_t nRow = (*itListKeyName)[m_oCellKeyColumns.m_oColY];
	assert((nColumn >= 0) && (nColumn < static_cast<int32_t>(m_aEditColumnsWeight.size())));
	assert((nRow >= 0) && (nRow < static_cast<int32_t>(m_aEditRowsWeight.size())));
	const int32_t nHK = (*itListKeyName)[m_oCellKeyColumns.m_oColHiddenHardwareKey];
	hk::HARDWARE_KEY eKey = static_cast<hk::HARDWARE_KEY>(nHK);
	#ifndef NDEBUG
	const int32_t nIdx = nColumn + nRow * m_aEditColumnsWeight.size();
	assert(nIdx < static_cast<int32_t>(m_aEditHK.size()));
	assert(m_aEditHK[nIdx] == eKey);
	#endif //NDEBUG
//std::cout << "BtkbWindow::onCellNameActivated() nRow=" << nRow << " nColumn=" << nColumn << " eKey=" << nHK << " nIdx=" << nIdx << '\n';
	//
	m_refKeyScreen->changeTo(eKey, nColumn, nRow, oPath);
}
void BtkbWindow::onKeysFullscreenChanged() noexcept
{
	m_bKeysFullscreen = m_p0CheckButtonFullscreen->get_active();
//std::cout << "BtkbWindow::onKeysFullscreenChanged()  m_bKeysFullscreen=" << m_bKeysFullscreen << '\n';
}
void BtkbWindow::onSpinExitPositionXChanged() noexcept
{
	m_nKeysExitButtonPercX = m_p0SpinExitPositionX->get_value_as_int();
}
void BtkbWindow::onSpinExitPositionYChanged() noexcept
{
	m_nKeysExitButtonPercY = m_p0SpinExitPositionY->get_value_as_int();
}
void BtkbWindow::onKeysExitInCellChanged() noexcept
{
	m_bKeysExitInCell = m_p0CheckButtonInCell->get_active();
	m_p0SpinExitInCellC->set_sensitive(m_bKeysExitInCell);
	m_p0SpinExitInCellR->set_sensitive(m_bKeysExitInCell);
}
void BtkbWindow::onSpinExitInCellColumnChanged() noexcept
{
	m_nKeysExitButtonInCellColumn = m_p0SpinExitInCellC->get_value_as_int();
}
void BtkbWindow::onSpinExitInCellRowChanged() noexcept
{
	m_nKeysExitButtonInCellRow = m_p0SpinExitInCellR->get_value_as_int();
}
void BtkbWindow::onKeysLogSentKeysChanged() noexcept
{
	m_bKeysLogSentKeys = m_p0CheckButtonLogSentKeys->get_active();
}
void BtkbWindow::onButtonKeyboard() noexcept
{
//std::cout << "BtkbWindow::onButtonKeyboard m_bKeysFullscreen=" << m_bKeysFullscreen << '\n';
	changeScreen(s_nScreenKeyboard, "");
//std::cout << "BtkbWindow::onButtonKeyboard() -----------------------" << '\n';
	m_refKeyboardScreen->changeTo(m_bKeysFullscreen);
}

void BtkbWindow::onButtonConnect() noexcept
{
	m_oBtKeyClient.connectToServer(m_oCurServer.m_oBtAddr, m_oCurServer.m_nL2capPort);
	setSensitivityForState();
}
void BtkbWindow::onButtonDisconnect() noexcept
{
	m_oBtKeyClient.disconnectFromServer();
	setSensitivityForState();
}
void BtkbWindow::onButtonRemove() noexcept
{
	finalizeKeys();
	printStringToLog("Sending removal request ...");
	m_oBtKeyClient.sendRemoveToServer();
	setSensitivityForState();
}
void BtkbWindow::finalizeKeys() noexcept
{
	auto aPressedKeys = m_aPressedKeys;
	for (const auto& oPK : aPressedKeys) {
		releaseKey(oPK.m_eKey, oPK.m_eType, oPK.m_p0Finger, true);
	}
}
bool BtkbWindow::isCellKeyPressed(int32_t nColumn, int32_t nRow) const noexcept
{
	assert((nColumn < m_nTotColumns) && (nRow < m_nTotRows));
	const int32_t nColRow = nColumn + nRow * m_nTotColumns;
	const hk::HARDWARE_KEY eKey = m_aEditHK[nColRow];
	return isKeyPressed(eKey);
}
bool BtkbWindow::isKeyPressed(hk::HARDWARE_KEY eKey) const noexcept
{
	const auto itFind = std::find_if(m_aPressedKeys.begin(), m_aPressedKeys.end(), [&](const PressedKey& oPK) {
		return oPK.m_eKey == eKey;
	});
	const bool bFound = (itFind != m_aPressedKeys.end());
	return bFound;
}
bool BtkbWindow::pressKeyboardKey(hk::HARDWARE_KEY eKey) noexcept
{
	assert(eKey != hk::HK_NULL);
	return pressKey(eKey, PRESS_TYPE_KEYBOARD, nullptr);
}
bool BtkbWindow::pressMouseKey(hk::HARDWARE_KEY eKey) noexcept
{
	assert(eKey != hk::HK_NULL);
	return pressKey(eKey, PRESS_TYPE_MOUSE, nullptr);
}
bool BtkbWindow::pressFingerKey(hk::HARDWARE_KEY eKey, const GdkEventSequence* p0Finger) noexcept
{
	assert(eKey != hk::HK_NULL);
	return pressKey(eKey, PRESS_TYPE_TOUCH, p0Finger);
}
bool BtkbWindow::releaseKeyboardKey(hk::HARDWARE_KEY eKey) noexcept
{
	assert(eKey != hk::HK_NULL);
	return releaseKey(eKey, PRESS_TYPE_KEYBOARD, nullptr, false);
}
bool BtkbWindow::releaseMouseKey() noexcept
{
	return releaseKey(hk::HK_NULL, PRESS_TYPE_MOUSE, nullptr, false);
}
bool BtkbWindow::releaseFingerKey(const GdkEventSequence* p0Finger, bool bCancel) noexcept
{
	return releaseKey(hk::HK_NULL, PRESS_TYPE_TOUCH, p0Finger, bCancel);
}
bool BtkbWindow::pressKey(hk::HARDWARE_KEY eKey, PRESS_TYPE eType, const GdkEventSequence* p0Finger) noexcept
{
	bool bKeyAlreadyPressed = false;
	for (auto itCur = m_aPressedKeys.begin(); itCur != m_aPressedKeys.end(); ++itCur) {
		const bool bSameKey = (itCur->m_eKey == eKey);
		if (bSameKey) {
			bKeyAlreadyPressed = true;
		}
		if (bSameKey && (itCur->m_eType == eType) && (itCur->m_p0Finger == p0Finger)) {
			// already pressed
			return false; //------------------------------------------------
		}
	}
	PressedKey oPK;
	oPK.m_eKey = eKey;
	oPK.m_eType = eType;
	oPK.m_p0Finger = p0Finger;
	m_aPressedKeys.push_back(std::move(oPK));

	if (bKeyAlreadyPressed) {
		// Key wasn't already pressed by some other input, no need to send
		return true; //---------------------------------------------------------
	}
	if (m_bKeysLogSentKeys && (m_oBtKeyClient.getState() == BtKeyClient::STATE_CONNECTED)) {
		printStringToLog(std::string("Sending key ") + m_oInputStrings.getKeyString(eKey) + " pressed");
	}
	m_oBtKeyClient.sendKeyToServer(hk::KEY_PRESS, eKey);
	return true;
}
bool BtkbWindow::releaseKey(hk::HARDWARE_KEY eKey, PRESS_TYPE eType, const GdkEventSequence* p0Finger, bool bCancel) noexcept
{
	const auto itFind = std::find_if(m_aPressedKeys.begin(), m_aPressedKeys.end(), [&](const PressedKey& oPK) {
		const bool bKeyMatch = (eKey == hk::HK_NULL) || (oPK.m_eKey == eKey);
		return bKeyMatch && (oPK.m_eType == eType) && (oPK.m_p0Finger == p0Finger);
	});
	const bool bFound = (itFind != m_aPressedKeys.end());
	if (!bFound) {
		// wasn't pressed
		return false; //--------------------------------------------------------
	}
	eKey = itFind->m_eKey;
	m_aPressedKeys.erase(itFind);

	// look if the key is still pressed by other input
	const auto itFindPressed = std::find_if(m_aPressedKeys.begin(), m_aPressedKeys.end(), [&](const PressedKey& oPK) {
		return (oPK.m_eKey == eKey);
	});
	const bool bKeyStillPressed = (itFindPressed != m_aPressedKeys.end());
	if (bKeyStillPressed) {
		return true; //---------------------------------------------------------
	}

	if (m_bKeysLogSentKeys && (m_oBtKeyClient.getState() == BtKeyClient::STATE_CONNECTED)) {
		printStringToLog(std::string("Sending key ") + m_oInputStrings.getKeyString(eKey) + " " + (bCancel ? "canceled" : "released"));
	}
	m_oBtKeyClient.sendKeyToServer((bCancel ? hk::KEY_RELEASE_CANCEL : hk::KEY_RELEASE), eKey);
	return true;
}
void BtkbWindow::onClientStateChanged() noexcept
{
	const BtKeyClient::STATE eState = m_oBtKeyClient.getState();
	const std::string sState = [](BtKeyClient::STATE eState)
	{
		switch (eState) {
			case BtKeyClient::STATE_DISCONNECTED: return "Disconnected";
			case BtKeyClient::STATE_CONNECTING: return "Connecting";
			case BtKeyClient::STATE_CONNECTED: return "Connected";
			case BtKeyClient::STATE_SENDING: return "Sending";
			case BtKeyClient::STATE_REMOVING: return "Removing";
			default : return "Unknown";
		}
	}(eState);
//std::cout << "BtkbWindow::onClientStateChanged()  " << sState << '\n';
	printStringToLog( std::string("New state: ") + sState );
	setSensitivityForState();
}
void BtkbWindow::onClientError() noexcept
{
//std::cout << "BtkbWindow::onClientError()  " << m_oBtKeyClient.getError() << '\n';
	printStringToLog(m_oBtKeyClient.getError());
	//setSensitivityForState();
}

void BtkbWindow::onServersChanged() noexcept
{
	setSensitivityForState();
	const auto& sError = m_oBtKeyServers.getError();
	if (!sError.empty()) {
		printStringToLog(std::string("Refreshing: ") + sError);
		m_p0LabelProgress->set_text("Refreshing: Error");
		return;
	}
	recreateServersList();
	m_p0LabelProgress->set_text("Refreshing: Done");
}
void BtkbWindow::recreateServersList() noexcept
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
void BtkbWindow::onServersProgress(int32_t nSecs) noexcept
{
	m_p0LabelProgress->set_text(std::string("Refreshing: ") + std::to_string(nSecs) + " sec");
}
void BtkbWindow::recreateWeightsList() noexcept
{
	m_refTreeModelColumnWeight->clear();
	int32_t nIdx = 0;
	for (auto& nWeight : m_aEditColumnsWeight) {
		Gtk::TreeStore::iterator itListWeight = m_refTreeModelColumnWeight->append();
		(*itListWeight)[m_oWeightColumns.m_oIdx] = nIdx;
		(*itListWeight)[m_oWeightColumns.m_oWeight] = nWeight;
		++nIdx;
	}

	nIdx = 0;
	m_refTreeModelRowWeight->clear();
	for (auto& nWeight : m_aEditRowsWeight) {
		Gtk::TreeStore::iterator itListWeight = m_refTreeModelRowWeight->append();
		(*itListWeight)[m_oWeightColumns.m_oIdx] = nIdx;
		(*itListWeight)[m_oWeightColumns.m_oWeight] = nWeight;
		++nIdx;
	}
}
void BtkbWindow::recreateCellsKeyList() noexcept
{
	//
	const int32_t nTotElements = m_nTotColumns * m_nTotRows;
	if (nTotElements > static_cast<int32_t>(m_aEditNames.size())) {
		m_aEditNames.resize(nTotElements, "");
		m_aEditHK.resize(nTotElements, hk::HK_NULL);
	}
	m_refTreeModelCellKey->clear();
	for (int32_t nColumn = 0; nColumn < m_nTotColumns; ++nColumn) {
		for (int32_t nRow = 0; nRow < m_nTotRows; ++nRow) {
			const int32_t nIdx = nColumn + nRow * m_nTotColumns;
			Gtk::TreeStore::iterator itListCellKey = m_refTreeModelCellKey->append();
			(*itListCellKey)[m_oCellKeyColumns.m_oColX] = nColumn;
			(*itListCellKey)[m_oCellKeyColumns.m_oColY] = nRow;
			assert(nIdx < static_cast<int32_t>(m_aEditNames.size()));
			assert(nIdx < static_cast<int32_t>(m_aEditHK.size()));
			(*itListCellKey)[m_oCellKeyColumns.m_oColKeyName] = m_aEditNames[nIdx];
			(*itListCellKey)[m_oCellKeyColumns.m_oColHiddenHardwareKey] = m_aEditHK[nIdx];
		}
	}
}

void BtkbWindow::updateInCellColumn() noexcept
{
	m_p0SpinExitInCellC->set_value(m_nKeysExitButtonInCellColumn);
	m_refAdjustmentExitInCellC->set_upper(m_nTotColumns - 1);
}
void BtkbWindow::updateInCellRow() noexcept
{
	m_p0SpinExitInCellR->set_value(m_nKeysExitButtonInCellRow);
	m_refAdjustmentExitInCellR->set_upper(m_nTotRows - 1);
}
void BtkbWindow::saveStateToConfig() noexcept
{
	auto refConfClient = Gio::Settings::create(s_sSchemaId);

	refConfClient->set_int(s_sConfKeyTotColumns, m_nTotColumns);
	refConfClient->set_int(s_sConfKeyTotRows, m_nTotRows);
	const std::string sColumnWeights = vectorToString(m_aEditColumnsWeight);
	refConfClient->set_string(s_sConfKeyColumnWeights, sColumnWeights);
//std::cout << "   sColumnWeights=" << sColumnWeights << '\n';
	const std::string sRowsWeights = vectorToString(m_aEditRowsWeight);
	refConfClient->reset(s_sConfKeyRowWeights);
	refConfClient->set_string(s_sConfKeyRowWeights, sRowsWeights);
	refConfClient->apply();
	std::vector<int32_t> aHKs;
	for (auto& eHK : m_aEditHK) {
		aHKs.push_back(static_cast<int32_t>(eHK));
	}
	refConfClient->set_string(s_sConfKeyCellKeys, vectorToString(aHKs));
	refConfClient->set_boolean(s_sConfKeyFullscreen, m_bKeysFullscreen);
	refConfClient->set_int(s_sConfKeyExitButtonPercX, m_nKeysExitButtonPercX);
	refConfClient->set_int(s_sConfKeyExitButtonPercY, m_nKeysExitButtonPercY);
	refConfClient->set_boolean(s_sConfKeyExitInCell, m_bKeysExitInCell);
	refConfClient->set_int(s_sConfKeyExitButtonInCellColumn, m_nKeysExitButtonInCellColumn);
	refConfClient->set_int(s_sConfKeyExitButtonInCellRow, m_nKeysExitButtonInCellRow);
	refConfClient->set_boolean(s_sConfKeyLogSentKeys, m_bKeysLogSentKeys);
	//
	if (BtKeyServers::isValidPort(m_oCurServer.m_nL2capPort)) {
		refConfClient->set_string(s_sConfKeySelectedServerAddr, BtKeyServers::getStringFromAddr(m_oCurServer.m_oBtAddr));
		refConfClient->set_int(s_sConfKeySelectedServerPort, m_oCurServer.m_nL2capPort);
	}
	//
	int32_t nWindowW, nWindowH;
	get_size(nWindowW, nWindowH);
	refConfClient->set_int(s_sConfKeyWindowWidth, nWindowW);
	refConfClient->set_int(s_sConfKeyWindowHeight, nWindowH);
	std::cout << "Storing configuration ..." << '\n';
	::sleep(1);
}
void BtkbWindow::loadStateFromConfig() noexcept
{
//std::cout << "BtkbWindow::loadStateFromConfig()" << '\n';
	auto refConfClient = Gio::Settings::create(s_sSchemaId);

	const int32_t nTotColumns = refConfClient->get_int(s_sConfKeyTotColumns);
	if (nTotColumns < 1) {
		m_nTotColumns = 1;
	} else if (nTotColumns > s_nEditMaxGridW) {
		m_nTotColumns = s_nEditMaxGridW;
	} else {
		m_nTotColumns = nTotColumns;
	}
	m_aEditColumnsWeight.reserve(m_nTotColumns);
	//
	const int32_t nTotRows = refConfClient->get_int(s_sConfKeyTotRows);
	if (nTotRows < 1) {
		m_nTotRows = 1;
	} else if (nTotRows > s_nEditMaxGridH) {
		m_nTotRows = s_nEditMaxGridH;
	} else {
		m_nTotRows = nTotRows;
	}
	m_aEditRowsWeight.reserve(m_nTotRows);
	//
	const std::string sColumnWeights = refConfClient->get_string(s_sConfKeyColumnWeights);
//std::cout << "BtkbWindow::loadStateFromConfig()  sColumnWeights=" << sColumnWeights << " m_nTotColumns=" << m_nTotColumns << '\n';
	stringToVector(sColumnWeights, m_nTotColumns, m_nTotColumns, s_nEditMinWeight, s_nEditMaxWeight, s_nEditDefaultWeight
					, m_aEditColumnsWeight);
	//
	const std::string sRowWeights = refConfClient->get_string(s_sConfKeyRowWeights);
//std::cout << "BtkbWindow::loadStateFromConfig()    sRowWeights=" << sRowWeights << " m_nTotRows=" << m_nTotRows << '\n';
	stringToVector(sRowWeights, m_nTotRows, m_nTotRows, s_nEditMinWeight, s_nEditMaxWeight, s_nEditDefaultWeight
					, m_aEditRowsWeight);
	//
	std::vector<int32_t> aCellKeys;
	aCellKeys.reserve(m_nTotColumns * m_nTotRows);
	const std::string sCellKeys = refConfClient->get_string(s_sConfKeyCellKeys);
//std::cout << "BtkbWindow::loadStateFromConfig()    sCellKeys=" << sCellKeys  << '\n';
	stringToVector(sCellKeys, m_nTotColumns * m_nTotRows , m_nTotColumns * m_nTotRows, -1, std::numeric_limits<int32_t>::max(), -1
					, aCellKeys);
	m_aEditHK.resize(m_nTotColumns * m_nTotRows, hk::HK_NULL);
	const auto nCellKeys = static_cast<int32_t>(aCellKeys.size());
	for (int32_t nIdx = 0; nIdx < nCellKeys; ++nIdx) {
		const int32_t nHkCode = aCellKeys[nIdx];
		if (nHkCode < 0) {
			// The values is not set, keep the default (current)
			continue; //------
		}
		auto eHK = static_cast<hk::HARDWARE_KEY>(nHkCode);
		if (m_oInputStrings.getKeyString(eHK).empty()) {
			eHK = hk::HK_NULL;
		}
		m_aEditHK[nIdx] = eHK;
	}
	m_aEditNames.clear();
	m_aEditNames.reserve(m_aEditHK.size());
	for (auto eHK : m_aEditHK) {
		m_aEditNames.push_back(m_oInputStrings.getKeyString(eHK));
	}
	//
	m_bKeysFullscreen = refConfClient->get_boolean(s_sConfKeyFullscreen);
	//
	const int32_t nKeysExitButtonPercX = refConfClient->get_int(s_sConfKeyExitButtonPercX);
	if (nKeysExitButtonPercX < 0) {
		m_nKeysExitButtonPercX = 0;
	} else if (nKeysExitButtonPercX > 100) {
		m_nKeysExitButtonPercX = 100;
	} else {
		m_nKeysExitButtonPercX = nKeysExitButtonPercX;
	}
	//
	const int32_t nKeysExitButtonPercY = refConfClient->get_int(s_sConfKeyExitButtonPercY);
	if (nKeysExitButtonPercY < 0) {
		m_nKeysExitButtonPercY = 0;
	} else if (nKeysExitButtonPercY > 100) {
		m_nKeysExitButtonPercY = 100;
	} else {
		m_nKeysExitButtonPercY = nKeysExitButtonPercY;
	}
	//
	m_bKeysExitInCell = refConfClient->get_boolean(s_sConfKeyExitInCell);
	//
	const int32_t nKeysExitButtonInCellColumn = refConfClient->get_int(s_sConfKeyExitButtonInCellColumn);
	if (nKeysExitButtonInCellColumn < 0) {
		m_nKeysExitButtonInCellColumn = 0;
	} else if (nKeysExitButtonInCellColumn >= m_nTotColumns) {
		m_nKeysExitButtonInCellColumn = m_nTotColumns - 1;
	} else {
		m_nKeysExitButtonInCellColumn = nKeysExitButtonInCellColumn;
	}
	//
	const int32_t nKeysExitButtonInCellRow = refConfClient->get_int(s_sConfKeyExitButtonInCellRow);
	if (nKeysExitButtonInCellRow < 0) {
		m_nKeysExitButtonInCellRow = 0;
	} else if (nKeysExitButtonInCellRow >= m_nTotRows) {
		m_nKeysExitButtonInCellRow = m_nTotRows - 1;
	} else {
		m_nKeysExitButtonInCellRow = nKeysExitButtonInCellRow;
	}
	//
	m_bKeysLogSentKeys = refConfClient->get_boolean(s_sConfKeyLogSentKeys);
	//
	bool bIsValidAddr = false;
	const std::string sSelectedServerAddr = refConfClient->get_string(s_sConfKeySelectedServerAddr);
//std::cout << "BtkbWindow::loadStateFromConfig()    sSelectedServerAddr= " << sSelectedServerAddr<< '\n';
	bIsValidAddr = BtKeyServers::isValidStringAddr(sSelectedServerAddr);
	if (bIsValidAddr) {
		m_oCurServer.m_oBtAddr = BtKeyServers::getAddrFromString(sSelectedServerAddr);
	}
	//
	if (bIsValidAddr) {
		const int32_t nSelectedServerPort = refConfClient->get_int(s_sConfKeySelectedServerPort);
//std::cout << "BtkbWindow::loadStateFromConfig()    nSelectedServerPort= " << nSelectedServerPort << '\n';
		if (BtKeyServers::isValidPort(nSelectedServerPort)) {
			m_oCurServer.m_nL2capPort = nSelectedServerPort;
		}
	}
	//
	int32_t nWindowW = refConfClient->get_int(s_sConfKeyWindowWidth);
	int32_t nWindowH = refConfClient->get_int(s_sConfKeyWindowHeight);
	if (nWindowW <= 0) {
		nWindowW = s_nInitialWindowSizeW;
	} else if (nWindowW <= s_nMinWindowSizeW) {
		nWindowW = s_nMinWindowSizeW;
	}
	if (nWindowH <= 0) {
		nWindowH = s_nInitialWindowSizeH;
	} else if (nWindowH <= s_nMinWindowSizeH) {
		nWindowH = s_nMinWindowSizeH;
	}
	set_default_size(nWindowW, nWindowH);
}

} // namespace stmi

