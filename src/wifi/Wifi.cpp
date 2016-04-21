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

#include <SharedVars.h>
#include <Util.h>
#include "Wifi.h"

using namespace ctx;

DeviceStatusWifi::DeviceStatusWifi() :
	BasicProvider(SUBJ_STATE_WIFI),
	__lastState(UNKNOWN),
	__isInitialized(false),
	__isActivated(false),
	__connState(WIFI_CONNECTION_STATE_FAILURE)
{
	IF_FAIL_VOID_TAG(__startMonitor(), _W, "WiFi monitor initialization failed");

	if (!__getCurrentState()) {
		__stopMonitor();
		_W("Getting current WiFi status failed");
	}
}

DeviceStatusWifi::~DeviceStatusWifi()
{
	__stopMonitor();
}

bool DeviceStatusWifi::isSupported()
{
	return util::getSystemInfoBool("tizen.org/feature/network.wifi");
}

void DeviceStatusWifi::submitTriggerItem()
{
	registerTriggerItem(OPS_SUBSCRIBE | OPS_READ,
			"{"
				"\"State\":{\"type\":\"string\",\"values\":[\"Disabled\",\"Unconnected\",\"Connected\"]},"
				"\"BSSID\":{\"type\":\"string\"}"
			"}",
			NULL);
}

bool DeviceStatusWifi::__getCurrentState()
{
	int err;

	if (!__isInitialized) {
		err = wifi_initialize();
		IF_FAIL_RETURN_TAG(err == WIFI_ERROR_NONE, false, _E, "wifi_initialize() failed");
	}

	err = wifi_is_activated(&__isActivated);
	IF_FAIL_RETURN_TAG(err == WIFI_ERROR_NONE, false, _E, "wifi_is_activated() failed");

	err = wifi_get_connection_state(&__connState);
	IF_FAIL_RETURN_TAG(err == WIFI_ERROR_NONE, false, _E, "wifi_get_connection_state() failed");

	if (__isActivated) {
		if (__connState == WIFI_CONNECTION_STATE_CONNECTED) {
			__lastState = CONNECTED;
			__getBssid();
		} else {
			__lastState = UNCONNECTED;
			__clearBssid();
		}
	} else {
		__lastState = DISABLED;
		__clearBssid();
	}

	if (!__isInitialized)
		wifi_deinitialize();

	return true;
}

bool DeviceStatusWifi::__getBssid()
{
	int err;
	char *strBuf = NULL;
	wifi_ap_h ap = NULL;

	err = wifi_get_connected_ap(&ap);
	IF_FAIL_RETURN_TAG(err == WIFI_ERROR_NONE, false, _E, "wifi_get_connected_ap() failed");

	wifi_ap_get_bssid(ap, &strBuf);
	__bssid = (strBuf != NULL ? strBuf : "");
	g_free(strBuf);

	wifi_ap_destroy(ap);

	if (__bssid.empty())
		_W("Failed to get BSSID");

	SharedVars().set(SharedVars::WIFI_BSSID, __bssid);
	_D("BSSID: %s", __bssid.c_str());

	return !__bssid.empty();
}

void DeviceStatusWifi::__clearBssid()
{
	__bssid.clear();
	SharedVars().clear(SharedVars::WIFI_BSSID);
	_D("No WiFi connection");
}

bool DeviceStatusWifi::__getResponsePacket(Json* data)
{
	switch (__lastState) {
	case DISABLED:
		data->set(NULL, KEY_STATE, VAL_DISABLED);
		break;

	case UNCONNECTED:
		data->set(NULL, KEY_STATE, VAL_UNCONNECTED);
		break;

	case CONNECTED:
		data->set(NULL, KEY_STATE, VAL_CONNECTED);
		data->set(NULL, KEY_BSSID, __bssid);
		break;

	default:
		return false;
	}

	return true;
}

int DeviceStatusWifi::read()
{
	IF_FAIL_RETURN(__getCurrentState(), ERR_OPERATION_FAILED);

	Json dataRead;
	if (__getResponsePacket(&dataRead)) {
		replyToRead(NULL, ERR_NONE, dataRead);
		return ERR_NONE;
	}

	return ERR_OPERATION_FAILED;
}

bool DeviceStatusWifi::__startMonitor()
{
	IF_FAIL_RETURN(!__isInitialized, true);

	int err;
	err = wifi_initialize();
	IF_FAIL_RETURN_TAG(err == WIFI_ERROR_NONE, false, _E, "wifi_initialize() failed");

	err = wifi_set_device_state_changed_cb(__deviceStateChangedCb, this);
	IF_FAIL_CATCH_TAG(err == WIFI_ERROR_NONE, _E, "wifi_set_device_state_changed_cb() failed");

	err = wifi_set_connection_state_changed_cb(__connectionStateChangedCb, this);
	IF_FAIL_CATCH_TAG(err == WIFI_ERROR_NONE, _E, "wifi_set_connection_state_changed_cb() failed");

	__isInitialized = true;
	return true;

CATCH:
	wifi_deinitialize();
	return false;
}

void DeviceStatusWifi::__stopMonitor()
{
	IF_FAIL_VOID(__isInitialized);

	wifi_unset_device_state_changed_cb();
	wifi_unset_connection_state_changed_cb();
	wifi_deinitialize();
	__isInitialized = false;
}

int DeviceStatusWifi::subscribe()
{
#if 0
	IF_FAIL_RETURN(__startMonitor(), ERR_OPERATION_FAILED);
	if (!__getCurrentState()) {
		__stopMonitor();
		return ERR_OPERATION_FAILED;
	}
#endif

	return ERR_NONE;
}

int DeviceStatusWifi::unsubscribe()
{
#if 0
	__stopMonitor();
#endif
	return ERR_NONE;
}

void DeviceStatusWifi::__handleUpdate()
{
	int prevState = __lastState;

	if (__isActivated) {
		if (__connState == WIFI_CONNECTION_STATE_CONNECTED) {
			__lastState = CONNECTED;
		} else {
			__lastState = UNCONNECTED;
		}
	} else {
		__lastState = DISABLED;
	}

	if (__lastState != prevState) {
		if (__lastState == CONNECTED) {
			__getBssid();
		} else {
			__clearBssid();
		}

		Json data;
		if (__beingSubscribed && __getResponsePacket(&data))
			publish(NULL, ERR_NONE, data);
	}
}

void DeviceStatusWifi::__deviceStateChangedCb(wifi_device_state_e state, void *userData)
{
	DeviceStatusWifi *instance = static_cast<DeviceStatusWifi*>(userData);
	instance->__isActivated = (state == WIFI_DEVICE_STATE_ACTIVATED);
	instance->__handleUpdate();
}

void DeviceStatusWifi::__connectionStateChangedCb(wifi_connection_state_e state, wifi_ap_h ap, void *userData)
{
	DeviceStatusWifi *instance = static_cast<DeviceStatusWifi*>(userData);
	instance->__connState = state;
	instance->__handleUpdate();
}
