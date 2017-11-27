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
 * File:   main.cc
 */

#include "btkeyclient.h"
#include "btkeyservers.h"
#include "config.h"
#include "bttestwindow.h"

#include <gtkmm.h>
#include <gconfmm.h>

#include <cassert>
#include <iostream>
#include <cmath>
#include <cstdlib>

#include <bluetooth/bluetooth.h>


namespace stmi
{

void printVersion()
{
	std::cout << Config::getVersionString() << '\n';
}
void printUsage()
{
	std::cout << "Usage: stmm-input-bttest [OPTION]" << '\n';
	std::cout << "Test keyboard client that connects to a server over bluetooth" << '\n';
	std::cout << "with the btkeys protocol." << '\n';
	std::cout << "Option:" << '\n';
	std::cout << "  -h --help              Prints this message." << '\n';
	std::cout << "  -v --version           Prints version." << '\n';
	std::cout << "  -e --extra-server XX:XX:XX:XX:XX:XX    with X=[0-9|A-F]" << '\n';
	std::cout << "                         Extra server bluetooth address." << '\n';
	std::cout << "  -p --port P            L2CAP port: P " << '\n';
	std::cout << "                         (default " << BtKeyServers::s_nDefaultL2capPort << ")." << '\n';
	std::cout << "  -c --connect N         Connect timeout: N milliseconds" << '\n';
	std::cout << "                         (default: " << BtKeyClient::s_nDefaultTimeoutConnect << ")." << '\n';
	std::cout << "  -s --send N            Send timeout: N milliseconds" << '\n';
	std::cout << "                         (default: " << BtKeyClient::s_nDefaultTimeoutSend << ")." << '\n';
	std::cout << "  -n --noop N            Send NOOP after N milliseconds" << '\n';
	std::cout << "                         (default: " << BtKeyClient::s_nDefaultNoopAfter << ", none: 0)." << '\n';
	std::cout << "  -i --interval N        Interval: N milliseconds" << '\n';
	std::cout << "                         Granularity of timeout checks and noop" << '\n';
	std::cout << "                         (default: " << BtKeyClient::s_nDefaultInterval << ")." << '\n';
	std::cout << "  -r --refresh N         N * 1.28 sec detection time (Refresh)." << '\n';
	std::cout << "                         (default: " << BtKeyServers::s_nDefault1s28PeriodsAddr << ")." << '\n';
	std::cout << "  -f --flush             Forget previously found devices" << '\n';
	std::cout << "                         with a new Refresh." << '\n';
}

void evalNoArg(int& argc, char**& argv, const std::string& sOption1, const std::string& sOption2, bool& bVar)
{
	if (argv[1] == nullptr) {
		return;
	}
	const bool bIsOption1 = (sOption1 == std::string(argv[1]));
	if (bIsOption1 || ((!sOption2.empty()) && (sOption2 == std::string(argv[1])))) {
		bVar = true;
		--argc;
		++argv;
	}
}
bool evalIntArg(int& argc, char**& argv, const std::string& sOption1, const std::string& sOption2, int32_t& nVar, int32_t nMin)
{
	if (argv[1] == nullptr) {
		return true;
	}
	const bool bIsOption1 = (sOption1 == std::string(argv[1]));
	if (bIsOption1 || ((!sOption2.empty()) && (sOption2 == std::string(argv[1])))) {
		--argc;
		++argv;
		if (argc == 1) {
			std::cout << "Error: " << (bIsOption1 ? sOption1 : sOption2) << " missing argument" << '\n';
			return false; //----------------------------------------------------
		} else {
			try {
				double fInterval = Glib::Ascii::strtod(argv[1]);
				if (fInterval < nMin) {
					nVar = nMin;
				} else {
					nVar = std::ceil(fInterval);
				}
			} catch (const std::runtime_error& oErr) {
				std::cout << "Error: " << oErr.what() << '\n';
				return false; //------------------------------------------------
			}
			--argc;
			++argv;
		}
	}
	return true;
}
bool evalAddrArg(int& argc, char**& argv, const std::string& sOption1, const std::string& sOption2, ::bdaddr_t& oBtAddr)
{
	if (argv[1] == nullptr) {
		return true;
	}
	const bool bIsOption1 = (sOption1 == std::string(argv[1]));
	if (bIsOption1 || ((!sOption2.empty()) && (sOption2 == std::string(argv[1])))) {
		--argc;
		++argv;
		if (argc == 1) {
			std::cout << "Error: " << (bIsOption1 ? sOption1 : sOption2) << " missing argument" << '\n';
			return false; //----------------------------------------------------
		} else {
			std::string sAddr = argv[1];
			if (!BtKeyServers::isValidStringAddr(sAddr)) {
				std::cout << "Error: invalid bluetooth address" << '\n';
				return false; //------------------------------------------------
			}
			oBtAddr = BtKeyServers::getAddrFromString(sAddr);
			--argc;
			++argv;
		}
	}
	return true;
}
int bttestMain(int argc, char** argv)
{
	int32_t nTimeoutConnect = BtKeyClient::s_nDefaultTimeoutConnect;
	int32_t nTimeoutSend = BtKeyClient::s_nDefaultTimeoutSend;
	int32_t nInterval = BtKeyClient::s_nDefaultInterval;
	int32_t nNoopAfter = BtKeyClient::s_nDefaultNoopAfter;
	int32_t n1s28Periods = BtKeyServers::s_nDefault1s28PeriodsAddr;
	bool bRefreshFlush = false;
	int32_t nL2capPort = BtKeyServers::s_nDefaultL2capPort;
	::bdaddr_t oExtraAddr;
	memset(&oExtraAddr, 0, sizeof(oExtraAddr));
	bool bHelp = false;
	bool bVersion = false;
	//
	char* argvZeroSave = ((argc >= 1) ? argv[0] : nullptr);
	while (argc >= 2) {
		auto nOldArgC = argc;
		evalNoArg(argc, argv, "--help", "-h", bHelp);
		if (bHelp) {
			printUsage();
			return EXIT_SUCCESS; //---------------------------------------------
		}
		evalNoArg(argc, argv, "--version", "-v", bVersion);
		if (bVersion) {
			printVersion();
			return EXIT_SUCCESS; //---------------------------------------------
		}
		evalNoArg(argc, argv, "--flush", "-f", bRefreshFlush);
		//
		bool bOk = evalIntArg(argc, argv, "--interval", "-i", nInterval, 50);
		if (!bOk) {
			return EXIT_FAILURE; //---------------------------------------------
		}
		bOk = evalIntArg(argc, argv, "--connect", "-c", nTimeoutConnect, 100);
		if (!bOk) {
			return EXIT_FAILURE; //---------------------------------------------
		}
		bOk = evalIntArg(argc, argv, "--send", "-s", nTimeoutSend, 10);
		if (!bOk) {
			return EXIT_FAILURE; //---------------------------------------------
		}
		bOk = evalIntArg(argc, argv, "--noop", "-n", nNoopAfter, 0);
		if (!bOk) {
			return EXIT_FAILURE; //---------------------------------------------
		}
		bOk = evalIntArg(argc, argv, "--refresh", "-r", n1s28Periods, 1);
		if (!bOk) {
			return EXIT_FAILURE; //---------------------------------------------
		}
		bOk = evalIntArg(argc, argv, "--port", "-p", nL2capPort, 0);
		if (!bOk) {
			return EXIT_FAILURE; //---------------------------------------------
		}
		bOk = evalAddrArg(argc, argv, "--extra-server", "-e", oExtraAddr);
		if (!bOk) {
			return EXIT_FAILURE; //---------------------------------------------
		}
		if (nOldArgC == argc) {
			std::cout << "Unknown argument: " << std::string(argv[1]) << '\n';
			return EXIT_FAILURE; //---------------------------------------------
		}
		argv[0] = argvZeroSave;
	}

	if (! BtKeyServers::isValidPort(nL2capPort)) {
		std::cout << "Error: Port must be odd, from 4097 to 32765" << '\n';
		return EXIT_FAILURE; //-------------------------------------------------
	}

	int nRet = EXIT_SUCCESS;

	// server inquiry
	BtKeyServers oServers(n1s28Periods, nL2capPort, oExtraAddr, bRefreshFlush);
	{
		// client model
		BtKeyClient oClient(nTimeoutConnect, nTimeoutSend, nInterval, nNoopAfter);

		const Glib::ustring sAppName = "ch.efanomars.bttest";
		const Glib::ustring sWindoTitle = "stmm-input-bttest " + Config::getVersionString();
		try {
			//
			Glib::RefPtr<Gtk::Application> refApp = Gtk::Application::create(argc, argv, sAppName);
			Gnome::Conf::init();
			BttestWindow oWindow(sWindoTitle, oClient, oServers);
			nRet = refApp->run(oWindow);
			// Save "preferences"
			oWindow.saveStateToConfig();
		} catch (const std::runtime_error& oErr) {
			std::cout << "Error: " << oErr.what() << '\n';
			nRet = EXIT_FAILURE;
		}
	}
	if (oServers.isThreadHung()) {
		std::cout << "Sorry but a thread hung on a hci driver call ..." << '\n';
		std::terminate(); //----------------------------------------------------
	}
	return nRet;
}

} // namespace stmi

int main(int argc, char** argv)
{
	return stmi::bttestMain(argc, argv);
}

