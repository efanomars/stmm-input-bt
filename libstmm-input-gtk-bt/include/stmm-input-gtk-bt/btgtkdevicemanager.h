/*
 * Copyright © 2017-2019  Stefano Marsili, <stemars@gmx.ch>
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
 * File:   btgtkdevicemanager.h
 */

#ifndef STMI_BT_GTK_DEVICE_MANAGER_H
#define STMI_BT_GTK_DEVICE_MANAGER_H

#include <stmm-input-ev/stddevicemanager.h>
#include <stmm-input-ev/keyevent.h>

#include <stmm-input/event.h>
#include <stmm-input/hardwarekey.h>

#include <gtkmm.h>

#include <vector>
#include <string>
#include <memory>
#include <utility>

namespace stmi { class Accessor; }


namespace stmi
{

namespace Private
{
namespace Bt
{
	class GtkBackend;
	class BtKeysDevice;
	class GtkWindowData;
	class GtkWindowDataFactory;
	class BtGtkListenerExtraData;
} // namespace Bt
} // namespace Private

/** Handles bluetooth devices sending key events through a simple protocol.
 */
class BtGtkDeviceManager : public StdDeviceManager , public sigc::trackable
{
public:
	/** Creates an instance of this class.
	 *
	 * If bEnableEventClasses is `true` then all event classes in aEnDisableEventClasses are enabled, all others disabled,
	 * if `false` then all event classes supported by this instance are enabled except those in aEnDisableEventClasses.
	 * BtGtkDeviceManager doesn't allow disabling event classes once constructed, only enabling.
	 *
	 * Example: To enable all the event classes supported by this instance (currently just KeyEvent) pass
	 *
	 *     bEnableEventClasses = false,  aEnDisableEventClasses = {}
	 *
	 * @param sAppName The application name. Can be empty.
	 * @param bEnableEventClasses Whether to enable or disable all but aEnDisableEventClasses.
	 * @param aEnDisableEventClasses The event classes to be enabled or disabled according to bEnableEventClasses.
	 * @return The created instance and an empty string or null and an error string (example: couldn't create bluetooth server).
	 */
	static std::pair<shared_ptr<BtGtkDeviceManager>, std::string> create(const std::string& sAppName
																		, bool bEnableEventClasses, const std::vector<Event::Class>& aEnDisableEventClasses) noexcept;

	void enableEventClass(const Event::Class& oEventClass) noexcept override;

	/** Adds a stmi::GtkAccessor-wrapped Gtk::Window from which events should be received. 
	 * An instance of this class needs a stmi::GtkAccessor for each active Gtk::Window
	 * listeners want to receive events from.
	 * If the type of parameter refAccessor is not stmi::GtkAccessor, `false` is returned.
	 * If it is and the window isn't already added `true` is returned, `false` otherwise.
	 */
	bool addAccessor(const shared_ptr<Accessor>& refAccessor) noexcept override;
	/** Removes a stmi::GtkAccessor-wrapped Gtk::Window added with addAccessor().
	 * If the device manager has the accessor, `true` is returned, `false` otherwise.
	 * 
	 * This function doesn't delete the window itself, it just tells the device manager
	 * to stop tracking it.
	 * 
	 * Cancels are sent to listeners for each being pressed key.
	 */
	bool removeAccessor(const shared_ptr<Accessor>& refAccessor) noexcept override;
	/** Tells whether a window is already tracked by the device manager.
	 * @param refAccessor The wrapper of the Gtk::Window.
	 * @return Whether the window is currently tracked by the device manager.
	 */
	bool hasAccessor(const shared_ptr<Accessor>& refAccessor) noexcept override;
protected:
	void finalizeListener(ListenerData& oListenerData) noexcept override;
	/** Constructor.
	 * @see create()
	 */
	BtGtkDeviceManager(bool bEnableEventClasses, const std::vector<Event::Class>& aEnDisableEventClasses) noexcept;
	void init(std::unique_ptr<Private::Bt::GtkWindowDataFactory>& refFactory
								, std::unique_ptr<Private::Bt::GtkBackend>& refBackend) noexcept;
private:
	friend class Private::Bt::GtkBackend;
	void onDeviceAdded(const std::string& sName, int32_t nBackendId) noexcept;
	void onDeviceRemoved(int32_t nBackendId) noexcept;
	bool onBlueKey(int32_t nBackendId, KeyEvent::KEY_INPUT_TYPE eType, HARDWARE_KEY eHK) noexcept;

	bool findWindow(Gtk::Window* p0GtkmmWindow
					, std::vector< std::pair<Gtk::Window*, shared_ptr<Private::Bt::GtkWindowData> > >::iterator& itFind) noexcept;
	bool hasAccessor(const shared_ptr<Accessor>& refAccessor, bool& bValid
					, std::vector< std::pair<Gtk::Window*, shared_ptr<Private::Bt::GtkWindowData> > >::iterator& itFind) noexcept;
	void removeAccessor(const std::vector< std::pair<Gtk::Window*, shared_ptr<Private::Bt::GtkWindowData> > >::iterator& itGtkData) noexcept;
	void cancelDeviceKeys(const shared_ptr<Private::Bt::BtKeysDevice>& refBtKeysDevice) noexcept;
	void selectAccessor(const shared_ptr<Private::Bt::GtkWindowData>& refData) noexcept;
	void deselectAccessor() noexcept;
	void onIsActiveChanged(const shared_ptr<Private::Bt::GtkWindowData>& refWindowData) noexcept;
	
	friend class Private::Bt::GtkWindowData;
	friend class Private::Bt::BtKeysDevice;
	friend class Private::Bt::BtGtkListenerExtraData;
private:
	std::unique_ptr<Private::Bt::GtkWindowDataFactory> m_refFactory;
	std::unique_ptr<Private::Bt::GtkBackend> m_refBackend;
	// The GtkAccessor (GtkWindowData::m_refAccessor) will tell 
	// when the window gets deleted. The accessor can also be removed
	// explicitely during a listener callback.
	std::vector<std::pair<Gtk::Window*, shared_ptr<Private::Bt::GtkWindowData> > > m_aGtkWindowData;
	// The currently active accessor (window), can be null.
	std::shared_ptr<Private::Bt::GtkWindowData> m_refSelected;
	// Invariants:
	// - if m_refSelected is null, no button or hat is pressed in any of the joystick devices
	// - if m_refSelected is not null, all pressed buttons and hats were generated
	//   for the selected window.

	int32_t m_nCancelingNestedDepth;
	//
	std::vector< shared_ptr<Private::Bt::BtKeysDevice> > m_aBtDevices; // Index: nBackendId, Value: can be null!
	//
	const int32_t m_nClassIdxKeyEvent;
private:
	BtGtkDeviceManager(const BtGtkDeviceManager& oSource) = delete;
	BtGtkDeviceManager& operator=(const BtGtkDeviceManager& oSource) = delete;
};

} // namespace stmi

#endif /* STMI_BT_GTK_DEVICE_MANAGER_H */

