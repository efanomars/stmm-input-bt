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
 * File:   fixtureBtDM.h
 */

#ifndef _STMI_TESTING_FIXTURE_BT_DM_H_
#define _STMI_TESTING_FIXTURE_BT_DM_H_

#include "fixtureGlibApp.h"

#include "fixturevariantEventClasses.h"

#include "fakebtgtkdevicemanager.h"
#include <stmm-input/devicemanager.h>

namespace stmi
{

using std::shared_ptr;
using std::weak_ptr;

namespace testing
{

////////////////////////////////////////////////////////////////////////////////
class BtDMFixture : public GlibAppFixture
					, public FixtureVariantEventClassesEnable
					, public FixtureVariantEventClasses
{
protected:
	void SetUp() override
	{
		GlibAppFixture::SetUp();
		//
		const bool bEventClassesEnable = FixtureVariantEventClassesEnable::getEnable();
		const std::vector<Event::Class> aClasses = FixtureVariantEventClasses::getEventClasses();
		//
		m_refAllEvDM = FakeBtGtkDeviceManager::create(bEventClassesEnable, aClasses);
		assert(m_refAllEvDM.operator bool());
	}
	void TearDown() override
	{
		m_refAllEvDM.reset();
		GlibAppFixture::TearDown();
	}
public:
	shared_ptr<FakeBtGtkDeviceManager> m_refAllEvDM;
};

////////////////////////////////////////////////////////////////////////////////
class BtDMOneWinFixture : public BtDMFixture
{
protected:
	void SetUp() override
	{
		BtDMFixture::SetUp();
		m_refWin1 = Glib::RefPtr<Gtk::Window>(new Gtk::Window());
		assert(m_refWin1.operator bool());
	}
	void TearDown() override
	{
		m_refWin1.reset();
		BtDMFixture::TearDown();
	}
public:
	Glib::RefPtr<Gtk::Window> m_refWin1;
};

////////////////////////////////////////////////////////////////////////////////
class BtDMOneWinOneAccFixture : public BtDMOneWinFixture
{
protected:
	void SetUp() override
	{
		BtDMOneWinFixture::SetUp();
		m_refGtkAccessor1 = std::make_shared<stmi::GtkAccessor>(m_refWin1);
		assert(m_refGtkAccessor1.operator bool());
		#ifndef NDEBUG
		const bool bAdded = 
		#endif
		m_refAllEvDM->addAccessor(m_refGtkAccessor1);
		assert(bAdded);
	}
	void TearDown() override
	{
		m_refGtkAccessor1.reset();
		BtDMOneWinFixture::TearDown();
	}
public:
	shared_ptr<stmi::GtkAccessor> m_refGtkAccessor1;
};

////////////////////////////////////////////////////////////////////////////////
class BtDMOneWinOneAccOneDevFixture : public BtDMOneWinOneAccFixture
{
protected:
	void SetUp() override
	{
		BtDMOneWinOneAccFixture::SetUp();
		m_p0FakeBackend = m_refAllEvDM->getBackend();
		assert(m_p0FakeBackend != nullptr);
		m_nBtDeviceId = m_p0FakeBackend->simulateNewDevice(Bt::FakeGtkBackend::getBdAddrFromString("AA:AA:AA:AA:AA:AA"));
		assert(m_nBtDeviceId >= 0);
	}
	void TearDown() override
	{
		m_nBtDeviceId = -1;
		m_p0FakeBackend = nullptr;
		BtDMOneWinOneAccFixture::TearDown();
	}
public:
	Bt::FakeGtkBackend* m_p0FakeBackend;
	int32_t m_nBtDeviceId;
};

////////////////////////////////////////////////////////////////////////////////
class BtDMOneWinOneAccOneDevOneListenerFixture : public BtDMOneWinOneAccOneDevFixture
{
protected:
	void SetUp() override
	{
		BtDMOneWinOneAccOneDevFixture::SetUp();
		m_refListener1 = std::make_shared<stmi::EventListener>(
				[&](const shared_ptr<stmi::Event>& refEvent)
				{
					m_aReceivedEvents1.emplace_back(refEvent);
				});
		#ifndef NDEBUG
		const bool bListenerAdded =
		#endif
		m_refAllEvDM->addEventListener(m_refListener1, std::shared_ptr<stmi::CallIf>{});
		assert(bListenerAdded);
	}
	void TearDown() override
	{
		m_refListener1.reset();
		m_aReceivedEvents1.clear();
		BtDMOneWinOneAccOneDevFixture::TearDown();
	}
public:
	std::vector< shared_ptr<stmi::Event> > m_aReceivedEvents1;
	shared_ptr<stmi::EventListener> m_refListener1;
};

} // namespace testing

} // namespace stmi

#endif /* _STMI_TESTING_FIXTURE_BT_DM_H_ */
