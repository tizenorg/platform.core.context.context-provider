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

#include <types_internal.h>
#include <ContextManager.h>
#include <ContextProviderBase.h>
#include <device_context_provider.h>

#include "system/system_types.h"
#include "social/social_types.h"
#include "activity/activity_types.h"

#include "system/alarm.h"
#include "system/time.h"

#ifdef _MOBILE_
#include "system/runtime-info/charger.h"
#include "system/runtime-info/gps.h"
#include "system/runtime-info/usb.h"
#include "system/wifi.h"
#include "system/headphone.h"
#include "system/battery.h"
#include "system/psmode.h"
#include "social/call.h"
#include "social/email.h"
#include "social/message.h"
#include "social/contacts.h"
#include "activity/activity.h"
#endif

#ifdef _WEARABLE_
#include "system/runtime-info/charger.h"
#include "system/runtime-info/gps.h"
#include "system/runtime-info/usb.h"
#include "system/wifi.h"
#include "system/headphone.h"
#include "system/battery.h"
#include "system/psmode.h"
#include "social/call.h"
#include "social/message.h"
#include "activity/activity.h"
#endif

#ifdef _TV_
#include "system/wifi.h"
#include "system/headphone.h"
#endif

#define PRIV_NETWORK	"network.get"
#define PRIV_TELEPHONY	"telephony"
#define PRIV_MESSAGE	"message.read"
#define PRIV_CONTACT	"contact.read"

template<typename provider>
void registerProvider(const char *subject, const char *privilege)
{
	if (!provider::is_supported())
		return;

	ctx::ContextProviderInfo providerInfo(provider::create, provider::destroy, NULL, privilege);
	ctx::context_manager::registerProvider(subject, providerInfo);
	provider::submit_trigger_item();
}

EXTAPI bool ctx::init_device_context_provider()
{
	registerProvider<device_status_alarm>(DEVICE_ST_SUBJ_ALARM, NULL);
	registerProvider<device_status_time>(DEVICE_ST_SUBJ_TIME, NULL);

#ifdef _MOBILE_
	registerProvider<device_status_wifi>(DEVICE_ST_SUBJ_WIFI, PRIV_NETWORK);
	registerProvider<device_status_headphone>(DEVICE_ST_SUBJ_HEADPHONE, NULL);

	registerProvider<device_status_charger>(DEVICE_ST_SUBJ_CHARGER, NULL);
	registerProvider<device_status_gps>(DEVICE_ST_SUBJ_GPS, NULL);
	registerProvider<device_status_usb>(DEVICE_ST_SUBJ_USB, NULL);
	registerProvider<device_status_battery>(DEVICE_ST_SUBJ_BATTERY, NULL);
	registerProvider<device_status_psmode>(DEVICE_ST_SUBJ_PSMODE, NULL);

	registerProvider<social_status_call>(SOCIAL_ST_SUBJ_CALL, PRIV_TELEPHONY);
	registerProvider<social_status_email>(SOCIAL_ST_SUBJ_EMAIL, NULL);
	registerProvider<social_status_message>(SOCIAL_ST_SUBJ_MESSAGE, PRIV_MESSAGE);
	registerProvider<social_status_contacts>(SOCIAL_ST_SUBJ_CONTACTS, PRIV_CONTACT);

	registerProvider<user_activity_stationary>(USER_ACT_SUBJ_STATIONARY, NULL);
	registerProvider<user_activity_walking>(USER_ACT_SUBJ_WALKING, NULL);
	registerProvider<user_activity_running>(USER_ACT_SUBJ_RUNNING, NULL);
	registerProvider<user_activity_in_vehicle>(USER_ACT_SUBJ_IN_VEHICLE, NULL);

	/* Create context providers, which need to be initiated before being subscribed */
	if (device_status_wifi::is_supported())
		device_status_wifi::create(NULL);
#endif

#ifdef _WEARABLE_
	registerProvider<device_status_wifi>(DEVICE_ST_SUBJ_WIFI, PRIV_NETWORK);
	registerProvider<device_status_headphone>(DEVICE_ST_SUBJ_HEADPHONE, NULL);

	registerProvider<device_status_charger>(DEVICE_ST_SUBJ_CHARGER, NULL);
	registerProvider<device_status_gps>(DEVICE_ST_SUBJ_GPS, NULL);
	registerProvider<device_status_usb>(DEVICE_ST_SUBJ_USB, NULL);
	registerProvider<device_status_battery>(DEVICE_ST_SUBJ_BATTERY, NULL);
	registerProvider<device_status_psmode>(DEVICE_ST_SUBJ_PSMODE, NULL);

	registerProvider<social_status_call>(SOCIAL_ST_SUBJ_CALL, PRIV_TELEPHONY);
	registerProvider<social_status_message>(SOCIAL_ST_SUBJ_MESSAGE, PRIV_MESSAGE);

	registerProvider<user_activity_stationary>(USER_ACT_SUBJ_STATIONARY, NULL);
	registerProvider<user_activity_walking>(USER_ACT_SUBJ_WALKING, NULL);
	registerProvider<user_activity_running>(USER_ACT_SUBJ_RUNNING, NULL);
	registerProvider<user_activity_in_vehicle>(USER_ACT_SUBJ_IN_VEHICLE, NULL);

	/* Create context providers, which need to be initiated before being subscribed */
	if (device_status_wifi::is_supported())
		device_status_wifi::create(NULL);
#endif

#ifdef _TV_
	registerProvider<device_status_wifi>(DEVICE_ST_SUBJ_WIFI, PRIV_NETWORK);
	registerProvider<device_status_headphone>(DEVICE_ST_SUBJ_HEADPHONE, NULL);

	/* Create context providers, which need to be initiated before being subscribed */
	if (device_status_wifi::is_supported())
		device_status_wifi::create(NULL);
#endif

	return true;
}
