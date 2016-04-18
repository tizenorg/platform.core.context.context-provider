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

#include <Types.h>
#include <ContextProvider.h>
#include <DeviceContextProvider.h>

#include "system/SystemTypes.h"
#include "social/SocialTypes.h"
#include "activity/ActivityTypes.h"

#include "system/Alarm.h"
#include "system/Time.h"

#ifdef _MOBILE_
#include "system/runtime_info/Charger.h"
#include "system/runtime_info/Gps.h"
#include "system/runtime_info/Usb.h"
#include "system/Wifi.h"
#include "system/Headphone.h"
#include "system/Battery.h"
#include "system/Psmode.h"
#include "social/Call.h"
#include "social/Email.h"
#include "social/Message.h"
#include "social/Contacts.h"
#include "activity/Activity.h"
#endif

#ifdef _WEARABLE_
#include "system/runtime_info/Charger.h"
#include "system/runtime_info/Gps.h"
#include "system/runtime_info/Usb.h"
#include "system/Wifi.h"
#include "system/Headphone.h"
#include "system/Battery.h"
#include "system/Psmode.h"
#include "social/Call.h"
#include "social/Message.h"
#include "activity/Activity.h"
#endif

#define PRIV_NETWORK	"network.get"
#define PRIV_TELEPHONY	"telephony"
#define PRIV_MESSAGE	"message.read"
#define PRIV_CONTACT	"contact.read"

using namespace ctx;

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

SO_EXPORT bool initDeviceContextProvider()
{
	registerProvider<DeviceStatusAlarm>(DEVICE_ST_SUBJ_ALARM, NULL);
	registerProvider<DeviceStatusTime>(DEVICE_ST_SUBJ_TIME, NULL);

#ifdef _MOBILE_
	registerProvider<DeviceStatusWifi>(DEVICE_ST_SUBJ_WIFI, PRIV_NETWORK);
	registerProvider<DeviceStatusHeadphone>(DEVICE_ST_SUBJ_HEADPHONE, NULL);

	registerProvider<DeviceStatusCharger>(DEVICE_ST_SUBJ_CHARGER, NULL);
	registerProvider<DeviceStatusGps>(DEVICE_ST_SUBJ_GPS, NULL);
	registerProvider<DeviceStatusUsb>(DEVICE_ST_SUBJ_USB, NULL);
	registerProvider<DeviceStatusBattery>(DEVICE_ST_SUBJ_BATTERY, NULL);
	registerProvider<DeviceStatusPsmode>(DEVICE_ST_SUBJ_PSMODE, NULL);

	registerProvider<SocialStatusCall>(SOCIAL_ST_SUBJ_CALL, PRIV_TELEPHONY);
	registerProvider<SocialStatusEmail>(SOCIAL_ST_SUBJ_EMAIL, NULL);
	registerProvider<SocialStatusMessage>(SOCIAL_ST_SUBJ_MESSAGE, PRIV_MESSAGE);
	registerProvider<SocialStatusContacts>(SOCIAL_ST_SUBJ_CONTACTS, PRIV_CONTACT);

	registerProvider<UserActivityStationary>(USER_ACT_SUBJ_STATIONARY, NULL);
	registerProvider<UserActivityWalking>(USER_ACT_SUBJ_WALKING, NULL);
	registerProvider<UserActivityRunning>(USER_ACT_SUBJ_RUNNING, NULL);
	registerProvider<UserActivityInVehicle>(USER_ACT_SUBJ_IN_VEHICLE, NULL);

	/* Create context providers, which need to be initiated before being subscribed */
	/*
	if (DeviceStatusWifi::isSupported())
		DeviceStatusWifi::create(NULL);
	*/
#endif

#ifdef _WEARABLE_
	registerProvider<DeviceStatusWifi>(DEVICE_ST_SUBJ_WIFI, PRIV_NETWORK);
	registerProvider<DeviceStatusHeadphone>(DEVICE_ST_SUBJ_HEADPHONE, NULL);

	registerProvider<DeviceStatusCharger>(DEVICE_ST_SUBJ_CHARGER, NULL);
	registerProvider<DeviceStatusGps>(DEVICE_ST_SUBJ_GPS, NULL);
	registerProvider<DeviceStatusUsb>(DEVICE_ST_SUBJ_USB, NULL);
	registerProvider<DeviceStatusBattery>(DEVICE_ST_SUBJ_BATTERY, NULL);
	registerProvider<DeviceStatusPsmode>(DEVICE_ST_SUBJ_PSMODE, NULL);

	registerProvider<SocialStatusCall>(SOCIAL_ST_SUBJ_CALL, PRIV_TELEPHONY);
	registerProvider<SocialStatusMessage>(SOCIAL_ST_SUBJ_MESSAGE, PRIV_MESSAGE);

	registerProvider<UserActivityStationary>(USER_ACT_SUBJ_STATIONARY, NULL);
	registerProvider<UserActivityWalking>(USER_ACT_SUBJ_WALKING, NULL);
	registerProvider<UserActivityRunning>(USER_ACT_SUBJ_RUNNING, NULL);
	registerProvider<UserActivityInVehicle>(USER_ACT_SUBJ_IN_VEHICLE, NULL);
#endif

	return true;
}
