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
 * File:   btgtkwindowdata.h
 */

#ifndef STMI_BT_GTK_WINDOW_DATA_H
#define STMI_BT_GTK_WINDOW_DATA_H

#include "btgtkdevicemanager.h"

#include "private-recycler.h"

#include <gtkmm.h>

namespace stmi
{

namespace Private
{
namespace Bt
{

using std::shared_ptr;
using std::weak_ptr;

class GtkWindowData : public std::enable_shared_from_this<GtkWindowData>, public sigc::trackable
{
public:
	GtkWindowData()
	: m_p0Owner(nullptr)
	, m_bIsEnabled(false)
	{
	}
	virtual ~GtkWindowData()
	{
		//std::cout << "Bt::GtkWindowData::~GtkWindowData()" << '\n';
		disable();
	}
	void reInit() {}

	#ifdef STMI_TESTING_IFACE
	virtual
	#endif
	void enable(const shared_ptr<GtkAccessor>& refAccessor, BtGtkDeviceManager* p0Owner)
	{
		assert(refAccessor);
		assert(p0Owner != nullptr);
		assert(!m_oIsActiveConn.connected());
		m_refAccessor = refAccessor;
		if (m_refAccessor->isDeleted()) {
			return; //----------------------------------------------------------
		}
		m_p0Owner = p0Owner;
		Gtk::Window* p0GtkmmWindow = m_refAccessor->getGtkmmWindow();
		m_oIsActiveConn = p0GtkmmWindow->property_is_active().signal_changed().connect(sigc::mem_fun(this, &GtkWindowData::onSigIsActiveChanged));
		m_bIsEnabled = true;
	}
	// !!! Doesn't reset accessor!
	#ifdef STMI_TESTING_IFACE
	virtual
	#endif
	void disable()
	{
		m_bIsEnabled = false;
		m_oIsActiveConn.disconnect();
	}
	#ifdef STMI_TESTING_IFACE
	virtual
	#else
	inline
	#endif
	bool isEnabled() const { return m_bIsEnabled; }

	#ifdef STMI_TESTING_IFACE
	virtual
	#else
	inline
	#endif
	const shared_ptr<GtkAccessor>& getAccessor() { return m_refAccessor; }

	#ifdef STMI_TESTING_IFACE
	virtual
	#else
	inline
	#endif
	bool isWindowActive() const
	{
		assert(m_refAccessor);
		auto p0GtkmmWindow = m_refAccessor->getGtkmmWindow();
		return p0GtkmmWindow->get_realized() && p0GtkmmWindow->get_visible() && p0GtkmmWindow->is_active();
	}

protected:
	inline void setOwner(BtGtkDeviceManager* p0Owner)
	{
		m_p0Owner = p0Owner;
	}
	void onSigIsActiveChanged() { m_p0Owner->onIsActiveChanged(shared_from_this()); }
private:
	//
	shared_ptr<GtkAccessor> m_refAccessor;
	BtGtkDeviceManager* m_p0Owner;
	//
	bool m_bIsEnabled;
	//
	sigc::connection m_oIsActiveConn;
};

////////////////////////////////////////////////////////////////////////////////
class GtkWindowDataFactory
{
public:
	#ifdef STMI_TESTING_IFACE
	virtual
	#endif
	shared_ptr<GtkWindowData> create()
	{
		shared_ptr<GtkWindowData> ref;
		m_oRecycler.create(ref);
		return ref;
	}
	#ifdef STMI_TESTING_IFACE
	virtual ~GtkWindowDataFactory() = default;
	#endif
private:
	Recycler<GtkWindowData> m_oRecycler;
};

} // namespace Bt
} // namespace Private

} // namespace stmi

#endif /* STMI_BT_GTK_WINDOW_DATA_H */
