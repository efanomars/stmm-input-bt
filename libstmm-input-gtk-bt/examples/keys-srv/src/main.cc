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
 * File:   main.cc
 */

#include "keyswindow.h"

#include <stmm-input-gtk-bt/stmm-input-gtk-bt.h>
#include <stmm-input/devicemanager.h>
#include <stmm-input/event.h>

#include <gtkmm.h>

#include <iostream>

namespace example
{

namespace keyssrv
{

int keysSrvMain(int argc, char** argv)
{
	const Glib::ustring sAppName = "net.exampleappsnirvana.keyssrv";
	const Glib::ustring sWindoTitle = "keys-srv - run from terminal";
	// Also initializes Gtk
	Glib::RefPtr<Gtk::Application> refApp = Gtk::Application::create(argc, argv, sAppName);
	std::cout << "keys-srv prints hardware key events from virtual keyboards connected" << '\n';
	std::cout << "         via bluetooth and tracks their lifetime as devices." << '\n';
	std::cout << "keys-srv uses libstmm-input-gtk-bt  version " << stmi::libconfig::gtkbt::getVersion() << '\n';
	// device manager has to be created after gtk is initialized!
	auto oPairDM = stmi::BtGtkDeviceManager::create("keys-srv", false, {});
	const shared_ptr<stmi::DeviceManager>& refDM = oPairDM.first;
	if (!refDM) {
		std::cout << oPairDM.second << '\n';
		return EXIT_FAILURE; //-------------------------------------------------
	}
	auto refKeysWindow = Glib::RefPtr<KeysWindow>(new KeysWindow(sWindoTitle));
	auto refAccessor = std::make_shared<stmi::GtkAccessor>(refKeysWindow);
	refDM->addAccessor(refAccessor);

	auto refAllListener = std::make_shared<stmi::EventListener>(
	[](const shared_ptr<stmi::Event>& refEvent)
	{
		std::vector< std::pair<stmi::HARDWARE_KEY, stmi::Event::AS_KEY_INPUT_TYPE> > aKeys = refEvent->getAsKeys();
		if (aKeys.empty()) {
			return;
		}
		std::cout << "Event: " << refEvent->getEventClass().getId();
		std::cout << "  Time: " << refEvent->getTimeUsec();
		auto refCapability = refEvent->getCapability();
		if (refCapability) {
			auto refDevice = refCapability->getDevice();
			if (refDevice) {
				std::cout << "  Device: " << refDevice->getName();
			}
		}
		std::cout << '\n';
		bool bFirst = true;
		for (auto& oPair : aKeys) {
			stmi::HARDWARE_KEY eHardwareKey = oPair.first;
			stmi::Event::AS_KEY_INPUT_TYPE eType = oPair.second;
			if (!bFirst) {
				std::cout << " - ";
			} else {
				std::cout << "  ";
				bFirst = false;
			}
			std::cout << "(" << static_cast<int32_t>(eHardwareKey) << ",";
			if (eType == stmi::Event::AS_KEY_PRESS) {
				std::cout << "PRESS";
			} else if (eType == stmi::Event::AS_KEY_RELEASE) {
				std::cout << "RELEASE";
			} else {
				std::cout << "CANCEL";
			}
			std::cout << ")";
		}
		std::cout << '\n';
	});
	refDM->addEventListener(refAllListener);
	//
	auto refMgmtListener = std::make_shared<stmi::EventListener>(
	[](const shared_ptr<stmi::Event>& refEvent)
	{
		assert(dynamic_cast<stmi::DeviceMgmtEvent*>(refEvent.get()) != nullptr);
		auto p0MgmtEvent = static_cast<stmi::DeviceMgmtEvent*>(refEvent.get());
		//
		auto refDevice = p0MgmtEvent->getDevice();
		std::cout << "Device ";
		//
		const auto eType = p0MgmtEvent->getDeviceMgmtType();
		if (eType == stmi::DeviceMgmtEvent::DEVICE_MGMT_ADDED) {
			std::cout << "ADDED";
		} else if (eType == stmi::DeviceMgmtEvent::DEVICE_MGMT_REMOVED) {
			std::cout << "REMOVED";
		} else {
			return; //------
		}
		std::cout << " Name: " << refDevice->getName() << "  Id: " << refDevice->getId() << '\n';
	});
	auto refCallIf = std::make_shared<stmi::CallIfEventClass>(stmi::DeviceMgmtEvent::getClass());
	refDM->addEventListener(refMgmtListener, refCallIf);

	return refApp->run(*(refKeysWindow.operator->()));
}

} // namespace keyssrv

} // namespace example

int main(int argc, char** argv)
{
	return example::keyssrv::keysSrvMain(argc, argv);
}

