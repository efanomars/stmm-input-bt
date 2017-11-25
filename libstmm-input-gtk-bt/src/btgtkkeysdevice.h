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
 * File:   btgtkjoystickdevice.h
 */

#ifndef STMI_BT_GTK_KEYS_DEVICE_H
#define STMI_BT_GTK_KEYS_DEVICE_H

#include "btgtkdevicemanager.h"

#include "recycler.h"

#include <stmm-input-ev/keycapability.h>

#include <stmm-input-base/basicdevice.h>

namespace stmi
{

namespace Private
{
namespace Bt
{


using std::shared_ptr;
using std::weak_ptr;

class BtKeysDevice final : public BasicDevice<BtGtkDeviceManager>, public KeyCapability
						, public std::enable_shared_from_this<BtKeysDevice>, public sigc::trackable
{
public:
	BtKeysDevice(std::string sName, const shared_ptr<BtGtkDeviceManager>& refDeviceManager);
	virtual ~BtKeysDevice();
	//
	shared_ptr<Capability> getCapability(const Capability::Class& oClass) const override;
	shared_ptr<Capability> getCapability(int32_t nCapabilityId) const override;
	std::vector<int32_t> getCapabilities() const override;
	std::vector<Capability::Class> getCapabilityClasses() const override;
	//
	bool isKeyboard() const override { return true; }
	shared_ptr<Device> getDevice() const override;
	//

	inline int32_t getDeviceId() const { return Device::getId(); }

	// This is public so that there's no need to friend GtkBackend (or even FakeGtkBackend)
	bool onBlueKey(KeyEvent::KEY_INPUT_TYPE eType, HARDWARE_KEY eHK, const shared_ptr<GtkWindowData>& refWindowData);
private:
	friend class stmi::BtGtkDeviceManager;
	void cancelSelectedAccessorKeys();
	void finalizeListener(BtGtkDeviceManager::ListenerData& oListenerData, int64_t nEventTimeUsec);
	void removingDevice();
	//
	void handleKeyPress(BtGtkDeviceManager* p0Owner, const shared_ptr<GtkAccessor>& refSelectedAccessor
					, const shared_ptr<KeyCapability>& refThis
					, KeyEvent::KEY_INPUT_TYPE eType, HARDWARE_KEY eKey);
	//
	void sendKeyEventToListener(const BtGtkDeviceManager::ListenerData& oListenerData, int64_t nEventTimeUsec
								, uint64_t nPressedTimeStamp
								, KeyEvent::KEY_INPUT_TYPE eInputType, HARDWARE_KEY eHardwareKey
								, const shared_ptr<GtkAccessor>& refAccessor
								, const shared_ptr<KeyCapability>& refCapability
								, int32_t nClassIdxKeyEvent
								, shared_ptr<Event>& refEvent);
private:
	struct KeyData
	{
		uint64_t m_nPressedTimeStamp;
	};
	std::unordered_map<HARDWARE_KEY, KeyData> m_oPressedKeys;
	//
	class ReKeyEvent :public KeyEvent
	{
	public:
		ReKeyEvent(int64_t nTimeUsec, const shared_ptr<Accessor>& refAccessor
					, const shared_ptr<KeyCapability>& refKeyCapability, KEY_INPUT_TYPE eType, HARDWARE_KEY eKey)
		: KeyEvent(nTimeUsec, refAccessor, refKeyCapability, eType, eKey)
		{
		}
		void reInit(int64_t nTimeUsec, const shared_ptr<Accessor>& refAccessor
				, const shared_ptr<KeyCapability>& refKeyCapability, KEY_INPUT_TYPE eType, HARDWARE_KEY eKey)
		{
			setTimeUsec(nTimeUsec);
			setAccessor(refAccessor);
			setKeyCapability(refKeyCapability);
			setType(eType);
			setKey(eKey);
		}
	};
	Private::Recycler<ReKeyEvent, Event> m_oKeyEventRecycler;
	//
private:
	BtKeysDevice(const BtKeysDevice& oSource) = delete;
	BtKeysDevice& operator=(const BtKeysDevice& oSource) = delete;
};

} // namespace Bt
} // namespace Private

} // namespace stmi

#endif /* STMI_BT_GTK_KEYS_DEVICE_H */
