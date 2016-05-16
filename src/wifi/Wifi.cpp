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

WifiStateProvider::WifiStateProvider() :
	BasicProvider(SUBJ_STATE_WIFI),
	__lastState(UNKNOWN),
	__isActivated(false),
	__connState(WIFI_CONNECTION_STATE_FAILURE)
{
	IF_FAIL_VOID_TAG(__startMonitor(), _W, "WiFi monitor initialization failed");

	if (!__getCurrentState()) {
		__stopMonitor();
		_W("Getting current WiFi status failed");
	}
}

WifiStateProvider::~WifiStateProvider()
{
	__stopMonitor();
}

void WifiStateProvider::getPrivilege(std::vector<const char*> &privilege)
{
	privilege.push_back(PRIV_NETWORK);
}

bool WifiStateProvider::isSupported()
{
	return util::getSystemInfoBool("tizen.org/feature/network.wifi");
}

bool WifiStateProvider::__getCurrentState()
{
	int err;

	err = __wrapper.isActivated(&__isActivated);
	IF_FAIL_RETURN_TAG(err == WIFI_ERROR_NONE, false, _E, "isActivated() failed");

	err = __wrapper.getConnectionState(&__connState);
	IF_FAIL_RETURN_TAG(err == WIFI_ERROR_NONE, false, _E, "getConnectionState() failed");

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

	return true;
}

bool WifiStateProvider::__getBssid()
{
	int err;
	char *strBuf = NULL;
	wifi_ap_h ap = NULL;

	err = __wrapper.getConnectedAP(&ap);
	IF_FAIL_RETURN_TAG(err == WIFI_ERROR_NONE, false, _E, "getConnectedAP() failed");

	__wrapper.getBssidFromAP(ap, &strBuf);
	__bssid = (strBuf != NULL ? strBuf : "");
	g_free(strBuf);

	__wrapper.destroyAP(ap);

	if (__bssid.empty())
		_W("Failed to get BSSID");

	SharedVars().set(SharedVars::WIFI_BSSID, __bssid);
	_D("BSSID: %s", __bssid.c_str());

	return !__bssid.empty();
}

void WifiStateProvider::__clearBssid()
{
	__bssid.clear();
	SharedVars().clear(SharedVars::WIFI_BSSID);
	_D("No WiFi connection");
}

bool WifiStateProvider::__getResponsePacket(Json* data)
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

int WifiStateProvider::read()
{
	IF_FAIL_RETURN(__getCurrentState(), ERR_OPERATION_FAILED);

	Json dataRead;
	if (__getResponsePacket(&dataRead)) {
		replyToRead(NULL, ERR_NONE, dataRead);
		return ERR_NONE;
	}

	return ERR_OPERATION_FAILED;
}

bool WifiStateProvider::__startMonitor()
{
	int err;

	err = __wrapper.setDeviceStateChangedCb(__deviceStateChangedCb, this);
	IF_FAIL_RETURN_TAG(err == WIFI_ERROR_NONE, false, _E, "setDeviceStateChangedCb() failed");

	err = __wrapper.setConnectionStateChangedCb(__connectionStateChangedCb, this);
	IF_FAIL_RETURN_TAG(err == WIFI_ERROR_NONE, false, _E, "setConnectionStateChangedCb() failed");

	return true;
}

void WifiStateProvider::__stopMonitor()
{
	__wrapper.unsetDeviceStateChangedCb();
	__wrapper.unsetConnectionStateChangedCb();
}

int WifiStateProvider::subscribe()
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

int WifiStateProvider::unsubscribe()
{
#if 0
	__stopMonitor();
#endif
	return ERR_NONE;
}

void WifiStateProvider::__handleUpdate()
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

void WifiStateProvider::__deviceStateChangedCb(wifi_device_state_e state, void *userData)
{
	WifiStateProvider *instance = static_cast<WifiStateProvider*>(userData);
	instance->__isActivated = (state == WIFI_DEVICE_STATE_ACTIVATED);
	instance->__handleUpdate();
}

void WifiStateProvider::__connectionStateChangedCb(wifi_connection_state_e state, wifi_ap_h ap, void *userData)
{
	WifiStateProvider *instance = static_cast<WifiStateProvider*>(userData);
	instance->__connState = state;
	instance->__handleUpdate();
}
