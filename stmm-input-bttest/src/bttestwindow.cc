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

 #include <gconfmm.h>
#include <gtkmm-3.0/gtkmm/button.h>
#include <gtkmm-2.4/gtkmm/misc.h>

namespace stmi
{

const int32_t BttestWindow::s_nEditDefaultWeight = 10;

const std::string BttestWindow::s_sConfAppDir = "/apps/stmm-input-bttest";
const std::string BttestWindow::s_sConfKeyTotColumns = "tot_columns";
const std::string BttestWindow::s_sConfKeyTotRows = "tot_rows";
const std::string BttestWindow::s_sConfKeyColumnWeights = "column_weights";
const std::string BttestWindow::s_sConfKeyRowWeights = "row_weights";
const std::string BttestWindow::s_sConfKeyCellKeys = "cell_keys";
const std::string BttestWindow::s_sConfKeyFullscreen = "fullscreen";
const std::string BttestWindow::s_sConfKeyExitButtonPercX = "exit_button_perc_x";
const std::string BttestWindow::s_sConfKeyExitButtonPercY = "exit_button_perc_y";
const std::string BttestWindow::s_sConfKeyExitInCell = "exit_button_in_cell";
const std::string BttestWindow::s_sConfKeyExitButtonInCellColumn = "exit_button_in_cell_column";
const std::string BttestWindow::s_sConfKeyExitButtonInCellRow = "exit_button_in_cell_row";
const std::string BttestWindow::s_sConfKeyLogSentKeys = "log_sent_keys";
const std::string BttestWindow::s_sConfKeySelectedServerAddr = "selected_server_addr";
const std::string BttestWindow::s_sConfKeySelectedServerPort = "selected_server_port";

BttestWindow::BttestWindow(const std::string& sTitle, BtKeyClient& oBtKeyClient, BtKeyServers& oBtKeyServers)
: m_oBtKeyClient(oBtKeyClient)
, m_oBtKeyServers(oBtKeyServers)
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

	loadStateFromConfig();

	std::string sInfoText = std::string("") + 
			"stmm-input-bttest\n"
			"=================\n"
			"\n"
			"Version: " + Config::getVersionString() + "\n"
			"\n"
			"Copyright © 2017   Stefano Marsili, <stemars@gmx.ch>\n"
			"\n"
			"This is a test client application for the btkeys bluetooth\n"
			"protocol to connect to the server started by the\n"
			"libstmm-input-gtk-bt library.\n"
			"\n"
			"To detect the present devices press the 'Refresh' button.\n"
			"Per default detection takes up to 15 seconds.\n"
			"After choosing a server by clicking on it in the created list,\n"
			"press the 'Select' and then the 'Connect' button.\n"
			"If the connection succeeds start the keyboard in the keys tab.\n"
			"\n"
			"In fullscreen mode the keyboard can be closed by pressing\n"
			"the green (if connected) or red (if disconnected) square.\n"
			;
	//
	set_title(sTitle);
	set_default_size(s_nInitialWindowSizeW, s_nInitialWindowSizeH);
	set_resizable(true);

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

	Gtk::Box* m_p0VBoxMain = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_VERTICAL));
	Gtk::Window::add(*m_p0VBoxMain);

	m_p0NotebookChoices = Gtk::manage(new Gtk::Notebook());
	m_p0VBoxMain->pack_start(*m_p0NotebookChoices, true, true);
		m_p0NotebookChoices->signal_switch_page().connect(
						sigc::mem_fun(*this, &BttestWindow::onNotebookSwitchPage) );

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
			m_p0HBoxServersCmds->pack_start(*m_p0LabelProgress, false, true);

		Gtk::Box* m_p0VBoxConnection = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_VERTICAL));
		m_p0VBoxServers->pack_start(*m_p0VBoxConnection, false, false);
			m_p0VBoxConnection->set_spacing(5);

			Gtk::Box* m_p0HBoxStagedServer = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_HORIZONTAL));
			m_p0VBoxConnection->pack_start(*m_p0HBoxStagedServer, false, false);
				m_p0HBoxStagedServer->set_spacing(5);

				Gtk::Button* m_p0ButtonInputServer = Gtk::manage(new Gtk::Button("Modify"));
				m_p0HBoxStagedServer->pack_start(*m_p0ButtonInputServer, false, true);
					m_p0ButtonInputServer->signal_clicked().connect(
									sigc::mem_fun(*this, &BttestWindow::onInputStagedServer) );
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
									sigc::mem_fun(*this, &BttestWindow::onButtonConnect) );
				m_p0ButtonDisconnect = Gtk::manage(new Gtk::Button("Disconnect"));
				m_p0HBoxConnectionCmds->pack_start(*m_p0ButtonDisconnect, true, true);
					m_p0ButtonDisconnect->signal_clicked().connect(
									sigc::mem_fun(*this, &BttestWindow::onButtonDisconnect) );
				m_p0ButtonRemove = Gtk::manage(new Gtk::Button("Remove"));
				m_p0HBoxConnectionCmds->pack_start(*m_p0ButtonRemove, true, true);
					m_p0ButtonRemove->signal_clicked().connect(
									sigc::mem_fun(*this, &BttestWindow::onButtonRemove) );

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
									sigc::mem_fun(*this, &BttestWindow::onSpinNrColumnsChanged) );
				m_refTreeModelColumnWeight = Gtk::TreeStore::create(m_oWeightColumns);
				m_p0TreeViewColumnWeights = Gtk::manage(new Gtk::TreeView(m_refTreeModelColumnWeight));
				m_p0VBoxColumns->pack_start(*m_p0TreeViewColumnWeights, false, false);
					m_p0TreeViewColumnWeights->append_column("Col", m_oWeightColumns.m_oIdx);
					m_p0TreeViewColumnWeights->append_column("Weight", m_oWeightColumns.m_oWeight);
					oWeightTreeViewAligner(m_p0TreeViewColumnWeights);
					m_p0TreeViewColumnWeights->signal_row_activated().connect(
									sigc::mem_fun(*this, &BttestWindow::onColumnWeightActivated) );
						//on_my_row_activated(const TreeModel::Path& path, TreeViewColumn* column)
			Gtk::Box* m_p0VBoxRows = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_VERTICAL));
			m_p0HBoxSize->pack_start(*m_p0VBoxRows, false, false);
				Gtk::Label* m_p0LabelRows = Gtk::manage(new Gtk::Label("Height:"));
					m_p0LabelRows->set_alignment(Gtk::Align::ALIGN_START, m_p0LabelRows->property_valign());
				m_p0VBoxRows->pack_start(*m_p0LabelRows, false, false);
				m_p0SpinRows = Gtk::manage(new Gtk::SpinButton(m_refAdjustmentNrRows));
				m_p0VBoxRows->pack_start(*m_p0SpinRows, false, false);
					m_p0SpinRows->signal_value_changed().connect(
									sigc::mem_fun(*this, &BttestWindow::onSpinNrRowsChanged) );
				m_refTreeModelRowWeight = Gtk::TreeStore::create(m_oWeightColumns);
				m_p0TreeViewRowWeights = Gtk::manage(new Gtk::TreeView(m_refTreeModelRowWeight));
				m_p0VBoxRows->pack_start(*m_p0TreeViewRowWeights, false, false);
					m_p0TreeViewRowWeights->append_column("Row", m_oWeightColumns.m_oIdx);
					m_p0TreeViewRowWeights->append_column("Weight", m_oWeightColumns.m_oWeight);
					oWeightTreeViewAligner(m_p0TreeViewRowWeights);
					m_p0TreeViewRowWeights->signal_row_activated().connect(
									sigc::mem_fun(*this, &BttestWindow::onRowWeightActivated) );
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
								sigc::mem_fun(*this, &BttestWindow::onCellNameActivated) );

	Gtk::Label* m_p0TabLabelKeys = Gtk::manage(new Gtk::Label("Keys"));
	Gtk::Box* m_p0VBoxKeys = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_VERTICAL));
	m_aPageIndex[s_nTabKeys] = m_p0NotebookChoices->append_page(*m_p0VBoxKeys, *m_p0TabLabelKeys);
			m_p0VBoxKeys->set_spacing(10);
		m_p0CheckButtonFullscreen = Gtk::manage(new Gtk::CheckButton("Fullscreen"));
		m_p0VBoxKeys->pack_start(*m_p0CheckButtonFullscreen, false, false);
			m_p0CheckButtonFullscreen->set_active(m_bKeysFullscreen);
			m_p0CheckButtonFullscreen->signal_clicked().connect(
							sigc::mem_fun(*this, &BttestWindow::onKeysFullscreenChanged));
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
								sigc::mem_fun(*this, &BttestWindow::onSpinExitPositionXChanged) );
				Gtk::Label* m_p0LabelExitPositionY = Gtk::manage(new Gtk::Label("  Y: "));
				m_p0HBoxExitPositionXY->pack_start(*m_p0LabelExitPositionY, false, false);
				m_p0SpinExitPositionY = Gtk::manage(new Gtk::SpinButton(m_refAdjustmentExitPositionY));
				m_p0HBoxExitPositionXY->pack_start(*m_p0SpinExitPositionY, false, false);
					m_p0SpinExitPositionY->signal_value_changed().connect(
								sigc::mem_fun(*this, &BttestWindow::onSpinExitPositionYChanged) );
		m_p0CheckButtonInCell = Gtk::manage(new Gtk::CheckButton("Within cell"));
		m_p0VBoxKeys->pack_start(*m_p0CheckButtonInCell, false, false);
			m_p0CheckButtonInCell->set_active(m_bKeysExitInCell);
			m_p0CheckButtonInCell->signal_clicked().connect(
							sigc::mem_fun(*this, &BttestWindow::onKeysExitInCellChanged));
		Gtk::Box* m_p0HBoxInCellCR = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_HORIZONTAL));
		m_p0VBoxKeys->pack_start(*m_p0HBoxInCellCR, false, false);
			Gtk::Label* m_p0LabelInCellC = Gtk::manage(new Gtk::Label(" C: "));
			m_p0HBoxInCellCR->pack_start(*m_p0LabelInCellC, false, false);
			m_p0SpinExitInCellC = Gtk::manage(new Gtk::SpinButton(m_refAdjustmentExitInCellC));
			m_p0HBoxInCellCR->pack_start(*m_p0SpinExitInCellC, false, false);
				m_p0SpinExitInCellC->signal_value_changed().connect(
							sigc::mem_fun(*this, &BttestWindow::onSpinExitInCellColumnChanged) );
			Gtk::Label* m_p0LabelInCellR = Gtk::manage(new Gtk::Label("  R: "));
			m_p0HBoxInCellCR->pack_start(*m_p0LabelInCellR, false, false);
			m_p0SpinExitInCellR = Gtk::manage(new Gtk::SpinButton(m_refAdjustmentExitInCellR));
			m_p0HBoxInCellCR->pack_start(*m_p0SpinExitInCellR, false, false);
				m_p0SpinExitInCellR->signal_value_changed().connect(
							sigc::mem_fun(*this, &BttestWindow::onSpinExitInCellRowChanged) );
		m_p0CheckButtonLogSentKeys = Gtk::manage(new Gtk::CheckButton("Log sent keys"));
		m_p0VBoxKeys->pack_start(*m_p0CheckButtonLogSentKeys, false, false);
			m_p0CheckButtonLogSentKeys->set_active(m_bKeysLogSentKeys);
			m_p0CheckButtonLogSentKeys->signal_clicked().connect(
							sigc::mem_fun(*this, &BttestWindow::onKeysLogSentKeysChanged));
		Gtk::Button* m_p0ButtonKeyboard = Gtk::manage(new Gtk::Button("Keyboard"));
		m_p0VBoxKeys->pack_start(*m_p0ButtonKeyboard, true, true);
			m_p0ButtonKeyboard->signal_clicked().connect(
							sigc::mem_fun(*this, &BttestWindow::onButtonKeyboard) );

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

	Gtk::Label* m_p0TabLabelInfo = Gtk::manage(new Gtk::Label("Info"));
	Gtk::ScrolledWindow* m_p0ScrolledInfo = Gtk::manage(new Gtk::ScrolledWindow());
	m_aPageIndex[s_nTabInfo] = m_p0NotebookChoices->append_page(*m_p0ScrolledInfo, *m_p0TabLabelInfo);
		m_p0ScrolledInfo->set_border_width(5);
		m_p0ScrolledInfo->set_policy(Gtk::POLICY_NEVER, Gtk::POLICY_AUTOMATIC);

		Gtk::Label* m_p0LabelInfoText = Gtk::manage(new Gtk::Label(sInfoText));
		m_p0ScrolledInfo->add(*m_p0LabelInfoText);
			m_p0LabelInfoText->set_line_wrap_mode(Pango::WRAP_WORD_CHAR);


	show_all_children();

	m_oBtKeyClient.m_oStateChangedSignal.connect( sigc::mem_fun(this, &BttestWindow::onClientStateChanged) );
	m_oBtKeyClient.m_oErrorSignal.connect( sigc::mem_fun(this, &BttestWindow::onClientError) );

	m_oBtKeyServers.m_oServersChangedSignal.connect( sigc::mem_fun(this, &BttestWindow::onServersChanged) );
	m_oBtKeyServers.m_oRefreshProgressSignal.connect( sigc::mem_fun(this, &BttestWindow::onServersProgress) );

	//assert(BtKeyServers::isValid(m_oCurServer));
	setStagedServerAndPort();
//	m_p0LabelServer->set_text("Server: -----------------  Port: -----");

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
	if (m_refKeyboardDialog) {
		m_refKeyboardDialog->connectionStateChanged();
	}
}
void BttestWindow::onNotebookSwitchPage(Gtk::Widget*, guint nPageNum)
{
	if (nPageNum >= s_nTotPages) {
		return;
	}
	//if (nPageNum == m_aPageIndex[s_nTabEdit]) {
	//}
}
void BttestWindow::onServerSelectionChanged()
{
//std::cout << "BttestWindow::onServerSelectionChanged()" << '\n';
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
		setStagedServerAndPort();
//		m_p0LabelServer->set_text("Server: " + BtKeyServers::getStringFromAddr(m_oCurServer.m_oBtAddr)
//								+ "  Port: " + std::to_string(m_oCurServer.m_nL2capPort));
		//m_p0NotebookChoices->set_current_page(s_nTabKeys);
	}
}
void BttestWindow::setStagedServerAndPort()
{
	if (BtKeyServers::isValidPort(m_oCurServer.m_nL2capPort)) {
		m_p0LabelServer->set_text("Server: " + BtKeyServers::getStringFromAddr(m_oCurServer.m_oBtAddr)
								+ "  Port: " + std::to_string(m_oCurServer.m_nL2capPort));
	} else {
		m_p0LabelServer->set_text("Server: -----------------  Port: -----");
	}
}
void BttestWindow::onRefreshServers()
{
//std::cout << "BttestWindow::onRefreshServers()" << '\n';
	assert(! m_oBtKeyServers.isRefreshing());
	m_oBtKeyServers.startRefreshServers();
	setSensitivityForState();
}
void BttestWindow::onInputStagedServer()
{
	std::string sAddr = (BtKeyServers::isValidPort(m_oCurServer.m_nL2capPort) && BtKeyServers::isValidAddr(m_oCurServer.m_oBtAddr)
							? BtKeyServers::getStringFromAddr(m_oCurServer.m_oBtAddr)
							: "");
	if (!m_refAddrDialog) {
		m_refAddrDialog = Glib::RefPtr<AddrDialog>(new AddrDialog());
		m_refAddrDialog->set_transient_for(*this);
	}
//std::cout << "-------> sAddr=" << sAddr << '\n';
	const int nRet = m_refAddrDialog->run(sAddr);
	m_refAddrDialog->hide();
	if (nRet == AddrDialog::s_nRetOk) {
		sAddr = m_refAddrDialog->getAddr();
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
void BttestWindow::onSpinNrColumnsChanged()
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
void BttestWindow::onSpinNrRowsChanged()
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
void BttestWindow::onColumnWeightActivated(const Gtk::TreeModel::Path& oPath, Gtk::TreeViewColumn* /*p0Column*/)
{
//std::cout << " onColumnWeightActivated()" << '\n';
	Gtk::TreeStore::iterator itListWeight = m_refTreeModelColumnWeight->get_iter(oPath);
	int32_t nWeight = (*itListWeight)[m_oWeightColumns.m_oWeight];
	const int32_t nCol = (*itListWeight)[m_oWeightColumns.m_oIdx];
	assert((nCol >= 0) && (nCol < static_cast<int32_t>(m_aEditColumnsWeight.size())));
	assert(m_aEditColumnsWeight[nCol] == nWeight);
	if (!m_refWeightDialog) {
		m_refWeightDialog = Glib::RefPtr<WeightDialog>(new WeightDialog(s_nEditMinWeight, s_nEditMaxWeight));
		m_refWeightDialog->set_transient_for(*this);
	}
	const int nRet = m_refWeightDialog->run(nWeight, true, nCol);
	m_refWeightDialog->hide();
	if (nRet == WeightDialog::s_nRetOk) {
		nWeight = m_refWeightDialog->getWeight();
		m_aEditColumnsWeight[nCol] = nWeight;
		(*itListWeight)[m_oWeightColumns.m_oWeight] = nWeight;
	}
}
void BttestWindow::onRowWeightActivated(const Gtk::TreeModel::Path& oPath, Gtk::TreeViewColumn* /*p0Column*/)
{
//std::cout << " onRowWeightActivated()" << '\n';
	Gtk::TreeStore::iterator itListWeight = m_refTreeModelRowWeight->get_iter(oPath);
	int32_t nWeight = (*itListWeight)[m_oWeightColumns.m_oWeight];
	const int32_t nRow = (*itListWeight)[m_oWeightColumns.m_oIdx];
	assert((nRow >= 0) && (nRow < static_cast<int32_t>(m_aEditRowsWeight.size())));
	assert(m_aEditRowsWeight[nRow] == nWeight);
	if (!m_refWeightDialog) {
		m_refWeightDialog = Glib::RefPtr<WeightDialog>(new WeightDialog(s_nEditMinWeight, s_nEditMaxWeight));
		m_refWeightDialog->set_transient_for(*this);
	}
	const int nRet = m_refWeightDialog->run(nWeight, true, nRow);
	m_refWeightDialog->hide();
	if (nRet == WeightDialog::s_nRetOk) {
		nWeight = m_refWeightDialog->getWeight();
		m_aEditRowsWeight[nRow] = nWeight;
		(*itListWeight)[m_oWeightColumns.m_oWeight] = nWeight;
	}
}
void BttestWindow::onCellNameActivated(const Gtk::TreeModel::Path& oPath, Gtk::TreeViewColumn* /*p0Column*/)
{
//std::cout << " onCellNameActivated()" << '\n';
	Gtk::TreeStore::iterator itListKeyName = m_refTreeModelCellKey->get_iter(oPath);
	const int32_t nColumn = (*itListKeyName)[m_oCellKeyColumns.m_oColX];
	const int32_t nRow = (*itListKeyName)[m_oCellKeyColumns.m_oColY];
	const int32_t nIdx = nColumn + nRow * m_aEditColumnsWeight.size();
	const int32_t nHK = (*itListKeyName)[m_oCellKeyColumns.m_oColHiddenHardwareKey];
	hk::HARDWARE_KEY eKey = static_cast<hk::HARDWARE_KEY>(nHK);
	assert((nColumn >= 0) && (nColumn < static_cast<int32_t>(m_aEditColumnsWeight.size())));
	assert((nRow >= 0) && (nRow < static_cast<int32_t>(m_aEditRowsWeight.size())));
	assert(nIdx < static_cast<int32_t>(m_aEditHK.size()));
	assert(m_aEditHK[nIdx] == eKey);
	if (!m_refKeyDialog) {
		m_refKeyDialog = Glib::RefPtr<KeyDialog>(new KeyDialog(m_oInputStrings));
		m_refKeyDialog->set_transient_for(*this);
	}
	const int nRet = m_refKeyDialog->run(eKey, nColumn, nRow);
	m_refKeyDialog->hide();
	if (nRet == WeightDialog::s_nRetOk) {
		eKey = m_refKeyDialog->getHardwareKey();
		m_aEditHK[nIdx] = eKey;
		m_aEditNames[nIdx] = m_oInputStrings.getKeyString(eKey);
		(*itListKeyName)[m_oCellKeyColumns.m_oColHiddenHardwareKey] = eKey;
		(*itListKeyName)[m_oCellKeyColumns.m_oColKeyName] = m_aEditNames[nIdx];
	}
}
void BttestWindow::onKeysFullscreenChanged()
{
	m_bKeysFullscreen = m_p0CheckButtonFullscreen->get_active();
//std::cout << "BttestWindow::onKeysFullscreenChanged()  m_bKeysFullscreen=" << m_bKeysFullscreen << '\n';
}
void BttestWindow::onSpinExitPositionXChanged()
{
	m_nKeysExitButtonPercX = m_p0SpinExitPositionX->get_value_as_int();
}
void BttestWindow::onSpinExitPositionYChanged()
{
	m_nKeysExitButtonPercY = m_p0SpinExitPositionY->get_value_as_int();
}
void BttestWindow::onKeysExitInCellChanged()
{
	m_bKeysExitInCell = m_p0CheckButtonInCell->get_active();
	m_p0SpinExitInCellC->set_sensitive(m_bKeysExitInCell);
	m_p0SpinExitInCellR->set_sensitive(m_bKeysExitInCell);
}
void BttestWindow::onSpinExitInCellColumnChanged()
{
	m_nKeysExitButtonInCellColumn = m_p0SpinExitInCellC->get_value_as_int();
}
void BttestWindow::onSpinExitInCellRowChanged()
{
	m_nKeysExitButtonInCellRow = m_p0SpinExitInCellR->get_value_as_int();
}
void BttestWindow::onKeysLogSentKeysChanged()
{
	m_bKeysLogSentKeys = m_p0CheckButtonLogSentKeys->get_active();
}
void BttestWindow::onButtonKeyboard()
{
//std::cout << "BttestWindow::onButtonKeyboard m_bKeysFullscreen=" << m_bKeysFullscreen << '\n';
	if (!m_refKeyboardDialog) {
		m_refKeyboardDialog = Glib::RefPtr<KeyboardDialog>(new KeyboardDialog(*this, m_bKeysFullscreen));
		m_refKeyboardDialog->set_transient_for(*this);
		//Glib::signal_timeout().connect_once(sigc::mem_fun(*this, &BttestWindow::onButtonKeyboard), 0);
		//return; //--------------------------------------------------------------
	}
	m_refKeyboardDialog->run(m_bKeysFullscreen);
	m_refKeyboardDialog->hide();
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
	finalizeKeys();
	printStringToLog("Sending removal request ...");
	m_oBtKeyClient.sendRemoveToServer();
	setSensitivityForState();
}
void BttestWindow::finalizeKeys()
{
	for (auto oPK : m_aPressedKeys) {
		releaseKey(oPK.m_eKey, oPK.m_eType, oPK.m_p0Finger, true);
	}
}
bool BttestWindow::isCellKeyPressed(int32_t nColumn, int32_t nRow) const
{
	assert((nColumn < m_nTotColumns) && (nRow < m_nTotRows));
	const int32_t nColRow = nColumn + nRow * m_nTotColumns;
	const hk::HARDWARE_KEY eKey = m_aEditHK[nColRow];
	return isKeyPressed(eKey);
}
bool BttestWindow::isKeyPressed(hk::HARDWARE_KEY eKey) const
{
	const auto itFind = std::find_if(m_aPressedKeys.begin(), m_aPressedKeys.end(), [&](const PressedKey& oPK) {
		return oPK.m_eKey == eKey;
	});
	const bool bFound = (itFind != m_aPressedKeys.end());
	return bFound;
}
bool BttestWindow::pressKeyboardKey(hk::HARDWARE_KEY eKey)
{
	assert(eKey != hk::HK_NULL);
	return pressKey(eKey, PRESS_TYPE_KEYBOARD, nullptr);
}
bool BttestWindow::pressMouseKey(hk::HARDWARE_KEY eKey)
{
	assert(eKey != hk::HK_NULL);
	return pressKey(eKey, PRESS_TYPE_MOUSE, nullptr);
}
bool BttestWindow::pressFingerKey(hk::HARDWARE_KEY eKey, const GdkEventSequence* p0Finger)
{
	assert(eKey != hk::HK_NULL);
	return pressKey(eKey, PRESS_TYPE_TOUCH, p0Finger);
}
bool BttestWindow::releaseKeyboardKey(hk::HARDWARE_KEY eKey)
{
	assert(eKey != hk::HK_NULL);
	return releaseKey(eKey, PRESS_TYPE_KEYBOARD, nullptr, false);
}
bool BttestWindow::releaseMouseKey()
{
	return releaseKey(hk::HK_NULL, PRESS_TYPE_MOUSE, nullptr, false);
}
bool BttestWindow::releaseFingerKey(const GdkEventSequence* p0Finger, bool bCancel)
{
	return releaseKey(hk::HK_NULL, PRESS_TYPE_TOUCH, p0Finger, bCancel);
}
bool BttestWindow::pressKey(hk::HARDWARE_KEY eKey, PRESS_TYPE eType, const GdkEventSequence* p0Finger)
{
	bool bKeyAlreadyPressed = false;
	auto itFindSame = m_aPressedKeys.end();
	for (auto itCur = m_aPressedKeys.begin(); itCur != m_aPressedKeys.end(); ++itCur) {
		const bool bSameKey = (itCur->m_eKey == eKey);
		if (bSameKey) {
			bKeyAlreadyPressed = true;
		}
		if (bSameKey && (itCur->m_eType == eType) && (itCur->m_p0Finger == p0Finger)) {
			if (itFindSame == m_aPressedKeys.end()) {
				itFindSame = itCur;
			}
		}
	}
	if (itFindSame != m_aPressedKeys.end()) {
		// already pressed
		return false; //--------------------------------------------------------
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
bool BttestWindow::releaseKey(hk::HARDWARE_KEY eKey, PRESS_TYPE eType, const GdkEventSequence* p0Finger, bool bCancel)
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
		printStringToLog(std::string("Refreshing: ") + sError);
		m_p0LabelProgress->set_text("Refreshing: Error");
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
void BttestWindow::recreateWeightsList()
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
void BttestWindow::recreateCellsKeyList()
{
	m_refTreeModelCellKey->clear();
	for (int32_t nColumn = 0; nColumn < m_nTotColumns; ++nColumn) {
		for (int32_t nRow = 0; nRow < m_nTotRows; ++nRow) {
			const int32_t nIdx = nColumn + nRow * m_nTotColumns;
			Gtk::TreeStore::iterator itListCellKey = m_refTreeModelCellKey->append();
			(*itListCellKey)[m_oCellKeyColumns.m_oColX] = nColumn;
			(*itListCellKey)[m_oCellKeyColumns.m_oColY] = nRow;
			assert(nIdx < static_cast<int32_t>(m_aEditNames.size()));
			assert(nIdx < static_cast<int32_t>(m_aEditHK.size()));
//std::cout << " nIdx=" << nIdx << "  key:" << static_cast<int32_t>(m_aEditHK[nIdx]) << "  name:" << m_aEditNames[nIdx] << '\n';
			(*itListCellKey)[m_oCellKeyColumns.m_oColKeyName] = m_aEditNames[nIdx];
			(*itListCellKey)[m_oCellKeyColumns.m_oColHiddenHardwareKey] = m_aEditHK[nIdx];
		}
	}
}

void BttestWindow::updateInCellColumn()
{
	m_p0SpinExitInCellC->set_value(m_nKeysExitButtonInCellColumn);
	m_refAdjustmentExitInCellC->set_upper(m_nTotColumns - 1);
}
void BttestWindow::updateInCellRow()
{
	m_p0SpinExitInCellR->set_value(m_nKeysExitButtonInCellRow);
	m_refAdjustmentExitInCellR->set_upper(m_nTotRows - 1);
}

void BttestWindow::saveStateToConfig()
{
	auto refConfClient = Gnome::Conf::Client::get_default_client();
	refConfClient->add_dir(s_sConfAppDir);
	refConfClient->set(s_sConfAppDir + "/" + s_sConfKeyTotColumns, m_nTotColumns);
	refConfClient->set(s_sConfAppDir + "/" + s_sConfKeyTotRows, m_nTotRows);
	refConfClient->set_int_list(s_sConfAppDir + "/" + s_sConfKeyColumnWeights, m_aEditColumnsWeight);
	refConfClient->set_int_list(s_sConfAppDir + "/" + s_sConfKeyRowWeights, m_aEditRowsWeight);
	std::vector<int32_t> aHKs;
	for (auto& eHK : m_aEditHK) {
		aHKs.push_back(static_cast<int32_t>(eHK));
	}
	refConfClient->set_int_list(s_sConfAppDir + "/" + s_sConfKeyCellKeys, aHKs);
	refConfClient->set(s_sConfAppDir + "/" + s_sConfKeyFullscreen, m_bKeysFullscreen);
	refConfClient->set(s_sConfAppDir + "/" + s_sConfKeyExitButtonPercX, m_nKeysExitButtonPercX);
	refConfClient->set(s_sConfAppDir + "/" + s_sConfKeyExitButtonPercY, m_nKeysExitButtonPercY);
	refConfClient->set(s_sConfAppDir + "/" + s_sConfKeyExitInCell, m_bKeysExitInCell);
	refConfClient->set(s_sConfAppDir + "/" + s_sConfKeyExitButtonInCellColumn, m_nKeysExitButtonInCellColumn);
	refConfClient->set(s_sConfAppDir + "/" + s_sConfKeyExitButtonInCellRow, m_nKeysExitButtonInCellRow);
	refConfClient->set(s_sConfAppDir + "/" + s_sConfKeyLogSentKeys, m_bKeysLogSentKeys);
	//
	if (BtKeyServers::isValidPort(m_oCurServer.m_nL2capPort)) {
		refConfClient->set(s_sConfAppDir + "/" + s_sConfKeySelectedServerAddr, BtKeyServers::getStringFromAddr(m_oCurServer.m_oBtAddr));
		refConfClient->set(s_sConfAppDir + "/" + s_sConfKeySelectedServerPort, m_oCurServer.m_nL2capPort);
	}

}
void BttestWindow::loadStateFromConfig()
{
	auto refConfClient = Gnome::Conf::Client::get_default_client();
	refConfClient->add_dir(s_sConfAppDir);

	auto oValue = refConfClient->get(s_sConfAppDir + "/" + s_sConfKeyTotColumns);
	auto eType = oValue.get_type();
	if (eType == Gnome::Conf::VALUE_INT) {
		const int32_t nTotColumns = oValue.get_int();
		if ((nTotColumns >= 1) && (nTotColumns <= s_nEditMaxGridW)) {
			m_nTotColumns = nTotColumns;
			m_aEditColumnsWeight.resize(nTotColumns, s_nEditDefaultWeight);
		}
	}
	oValue = refConfClient->get(s_sConfAppDir + "/" + s_sConfKeyTotRows);
	eType = oValue.get_type();
	if (eType == Gnome::Conf::VALUE_INT) {
		const int32_t nTotRows = oValue.get_int();
		if ((nTotRows >= 1) && (nTotRows <= s_nEditMaxGridH)) {
			m_nTotRows = nTotRows;
			m_aEditRowsWeight.resize(nTotRows, s_nEditDefaultWeight);
		}
	}
	oValue = refConfClient->get(s_sConfAppDir + "/" + s_sConfKeyColumnWeights);
	eType = oValue.get_type();
	if ((eType == Gnome::Conf::VALUE_LIST) && (oValue.get_list_type() == Gnome::Conf::VALUE_INT)) {
		const std::vector<int32_t> aColumnWeights = oValue.get_int_list();
		const auto nTotWeights = static_cast<int32_t>(aColumnWeights.size());
		for (int32_t nIdx = 0; nIdx < nTotWeights; ++nIdx) {
			int32_t nWeight = aColumnWeights[nIdx];
			if (nWeight < s_nEditMinWeight) {
				nWeight = s_nEditMinWeight;
			} else if (nWeight > s_nEditMaxWeight) {
				nWeight = s_nEditMaxWeight;
			}
			m_aEditColumnsWeight[nIdx] = nWeight;
		}
	}
	oValue = refConfClient->get(s_sConfAppDir + "/" + s_sConfKeyRowWeights);
	eType = oValue.get_type();
	if ((eType == Gnome::Conf::VALUE_LIST) && (oValue.get_list_type() == Gnome::Conf::VALUE_INT)) {
		const std::vector<int32_t> aRowWeights = oValue.get_int_list();
		const auto nTotWeights = static_cast<int32_t>(aRowWeights.size());
		for (int32_t nIdx = 0; nIdx < nTotWeights; ++nIdx) {
			int32_t nWeight = aRowWeights[nIdx];
			if (nWeight < s_nEditMinWeight) {
				nWeight = s_nEditMinWeight;
			} else if (nWeight > s_nEditMaxWeight) {
				nWeight = s_nEditMaxWeight;
			}
			m_aEditRowsWeight[nIdx] = nWeight;
		}
	}
	oValue = refConfClient->get(s_sConfAppDir + "/" + s_sConfKeyCellKeys);
	eType = oValue.get_type();
	if ((eType == Gnome::Conf::VALUE_LIST) && (oValue.get_list_type() == Gnome::Conf::VALUE_INT)) {
		const std::vector<int32_t> aCellKeys = oValue.get_int_list();
		const auto nCellKeys = static_cast<int32_t>(aCellKeys.size());
		for (int32_t nIdx = 0; nIdx < nCellKeys; ++nIdx) {
			auto eHK = static_cast<hk::HARDWARE_KEY>(aCellKeys[nIdx]);
			if (m_oInputStrings.getKeyString(eHK).empty()) {
				eHK = hk::HK_NULL;
			}
			m_aEditHK[nIdx] = eHK;
		}
	}
	m_aEditNames.clear();
	for (auto eHK : m_aEditHK) {
		m_aEditNames.push_back(m_oInputStrings.getKeyString(eHK));
	}
	//
	oValue = refConfClient->get(s_sConfAppDir + "/" + s_sConfKeyFullscreen);
	eType = oValue.get_type();
	if (eType == Gnome::Conf::VALUE_BOOL) {
		m_bKeysFullscreen = oValue.get_bool();
	}
	oValue = refConfClient->get(s_sConfAppDir + "/" + s_sConfKeyExitButtonPercX);
	eType = oValue.get_type();
	if (eType == Gnome::Conf::VALUE_INT) {
		const int32_t nExitButtonPercX = oValue.get_int();
		if ((nExitButtonPercX >= 0) && (nExitButtonPercX <= 100)) {
			m_nKeysExitButtonPercX = nExitButtonPercX;
		}
	}
	oValue = refConfClient->get(s_sConfAppDir + "/" + s_sConfKeyExitButtonPercY);
	eType = oValue.get_type();
	if (eType == Gnome::Conf::VALUE_INT) {
		const int32_t nExitButtonPercY = oValue.get_int();
		if ((nExitButtonPercY >= 0) && (nExitButtonPercY <= 100)) {
			m_nKeysExitButtonPercY = nExitButtonPercY;
		}
	}
	oValue = refConfClient->get(s_sConfAppDir + "/" + s_sConfKeyExitInCell);
	eType = oValue.get_type();
	if (eType == Gnome::Conf::VALUE_BOOL) {
		m_bKeysExitInCell = oValue.get_bool();
	}
	oValue = refConfClient->get(s_sConfAppDir + "/" + s_sConfKeyExitButtonInCellColumn);
	eType = oValue.get_type();
	if (eType == Gnome::Conf::VALUE_INT) {
		const int32_t nExitButtonInCellColumn = oValue.get_int();
		if ((nExitButtonInCellColumn >= 0) && (nExitButtonInCellColumn < m_nTotColumns)) {
			m_nKeysExitButtonInCellColumn = nExitButtonInCellColumn;
		}
	}
	oValue = refConfClient->get(s_sConfAppDir + "/" + s_sConfKeyExitButtonInCellRow);
	eType = oValue.get_type();
	if (eType == Gnome::Conf::VALUE_INT) {
		const int32_t nExitButtonInCellRow = oValue.get_int();
		if ((nExitButtonInCellRow >= 0) && (nExitButtonInCellRow < m_nTotRows)) {
			m_nKeysExitButtonInCellRow = nExitButtonInCellRow;
		}
	}
	oValue = refConfClient->get(s_sConfAppDir + "/" + s_sConfKeyLogSentKeys);
	eType = oValue.get_type();
	if (eType == Gnome::Conf::VALUE_BOOL) {
		m_bKeysLogSentKeys = oValue.get_bool();
	}
	//
	bool bIsValidAddr = false;
	oValue = refConfClient->get(s_sConfAppDir + "/" + s_sConfKeySelectedServerAddr);
	eType = oValue.get_type();
	if (eType == Gnome::Conf::VALUE_STRING) {
		const std::string sSelectedAddr = oValue.get_string();
		bIsValidAddr = BtKeyServers::isValidStringAddr(sSelectedAddr);
		if (bIsValidAddr) {
			m_oCurServer.m_oBtAddr = BtKeyServers::getAddrFromString(sSelectedAddr);
		}
	}
//std::cout << "loading  bIsValidAddr " << bIsValidAddr << '\n';
	if (bIsValidAddr) {
		oValue = refConfClient->get(s_sConfAppDir + "/" + s_sConfKeySelectedServerPort);
		eType = oValue.get_type();
		if (eType == Gnome::Conf::VALUE_INT) {
			const int32_t nSelectedPort = oValue.get_int();
			if (BtKeyServers::isValidPort(nSelectedPort)) {
				m_oCurServer.m_nL2capPort = nSelectedPort;
			}
		}
	}
}

} // namespace stmi

