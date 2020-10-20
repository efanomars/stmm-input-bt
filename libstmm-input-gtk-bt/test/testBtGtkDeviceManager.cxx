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
 * File:   testBtGtkDeviceManager.cxx
 */

#define CATCH_CONFIG_MAIN
#include "catch2/catch.hpp"

#include "fixtureBtDM.h"

#include <stmm-input/callifs.h>

namespace stmi
{

using std::shared_ptr;
using std::weak_ptr;

namespace testing
{

////////////////////////////////////////////////////////////////////////////////
TEST_CASE_METHOD(STFX<GlibAppFixture>, "CreateFakeBtGtkDeviceManager")
{
	auto refBtDM = FakeBtGtkDeviceManager::create(false, {});
	REQUIRE(refBtDM.operator bool());
}

////////////////////////////////////////////////////////////////////////////////
TEST_CASE_METHOD(STFX<BtDMFixture>, "AddAccessor")
{
	auto refWin1 = Glib::RefPtr<Gtk::Window>(new Gtk::Window());
	REQUIRE(refWin1.operator bool());
	auto refAccessor = std::make_shared<stmi::GtkAccessor>(refWin1);
	const bool bAdded = m_refAllEvDM->addAccessor(refAccessor);

	REQUIRE(bAdded);
	REQUIRE(m_refAllEvDM->hasAccessor(refAccessor));
	const bool bRemoved = m_refAllEvDM->removeAccessor(refAccessor);
	REQUIRE(bRemoved);
	REQUIRE_FALSE(m_refAllEvDM->hasAccessor(refAccessor));
}

TEST_CASE_METHOD(STFX<BtDMFixture>, "AddListener")
{
	auto refListener = std::make_shared<stmi::EventListener>(
			[](const shared_ptr<stmi::Event>& /*refEvent*/)
			{
			});
	const bool bAdded = m_refAllEvDM->addEventListener(refListener);
	REQUIRE(bAdded);
	const bool bAdded2 = m_refAllEvDM->addEventListener(refListener);
	REQUIRE_FALSE(bAdded2);

	const bool bRemoved = m_refAllEvDM->removeEventListener(refListener);
	REQUIRE(bRemoved);
	const bool bRemoved2 = m_refAllEvDM->removeEventListener(refListener);
	REQUIRE_FALSE(bRemoved2);
}

TEST_CASE_METHOD(STFX<BtDMFixture>, "DeviceAddAndRemoveChange")
{
	std::vector< shared_ptr<stmi::Event> > aReceivedEvents;
	auto refListener = std::make_shared<stmi::EventListener>(
			[&](const shared_ptr<stmi::Event>& refEvent)
			{
				aReceivedEvents.emplace_back(refEvent);
			});
	const bool bListenerAdded = m_refAllEvDM->addEventListener(refListener, std::shared_ptr<stmi::CallIf>{});
	REQUIRE(bListenerAdded);

	auto refFakeBackend = m_refAllEvDM->getBackend();

	const int32_t nSimuId1 = refFakeBackend->simulateNewDevice(Bt::FakeGtkBackend::getBdAddrFromString("CC:CC:CC:CC:CC:CC"));
	REQUIRE(nSimuId1 >= 0);
	REQUIRE(aReceivedEvents.size() == 1);
	int32_t nDeviceId1;
	{
		const shared_ptr<stmi::Event>& refEvent = aReceivedEvents[0];
		REQUIRE(refEvent.operator bool());
		nDeviceId1 = refEvent->getCapabilityId();
		REQUIRE(refEvent->getEventClass() == typeid(stmi::DeviceMgmtEvent));
		auto refDeviceMgmtEvent = std::static_pointer_cast<stmi::DeviceMgmtEvent>(refEvent);
		REQUIRE(refDeviceMgmtEvent->getDeviceMgmtType() == stmi::DeviceMgmtEvent::DEVICE_MGMT_ADDED);
	}

	const int32_t nSimuId2 = refFakeBackend->simulateNewDevice(Bt::FakeGtkBackend::getBdAddrFromString("77:77:77:77:77:77"));
	REQUIRE(nSimuId2 >= 0);
	REQUIRE(aReceivedEvents.size() == 2);
	int32_t nDeviceId2;
	{
		const shared_ptr<stmi::Event>& refEvent = aReceivedEvents[1];
		REQUIRE(refEvent.operator bool());
		nDeviceId2 = refEvent->getCapabilityId();
		REQUIRE(refEvent->getEventClass() == typeid(stmi::DeviceMgmtEvent));
		auto refDeviceMgmtEvent = std::static_pointer_cast<stmi::DeviceMgmtEvent>(refEvent);
		REQUIRE(refDeviceMgmtEvent->getDeviceMgmtType() == stmi::DeviceMgmtEvent::DEVICE_MGMT_ADDED);
	}

	refFakeBackend->simulateRemoveDevice(nSimuId1);
	REQUIRE(aReceivedEvents.size() == 3);
	{
		const shared_ptr<stmi::Event>& refEvent = aReceivedEvents[2];
		REQUIRE(refEvent.operator bool());
		REQUIRE(refEvent->getCapabilityId() == nDeviceId1);
		REQUIRE(refEvent->getEventClass() == typeid(stmi::DeviceMgmtEvent));
		auto refDeviceMgmtEvent = std::static_pointer_cast<stmi::DeviceMgmtEvent>(refEvent);
		REQUIRE(refDeviceMgmtEvent->getDeviceMgmtType() == stmi::DeviceMgmtEvent::DEVICE_MGMT_REMOVED);
		// when removed device persists only within callback
		REQUIRE_FALSE(refDeviceMgmtEvent->getDevice().operator bool());
	}

	refFakeBackend->simulateRemoveDevice(nSimuId2);
	REQUIRE(aReceivedEvents.size() == 4);
	{
		const shared_ptr<stmi::Event>& refEvent = aReceivedEvents[3];
		REQUIRE(refEvent.operator bool());
		REQUIRE(refEvent->getCapabilityId() == nDeviceId2);
		REQUIRE(refEvent->getEventClass() == typeid(stmi::DeviceMgmtEvent));
		auto refDeviceMgmtEvent = std::static_pointer_cast<stmi::DeviceMgmtEvent>(refEvent);
		REQUIRE(refDeviceMgmtEvent->getDeviceMgmtType() == stmi::DeviceMgmtEvent::DEVICE_MGMT_REMOVED);
		// when removed device persists only within callback
		REQUIRE_FALSE(refDeviceMgmtEvent->getDevice().operator bool());
	}
}

////////////////////////////////////////////////////////////////////////////////
TEST_CASE_METHOD(STFX<BtDMOneWinOneAccOneDevOneListenerFixture>, "SingleKeyPress")
{
	auto refFakeFactory = m_refAllEvDM->getFactory();
	const std::shared_ptr<Bt::FakeGtkWindowData>& refWinData1 = refFakeFactory->getFakeWindowData(m_refWin1.operator->());
	REQUIRE(refWinData1.operator bool());
	assert(!refWinData1->isWindowActive());

	m_refAllEvDM->makeWindowActive(m_refGtkAccessor1);
	assert(refWinData1->isWindowActive());

	REQUIRE(m_aReceivedEvents1.size() == 0);

	m_p0FakeBackend->simulateKeyEvent(m_nBtDeviceId, KeyEvent::KEY_PRESS, stmi::HK_F1);

	REQUIRE(m_aReceivedEvents1.size() == 1);

	const shared_ptr<stmi::Event>& refEvent = m_aReceivedEvents1[0];
	REQUIRE(refEvent.operator bool());
	REQUIRE(refEvent->getEventClass() == typeid(stmi::KeyEvent));
	auto p0KeyEvent = static_cast<stmi::KeyEvent*>(refEvent.get());
	REQUIRE(p0KeyEvent->getKey() == stmi::HK_F1);
	REQUIRE(p0KeyEvent->getType() == stmi::KeyEvent::KEY_PRESS);
}

TEST_CASE_METHOD(STFX<BtDMOneWinOneAccOneDevOneListenerFixture>, "TwoAccessorKeyCancel")
{
	auto refWin2 = Glib::RefPtr<Gtk::Window>(new Gtk::Window());
	assert(refWin2.operator bool());
	auto refGtkAccessor2 = std::make_shared<stmi::GtkAccessor>(refWin2);
	assert(refGtkAccessor2.operator bool());
	#ifndef NDEBUG
	const bool bAdded = 
	#endif
	m_refAllEvDM->addAccessor(refGtkAccessor2);
	assert(bAdded);

	auto refFakeFactory = m_refAllEvDM->getFactory();
	const std::shared_ptr<Bt::FakeGtkWindowData>& refWinData1 = refFakeFactory->getFakeWindowData(m_refWin1.operator->());
	REQUIRE(refWinData1.operator bool());
	const std::shared_ptr<Bt::FakeGtkWindowData>& refWinData2 = refFakeFactory->getFakeWindowData(refWin2.operator->());
	REQUIRE(refWinData2.operator bool());

	REQUIRE(m_refAllEvDM->hasAccessor(m_refGtkAccessor1));
	REQUIRE(m_refAllEvDM->hasAccessor(refGtkAccessor2));

	m_refAllEvDM->makeWindowActive(m_refGtkAccessor1);

	m_p0FakeBackend->simulateKeyEvent(m_nBtDeviceId, KeyEvent::KEY_PRESS, stmi::HK_F1); // send press F1 key

	m_refAllEvDM->makeWindowActive(refGtkAccessor2);

	m_p0FakeBackend->simulateKeyEvent(m_nBtDeviceId, KeyEvent::KEY_PRESS, stmi::HK_F2); // send press F2 key

	m_refAllEvDM->removeAccessor(refGtkAccessor2);

	REQUIRE(m_refAllEvDM->hasAccessor(m_refGtkAccessor1));
	REQUIRE_FALSE(m_refAllEvDM->hasAccessor(refGtkAccessor2));

	REQUIRE(m_aReceivedEvents1.size() == 4); // received both F1 and F2 Press + Cancel to active window

	for (auto nIdx = 0u; nIdx < m_aReceivedEvents1.size(); ++nIdx) {
		auto& refEvent = m_aReceivedEvents1[nIdx];
		REQUIRE(refEvent.operator bool());
		REQUIRE(refEvent->getEventClass() == typeid(stmi::KeyEvent));
		auto p0KeyEvent = static_cast<stmi::KeyEvent*>(refEvent.get());
		if (nIdx == 0) {
			REQUIRE(p0KeyEvent->getKey() == stmi::HK_F1);
			REQUIRE(p0KeyEvent->getType() == stmi::KeyEvent::KEY_PRESS);
		} else if (nIdx == 1) {
			REQUIRE(p0KeyEvent->getKey() == stmi::HK_F1);
			REQUIRE(p0KeyEvent->getType() == stmi::KeyEvent::KEY_RELEASE_CANCEL);
		} else if (nIdx == 2) {
			REQUIRE(p0KeyEvent->getKey() == stmi::HK_F2);
			REQUIRE(p0KeyEvent->getType() == stmi::KeyEvent::KEY_PRESS);
		} else if (nIdx == 3) {
			REQUIRE(p0KeyEvent->getKey() == stmi::HK_F2);
			REQUIRE(p0KeyEvent->getType() == stmi::KeyEvent::KEY_RELEASE_CANCEL);
		}
	}
}

TEST_CASE_METHOD(STFX<BtDMOneWinOneAccOneDevOneListenerFixture>, "ThreeListeners")
{
	m_refAllEvDM->makeWindowActive(m_refGtkAccessor1);

	std::vector<shared_ptr<stmi::Event> > aReceivedEvents3;
	auto refListener3 = std::make_shared<stmi::EventListener>(
			[&](const shared_ptr<stmi::Event>& refEvent)
			{
				aReceivedEvents3.emplace_back(refEvent);
			});
	//
	std::vector<shared_ptr<stmi::Event> > aReceivedEvents2;
	auto refListener2 = std::make_shared<stmi::EventListener>(
			[&](const shared_ptr<stmi::Event>& refEvent)
			{
				aReceivedEvents2.emplace_back(refEvent);
				REQUIRE(refEvent.operator bool());
				REQUIRE(refEvent->getEventClass() == typeid(stmi::KeyEvent));
				auto p0KeyEvent = static_cast<stmi::KeyEvent*>(refEvent.get());
				if ((p0KeyEvent->getType() == KeyEvent::KEY_PRESS) && (p0KeyEvent->getKey() == stmi::HK_F1)) {
					// Add third listener
					const bool bListenerAdded3 = m_refAllEvDM->addEventListener(refListener3, std::shared_ptr<stmi::CallIf>{});
					REQUIRE(bListenerAdded3);
				}
			});
	const bool bListenerAdded2 = m_refAllEvDM->addEventListener(refListener2, std::make_shared<stmi::CallIfEventClass>(stmi::KeyEvent::getClass()));
	REQUIRE(bListenerAdded2);

	auto refFakeFactory = m_refAllEvDM->getFactory();
	#ifndef NDEBUG
	const std::shared_ptr<Bt::FakeGtkWindowData>& refWinData1 = 
	#endif //NDEBUG
	refFakeFactory->getFakeWindowData(m_refWin1.operator->());
	assert(refWinData1);

	m_p0FakeBackend->simulateKeyEvent(m_nBtDeviceId, KeyEvent::KEY_PRESS, stmi::HK_F1); // send press F1 key
	
	REQUIRE(m_aReceivedEvents1.size() == 1);
	REQUIRE(aReceivedEvents2.size() == 1);
	REQUIRE(aReceivedEvents3.size() == 0);

	m_p0FakeBackend->simulateKeyEvent(m_nBtDeviceId, KeyEvent::KEY_RELEASE, stmi::HK_F1); // send release F1 key

	REQUIRE(m_aReceivedEvents1.size() == 2);
	REQUIRE(aReceivedEvents2.size() == 2);
	REQUIRE(aReceivedEvents3.size() == 0); // Since it wasn't pressed when added listener 3 doesn't receive release

	m_p0FakeBackend->simulateKeyEvent(m_nBtDeviceId, KeyEvent::KEY_PRESS, stmi::HK_F2); // send press F2

	REQUIRE(m_aReceivedEvents1.size() == 3);
	REQUIRE(aReceivedEvents2.size() == 3);
	REQUIRE(aReceivedEvents3.size() == 1);

	m_refAllEvDM->removeEventListener(m_refListener1, true);

	REQUIRE(m_aReceivedEvents1.size() == 4);
	REQUIRE(aReceivedEvents2.size() == 3);
	REQUIRE(aReceivedEvents3.size() == 1);

	m_p0FakeBackend->simulateKeyEvent(m_nBtDeviceId, KeyEvent::KEY_PRESS, stmi::HK_F2); // send press F2 (suppressed)

	REQUIRE(m_aReceivedEvents1.size() == 4);
	REQUIRE(aReceivedEvents2.size() == 3);
	REQUIRE(aReceivedEvents3.size() == 1);

	m_p0FakeBackend->simulateKeyEvent(m_nBtDeviceId, KeyEvent::KEY_PRESS, stmi::HK_F3); // send press F3

	REQUIRE(m_aReceivedEvents1.size() == 4); // no longer active
	REQUIRE(aReceivedEvents2.size() == 4);
	REQUIRE(aReceivedEvents3.size() == 2);

	const bool bRemoved = m_refAllEvDM->removeAccessor(m_refGtkAccessor1);
	REQUIRE(bRemoved);

	REQUIRE(m_aReceivedEvents1.size() == 4); // no longer active
	REQUIRE(aReceivedEvents2.size() == 6);
	REQUIRE(aReceivedEvents3.size() == 4);

	for (auto nIdx = 0u; nIdx < m_aReceivedEvents1.size(); ++nIdx) {
		auto& refEvent = m_aReceivedEvents1[nIdx];
		REQUIRE(refEvent.operator bool());
		REQUIRE(refEvent->getEventClass() == typeid(stmi::KeyEvent));
		auto p0KeyEvent = static_cast<stmi::KeyEvent*>(refEvent.get());
		if (nIdx == 0) {
			REQUIRE(p0KeyEvent->getKey() == stmi::HK_F1);
			REQUIRE(p0KeyEvent->getType() == stmi::KeyEvent::KEY_PRESS);
		} else if (nIdx == 1) {
			REQUIRE(p0KeyEvent->getKey() == stmi::HK_F1);
			REQUIRE(p0KeyEvent->getType() == stmi::KeyEvent::KEY_RELEASE);
		} else if (nIdx == 2) {
			REQUIRE(p0KeyEvent->getKey() == stmi::HK_F2);
			REQUIRE(p0KeyEvent->getType() == stmi::KeyEvent::KEY_PRESS);
		} else if (nIdx == 3) {
			REQUIRE(p0KeyEvent->getKey() == stmi::HK_F2);
			REQUIRE(p0KeyEvent->getType() == stmi::KeyEvent::KEY_RELEASE_CANCEL);
		}
	}
	{
		stmi::HARDWARE_KEY eKey = stmi::HK_NULL;
		stmi::Event::AS_KEY_INPUT_TYPE eType = stmi::Event::AS_KEY_PRESS;
		bool bMoreThanOne = true;
		const bool bAsKey = aReceivedEvents2[4]->getAsKey(eKey, eType, bMoreThanOne);
		REQUIRE(bAsKey);
		REQUIRE_FALSE(bMoreThanOne);
		if (eKey == stmi::HK_F3) {
			// the order of cancels sent when an accessor is removed is non-deterministic
			std::swap(aReceivedEvents2[4], aReceivedEvents2[5]);
		}
	}
	for (auto nIdx = 0u; nIdx < aReceivedEvents2.size(); ++nIdx) {
		auto& refEvent = aReceivedEvents2[nIdx];
		auto p0KeyEvent = static_cast<stmi::KeyEvent*>(refEvent.get());
		if (nIdx == 0) {
			REQUIRE(p0KeyEvent->getKey() == stmi::HK_F1);
			REQUIRE(p0KeyEvent->getType() == stmi::KeyEvent::KEY_PRESS);
		} else if (nIdx == 1) {
			REQUIRE(p0KeyEvent->getKey() == stmi::HK_F1);
			REQUIRE(p0KeyEvent->getType() == stmi::KeyEvent::KEY_RELEASE);
		} else if (nIdx == 2) {
			REQUIRE(p0KeyEvent->getKey() == stmi::HK_F2);
			REQUIRE(p0KeyEvent->getType() == stmi::KeyEvent::KEY_PRESS);
		} else if (nIdx == 3) {
			REQUIRE(p0KeyEvent->getKey() == stmi::HK_F3);
			REQUIRE(p0KeyEvent->getType() == stmi::KeyEvent::KEY_PRESS);
		} else if (nIdx == 4) {
			REQUIRE(p0KeyEvent->getKey() == stmi::HK_F2);
			REQUIRE(p0KeyEvent->getType() == stmi::KeyEvent::KEY_RELEASE_CANCEL);
		} else if (nIdx == 5) {
			REQUIRE(p0KeyEvent->getKey() == stmi::HK_F3);
			REQUIRE(p0KeyEvent->getType() == stmi::KeyEvent::KEY_RELEASE_CANCEL);
		}
	}
	{
		stmi::HARDWARE_KEY eKey = stmi::HK_NULL;
		stmi::Event::AS_KEY_INPUT_TYPE eType = stmi::Event::AS_KEY_PRESS;
		bool bMoreThanOne = true;
		const bool bAsKey = aReceivedEvents3[2]->getAsKey(eKey, eType, bMoreThanOne);
		REQUIRE(bAsKey);
		REQUIRE_FALSE(bMoreThanOne);
		if (eKey == stmi::HK_F3) {
			// the order of cancels sent when an accessor is removed is non-deterministic
			std::swap(aReceivedEvents3[2], aReceivedEvents3[3]);
		}
	}
	for (auto nIdx = 0u; nIdx < aReceivedEvents3.size(); ++nIdx) {
		auto& refEvent = aReceivedEvents3[nIdx];
		auto p0KeyEvent = static_cast<stmi::KeyEvent*>(refEvent.get());
		if (nIdx == 0) {
			REQUIRE(p0KeyEvent->getKey() == stmi::HK_F2);
			REQUIRE(p0KeyEvent->getType() == stmi::KeyEvent::KEY_PRESS);
		} else if (nIdx == 1) {
			REQUIRE(p0KeyEvent->getKey() == stmi::HK_F3);
			REQUIRE(p0KeyEvent->getType() == stmi::KeyEvent::KEY_PRESS);
		} else if (nIdx == 2) {
			REQUIRE(p0KeyEvent->getKey() == stmi::HK_F2);
			REQUIRE(p0KeyEvent->getType() == stmi::KeyEvent::KEY_RELEASE_CANCEL);
		} else if (nIdx == 3) {
			REQUIRE(p0KeyEvent->getKey() == stmi::HK_F3);
			REQUIRE(p0KeyEvent->getType() == stmi::KeyEvent::KEY_RELEASE_CANCEL);
		}
	}
}

////////////////////////////////////////////////////////////////////////////////
class BtDMDelayedEventEnablingFixture : public BtDMOneWinOneAccOneDevOneListenerFixture
										, public FixtureVariantEventClassesEnable_True
										, public FixtureVariantEventClasses_DeviceMgmtEvent
{
};
TEST_CASE_METHOD(STFX<BtDMDelayedEventEnablingFixture>, "DelayedEventEnabling")
{
	REQUIRE(m_refAllEvDM->isEventClassEnabled(DeviceMgmtEvent::getClass()));
	REQUIRE_FALSE(m_refAllEvDM->isEventClassEnabled(KeyEvent::getClass()));

	auto refFakeFactory = m_refAllEvDM->getFactory();
	const std::shared_ptr<Bt::FakeGtkWindowData>& refWinData1 = refFakeFactory->getFakeWindowData(m_refWin1.operator->());
	REQUIRE(refWinData1.operator bool());
	assert(!refWinData1->isWindowActive());

	m_refAllEvDM->makeWindowActive(m_refGtkAccessor1);
	assert(refWinData1->isWindowActive());

	auto nBtDeviceId = m_p0FakeBackend->simulateNewDevice(Bt::FakeGtkBackend::getBdAddrFromString("44:44:44:44:44:44"));
	assert(nBtDeviceId >= 0);

	REQUIRE(m_aReceivedEvents1.size() == 1);

	m_refAllEvDM->enableEventClass(KeyEvent::getClass());

	REQUIRE(m_refAllEvDM->isEventClassEnabled(DeviceMgmtEvent::getClass()));
	REQUIRE(m_refAllEvDM->isEventClassEnabled(KeyEvent::getClass()));

	m_p0FakeBackend->simulateKeyEvent(nBtDeviceId, KeyEvent::KEY_PRESS, stmi::HK_F1); // send press F1

	REQUIRE(m_aReceivedEvents1.size() == 2);
}

} // namespace testing

} // namespace stmi
