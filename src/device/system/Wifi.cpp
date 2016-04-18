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
#include <ContextManager.h>
#include "SystemTypes.h"
#include "Wifi.h"

ctx::DeviceStatusWifi *ctx::DeviceStatusWifi::__instance = NULL;

ctx::DeviceStatusWifi::DeviceStatusWifi() :
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

ctx::DeviceStatusWifi::~DeviceStatusWifi()
{
}

ctx::ContextProvider *ctx::DeviceStatusWifi::create(void *data)
{
	CREATE_INSTANCE(DeviceStatusWifi);
}

void ctx::DeviceStatusWifi::destroy(void *data)
{
	__instance->__stopMonitor();
	DESTROY_INSTANCE();
}

void ctx::DeviceStatusWifi::destroySelf()
{
	/* WiFi status will be monitored continuously, even if no client is subscribing it */
}

bool ctx::DeviceStatusWifi::isSupported()
{
	return getSystemInfoBool("tizen.org/feature/network.wifi");
}

void ctx::DeviceStatusWifi::submitTriggerItem()
{
	context_manager::registerTriggerItem(DEVICE_ST_SUBJ_WIFI, OPS_SUBSCRIBE | OPS_READ,
			"{"
				"\"State\":{\"type\":\"string\",\"values\":[\"Disabled\",\"Unconnected\",\"Connected\"]},"
				"\"BSSID\":{\"type\":\"string\"}"
			"}",
			NULL);
}

bool ctx::DeviceStatusWifi::__getCurrentState()
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

bool ctx::DeviceStatusWifi::__getBssid()
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

	SharedVars().set(ctx::SharedVars::WIFI_BSSID, __bssid);
	_D("BSSID: %s", __bssid.c_str());

	return !__bssid.empty();
}

void ctx::DeviceStatusWifi::__clearBssid()
{
	__bssid.clear();
	SharedVars().clear(ctx::SharedVars::WIFI_BSSID);
	_D("No WiFi connection");
}

bool ctx::DeviceStatusWifi::__getResponsePacket(ctx::Json* data)
{
	switch (__lastState) {
	case DISABLED:
		data->set(NULL, DEVICE_ST_STATE, DEVICE_ST_DISABLED);
		break;

	case UNCONNECTED:
		data->set(NULL, DEVICE_ST_STATE, DEVICE_ST_UNCONNECTED);
		break;

	case CONNECTED:
		data->set(NULL, DEVICE_ST_STATE, DEVICE_ST_CONNECTED);
		data->set(NULL, DEVICE_ST_BSSID, __bssid);
		break;

	default:
		return false;
	}

	return true;
}

int ctx::DeviceStatusWifi::read()
{
	IF_FAIL_RETURN(__getCurrentState(), ERR_OPERATION_FAILED);

	ctx::Json dataRead;
	if (__getResponsePacket(&dataRead)) {
		ctx::context_manager::replyToRead(DEVICE_ST_SUBJ_WIFI, NULL, ERR_NONE, dataRead);
		return ERR_NONE;
	}

	return ERR_OPERATION_FAILED;
}

bool ctx::DeviceStatusWifi::__startMonitor()
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

void ctx::DeviceStatusWifi::__stopMonitor()
{
	IF_FAIL_VOID(__isInitialized);

	wifi_unset_device_state_changed_cb();
	wifi_unset_connection_state_changed_cb();
	wifi_deinitialize();
	__isInitialized = false;
}

int ctx::DeviceStatusWifi::subscribe()
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

int ctx::DeviceStatusWifi::unsubscribe()
{
#if 0
	__stopMonitor();
#endif
	return ERR_NONE;
}

void ctx::DeviceStatusWifi::__handleUpdate()
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

		ctx::Json data;
		if (__beingSubscribed && __getResponsePacket(&data))
			context_manager::publish(DEVICE_ST_SUBJ_WIFI, NULL, ERR_NONE, data);
	}
}

void ctx::DeviceStatusWifi::__deviceStateChangedCb(wifi_device_state_e state, void *userData)
{
	DeviceStatusWifi *instance = static_cast<DeviceStatusWifi*>(userData);
	instance->__isActivated = (state == WIFI_DEVICE_STATE_ACTIVATED);
	instance->__handleUpdate();
}

void ctx::DeviceStatusWifi::__connectionStateChangedCb(wifi_connection_state_e state, wifi_ap_h ap, void *userData)
{
	DeviceStatusWifi *instance = static_cast<DeviceStatusWifi*>(userData);
	instance->__connState = state;
	instance->__handleUpdate();
}
