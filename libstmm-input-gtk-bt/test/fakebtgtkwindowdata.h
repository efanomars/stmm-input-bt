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
 * File:   fakebtgtkwindowdata.h
 */

#ifndef STMI_TESTING_FAKE_BT_GTK_WINDOW_DATA_H
#define STMI_TESTING_FAKE_BT_GTK_WINDOW_DATA_H

#include "btgtkwindowdata.h"

#include "fakebtgtkbackend.h"

namespace stmi
{

using std::shared_ptr;
using std::weak_ptr;

namespace testing
{
namespace Bt
{

class FakeGtkWindowDataFactory;

////////////////////////////////////////////////////////////////////////////////
class FakeGtkWindowData : public Private::Bt::GtkWindowData
{
public:
	FakeGtkWindowData()
	: Private::Bt::GtkWindowData()
	, m_bIsEnabled(false)
	, m_bWindowActive(false)
	{
	}
	void enable(const shared_ptr<GtkAccessor>& refAccessor, BtGtkDeviceManager* p0Owner) override
	{
		assert(refAccessor);
		assert(p0Owner != nullptr);
		setOwner(p0Owner);
		m_refAccessor = refAccessor;
		m_bIsEnabled= true;
	}
	// !!! Doesn't reset accessor!
	void disable() override
	{
		m_bIsEnabled = false;
		disconnect();
	}
	bool isEnabled() const override
	{
		return m_bIsEnabled;
	}
	const shared_ptr<GtkAccessor>& getAccessor() override
	{
		return m_refAccessor;
	}
	bool isWindowActive() const override
	{
		return m_bWindowActive;
	}

	//
	void simulateWindowSetActive(bool bActive)
	{
		if (m_bWindowActive == bActive) {
			// no change
			return;
		}
		m_bWindowActive = bActive;
		onSigIsActiveChanged();
	}

private:
	void disconnect()
	{
//std::cout << "GtkWindowData::disconnect()" << '\n';
		m_aConnectedDevices.clear();
	}
	
private:
	friend class FakeGtkWindowDataFactory;
	shared_ptr<GtkAccessor> m_refAccessor;
	//
	std::vector<int32_t> m_aConnectedDevices; // Value: device id
	//
	bool m_bIsEnabled;
	//
	bool m_bWindowActive;
};

////////////////////////////////////////////////////////////////////////////////
class FakeGtkWindowDataFactory : public Private::Bt::GtkWindowDataFactory
{
public:
	FakeGtkWindowDataFactory() // FakeGtkBackend* p0FakeGtkBackend
	: Private::Bt::GtkWindowDataFactory()
	{
	}
	std::shared_ptr<Private::Bt::GtkWindowData> create() override
	{
		for (auto& refGtkWindowData : m_aFreePool) {
			if (refGtkWindowData.use_count() == 1) {
				// Recycle
				return refGtkWindowData; //-------------------------------------
			}
		}
		// The data is left in the pool!
		m_aFreePool.emplace_back(std::make_shared<FakeGtkWindowData>()); //m_p0FakeGtkBackend
		auto& refNew = m_aFreePool.back();
		return refNew;
	}
	const std::shared_ptr<FakeGtkWindowData>& getFakeWindowData(Gtk::Window* p0GtkmmWindow)
	{
		for (auto& refGtkWindowData : m_aFreePool) {
			if (!(refGtkWindowData.use_count() == 1)) {
				auto& refAccessor = refGtkWindowData->getAccessor();
				if ((refAccessor) && (!refAccessor->isDeleted()) && (refAccessor->getGtkmmWindow() == p0GtkmmWindow)) {
					return refGtkWindowData;
				}
			}
		}
		static std::shared_ptr<FakeGtkWindowData> s_refEmpty{};
		return s_refEmpty;
	}
	// There should be max one active window at any time
	// Any way this function returns the first active found.
	const std::shared_ptr<FakeGtkWindowData>& getActiveWindowData()
	{
		for (auto& refGtkWindowData : m_aFreePool) {
			if (!(refGtkWindowData.use_count() == 1)) {
				auto& refAccessor = refGtkWindowData->getAccessor();
				if ((refAccessor) && (!refAccessor->isDeleted()) && refGtkWindowData->isWindowActive()) {
					return refGtkWindowData;
				}
			}
		}
		static std::shared_ptr<FakeGtkWindowData> s_refEmpty{};
		return s_refEmpty;
	}
private:
	// The objects in the free pool might still be in use when the
	// removal of the accessor was done during a callback. This is detected
	// through the ref count of the shared_ptr.
	std::vector< std::shared_ptr<FakeGtkWindowData> > m_aFreePool;
};

} // namespace Bt
} // namespace testing

} // namespace stmi

#endif /* STMI_TESTING_FAKE_BT_GTK_WINDOW_DATA_H */
