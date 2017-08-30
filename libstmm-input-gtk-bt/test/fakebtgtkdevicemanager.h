/*
 * Copyright © 2017  Stefano Marsili, <stemars@gmx.ch>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, see <http://www.gnu.org/licenses/>
 */
/*
 * File:   fakebtmasgtkdevicemanager.h
 */

#ifndef _STMI_TESTING_FAKE_BT_GTK_DEVICE_MANAGER_H_
#define _STMI_TESTING_FAKE_BT_GTK_DEVICE_MANAGER_H_

#include "btgtkdevicemanager.h"

#include "fakebtgtkbackend.h"
#include "fakebtgtkwindowdata.h"

namespace stmi
{

using std::shared_ptr;
using std::weak_ptr;

namespace testing
{

class FakeBtGtkDeviceManager : public BtGtkDeviceManager
{
public:
	static shared_ptr<FakeBtGtkDeviceManager> create(bool bEnableEventClasses, const std::vector<Event::Class>& aEnDisableEventClasses)
	{
		shared_ptr<FakeBtGtkDeviceManager> refInstance(
				new FakeBtGtkDeviceManager(bEnableEventClasses, aEnDisableEventClasses));
//std::cout << "FakeBtGtkDeviceManager::create 1" << '\n';
		auto refBackend = std::make_unique<Bt::FakeGtkBackend>(refInstance.operator->());
		auto refFactory = std::make_unique<Bt::FakeGtkWindowDataFactory>();
		refInstance->init(refFactory, refBackend);
		return refInstance;
	}
	FakeBtGtkDeviceManager(bool bEnableEventClasses, const std::vector<Event::Class>& aEnDisableEventClasses)
	: BtGtkDeviceManager(bEnableEventClasses, aEnDisableEventClasses)
	, m_p0Factory(nullptr)
	, m_p0Backend(nullptr)
	{
//std::cout << "FakeBtGtkDeviceManager::FakeBtGtkDeviceManager" << '\n';
	}

	bool makeWindowActive(const shared_ptr<stmi::GtkAccessor>& refGtkAccessor)
	{
//std::cout << "FakeBtGtkDeviceManager::makeWindowActive 0" << '\n';
		assert(m_p0Factory != nullptr);
		assert(refGtkAccessor);
		auto p0GtkmmWindow = refGtkAccessor->getGtkmmWindow();
		if (refGtkAccessor->isDeleted()) {
			return false; //-----------------------------------------------------
		}
		auto& refOldWinData = m_p0Factory->getActiveWindowData();
		if (refOldWinData) {
			assert(refOldWinData->getAccessor());
			if (refOldWinData->getAccessor()->getGtkmmWindow() == p0GtkmmWindow) {
				// already active
				return true; //-----------------------------------------------------
			}
			// set old active to inactive
			refOldWinData->simulateWindowSetActive(false);
		}
		auto& refNewWinData = m_p0Factory->getFakeWindowData(p0GtkmmWindow);
		refNewWinData->simulateWindowSetActive(true);
		return true;
	}
	Bt::FakeGtkWindowDataFactory* getFactory() { return m_p0Factory; }
	Bt::FakeGtkBackend* getBackend() { return m_p0Backend; }
protected:
	void init(std::unique_ptr<Bt::FakeGtkWindowDataFactory>& refFactory
			, std::unique_ptr<Bt::FakeGtkBackend>& refBackend)
	{
		m_p0Factory = refFactory.get();
		m_p0Backend = refBackend.get();
		std::unique_ptr<Private::Bt::GtkWindowDataFactory> refF(refFactory.release());
		std::unique_ptr<Private::Bt::GtkBackend> refB(refBackend.release());
		assert(m_p0Factory == refF.get());
		assert(m_p0Backend == refB.get());
		BtGtkDeviceManager::init(refF, refB);
	}
private:
	Bt::FakeGtkWindowDataFactory* m_p0Factory;
	Bt::FakeGtkBackend* m_p0Backend;
};

} // namespace testing

} // namespace stmi

#endif /* _STMI_TESTING_FAKE_BT_GTK_DEVICE_MANAGER_H_ */
