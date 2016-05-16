/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd.
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
#include "WifiWrapper.h"

#define SET_CALLBACK(type, setFunc) \
	__cb##type = callback; \
	__cbData##type = userData; \
	if (__enabled##type) \
		return WIFI_ERROR_NONE; \
	int err = setFunc(__cbFunc##type, NULL); \
	if (err == WIFI_ERROR_NONE) \
		__enabled##type = true; \
	return err;

#define UNSET_CALLBACK(type, unsetFunc) \
	__cb##type = NULL; \
	__cbData##type = NULL; \
	if (!__enabled##type) \
		return WIFI_ERROR_NONE; \
	for (auto it : __instances) { \
		if (it->__cb##type != NULL) \
			return WIFI_ERROR_NONE; \
	} \
	unsetFunc(); \
	return WIFI_ERROR_NONE;

using namespace ctx;

std::set<WifiWrapper*> WifiWrapper::__instances;
bool WifiWrapper::__enabledDeviceState = false;
bool WifiWrapper::__enabledConnectionState = false;
bool WifiWrapper::__enabledBackgroundScan = false;

SO_EXPORT WifiWrapper::WifiWrapper() :
	__cbDeviceState(NULL),
	__cbConnectionState(NULL),
	__cbBackgroundScan(NULL),
	__cbDataDeviceState(NULL),
	__cbDataConnectionState(NULL),
	__cbDataBackgroundScan(NULL)
{
	if (__instances.empty())
		__init();

	__instances.insert(this);

	_D("#instances = %d", __instances.size());
}

SO_EXPORT WifiWrapper::~WifiWrapper()
{
	if (__cbDeviceState)
		unsetDeviceStateChangedCb();

	if (__cbConnectionState)
		unsetConnectionStateChangedCb();

	if (__cbBackgroundScan)
		unsetBackgroundScanCb();

	__instances.erase(this);

	if (__instances.empty())
		__release();

	_D("#instances = %d", __instances.size());
}

SO_EXPORT int WifiWrapper::isActivated(bool *activated)
{
	return wifi_is_activated(activated);
}

SO_EXPORT int WifiWrapper::getConnectionState(wifi_connection_state_e *state)
{
	return wifi_get_connection_state(state);
}

SO_EXPORT int WifiWrapper::getConnectedAP(wifi_ap_h *ap)
{
	return wifi_get_connected_ap(ap);
}

SO_EXPORT int WifiWrapper::getBssidFromAP(wifi_ap_h ap, char **bssid)
{
	return wifi_ap_get_bssid(ap, bssid);
}

SO_EXPORT int WifiWrapper::destroyAP(wifi_ap_h ap)
{
	return wifi_ap_destroy(ap);
}

SO_EXPORT int WifiWrapper::setDeviceStateChangedCb(wifi_device_state_changed_cb callback, void *userData)
{
	SET_CALLBACK(DeviceState, wifi_set_device_state_changed_cb)
}

SO_EXPORT int WifiWrapper::unsetDeviceStateChangedCb()
{
	UNSET_CALLBACK(DeviceState, wifi_unset_device_state_changed_cb)
}

SO_EXPORT int WifiWrapper::setConnectionStateChangedCb(wifi_connection_state_changed_cb callback, void *userData)
{
	SET_CALLBACK(ConnectionState, wifi_set_connection_state_changed_cb)
}

SO_EXPORT int WifiWrapper::unsetConnectionStateChangedCb()
{
	UNSET_CALLBACK(ConnectionState, wifi_unset_connection_state_changed_cb)
}

SO_EXPORT int WifiWrapper::setBackgroundScanCb(wifi_scan_finished_cb callback, void *userData)
{
	SET_CALLBACK(BackgroundScan, wifi_set_background_scan_cb)
}

SO_EXPORT int WifiWrapper::unsetBackgroundScanCb()
{
	UNSET_CALLBACK(BackgroundScan, wifi_unset_background_scan_cb)
}

SO_EXPORT int WifiWrapper::scan(wifi_scan_finished_cb callback, void *userData)
{
	return wifi_scan(callback, userData);
}

SO_EXPORT int WifiWrapper::foreachFoundAP(wifi_found_ap_cb callback, void *userData)
{
	return wifi_foreach_found_aps(callback, userData);
}

void WifiWrapper::__init()
{
	_D("Initialize");
	int err = wifi_initialize();
	IF_FAIL_VOID_TAG(err == WIFI_ERROR_NONE, _E, "wifi_initialize() failed");
}

void WifiWrapper::__release()
{
	_D("Deinitialize");
	wifi_deinitialize();
}

void WifiWrapper::__cbFuncDeviceState(wifi_device_state_e state, void *userData)
{
	for (auto it : __instances) {
		if (it->__cbDeviceState)
			it->__cbDeviceState(state, it->__cbDataDeviceState);
	}
}

void WifiWrapper::__cbFuncConnectionState(wifi_connection_state_e state, wifi_ap_h ap, void *userData)
{
	for (auto it : __instances) {
		if (it->__cbConnectionState)
			it->__cbConnectionState(state, ap, it->__cbDataConnectionState);
	}
}

void WifiWrapper::__cbFuncBackgroundScan(wifi_error_e error_code, void *userData)
{
	for (auto it : __instances) {
		if (it->__cbBackgroundScan)
			it->__cbBackgroundScan(error_code, it->__cbDataBackgroundScan);
	}
}
