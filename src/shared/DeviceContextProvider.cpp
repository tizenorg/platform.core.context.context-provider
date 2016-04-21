/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <new>
#include <DeviceContextProvider.h>

#include "../time/Alarm.h"
#include "../time/Time.h"
#include "../activity/Activity.h"
#include "../headphone/Headphone.h"
#include "../system/Battery.h"
#include "../system/Charger.h"
#include "../system/Gps.h"
#include "../system/Psmode.h"
#include "../system/Usb.h"
#include "../wifi/Wifi.h"

#ifdef _MOBILE_
#include "../call/Call.h"
#include "../contacts/Contacts.h"
#include "../email/Email.h"
#include "../message/Message.h"
#endif

#define PRIV_NETWORK	"network.get"
#define PRIV_TELEPHONY	"telephony"
#define PRIV_MESSAGE	"message.read"
#define PRIV_CONTACT	"contact.read"

template<typename Provider>
void registerProvider(const char *subject, const char *privilege)
{
	Provider *provider = new(std::nothrow) Provider();
	IF_FAIL_VOID_TAG(provider, _E, "Memory allocation failed");

	if (!provider->isSupported()) {
		delete provider;
		return;
	}

	provider->registerProvider(privilege, provider);
	provider->submitTriggerItem();
}

SO_EXPORT bool ctx::initDeviceContextProvider()
{
	registerProvider<AlarmProvider>(SUBJ_STATE_ALARM, NULL);
	registerProvider<TimeProvider>(SUBJ_STATE_TIME, NULL);

	registerProvider<WifiStateProvider>(SUBJ_STATE_WIFI, PRIV_NETWORK);
	registerProvider<HeadphoneStateProvider>(SUBJ_STATE_HEADPHONE, NULL);

	registerProvider<ChargerStateProvider>(SUBJ_STATE_CHARGER, NULL);
	registerProvider<GpsStateProvider>(SUBJ_STATE_GPS, NULL);
	registerProvider<UsbStateProvider>(SUBJ_STATE_USB, NULL);
	registerProvider<BatteryStateProvider>(SUBJ_STATE_BATTERY, NULL);
	registerProvider<PowerSaveModeProvider>(SUBJ_STATE_PSMODE, NULL);

	registerProvider<StationaryActivityProvider>(SUBJ_ACTIVITY_STATIONARY, NULL);
	registerProvider<WalkingActivityProvider>(SUBJ_ACTIVITY_WALKING, NULL);
	registerProvider<RunningActivityProvider>(SUBJ_ACTIVITY_RUNNING, NULL);
	registerProvider<InVehicleActivityProvider>(SUBJ_ACTIVITY_IN_VEHICLE, NULL);

#ifdef _MOBILE_
	registerProvider<CallStateProvider>(SUBJ_STATE_CALL, PRIV_TELEPHONY);
	registerProvider<EmailEventProvider>(SUBJ_STATE_EMAIL, NULL);
	registerProvider<MessageEventProvider>(SUBJ_STATE_MESSAGE, PRIV_MESSAGE);
	registerProvider<ContactsChangeProvider>(SUBJ_STATE_CONTACTS, PRIV_CONTACT);

	/* Create context providers, which need to be initiated before being subscribed */
	/*
	if (WifiStateProvider::isSupported())
		WifiStateProvider::create(NULL);
	*/
#endif

	return true;
}
