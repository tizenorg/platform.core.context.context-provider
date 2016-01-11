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

#include <shared_vars.h>
#include <context_mgr.h>
#include "system_types.h"
#include "wifi.h"

ctx::device_status_wifi *ctx::device_status_wifi::__instance = NULL;

ctx::device_status_wifi::device_status_wifi()
	: last_state(_UNKNOWN)
	, is_initialized(false)
	, is_activated(false)
{
	IF_FAIL_VOID_TAG(start_monitor(), _W, "WiFi monitor initialization failed");

	if (!get_current_state()) {
		stop_monitor();
		_W("Getting current WiFi status failed");
	}
}

ctx::device_status_wifi::~device_status_wifi()
{
}

ctx::context_provider_iface *ctx::device_status_wifi::create(void *data)
{
	CREATE_INSTANCE(device_status_wifi);
}

void ctx::device_status_wifi::destroy(void *data)
{
	__instance->stop_monitor();
	DESTROY_INSTANCE();
}

void ctx::device_status_wifi::destroy_self()
{
	/* WiFi status will be monitored continuously, even if no client is subscribing it */
}

bool ctx::device_status_wifi::is_supported()
{
	return get_system_info_bool("tizen.org/feature/network.wifi");
}

void ctx::device_status_wifi::submit_trigger_item()
{
	context_manager::register_trigger_item(DEVICE_ST_SUBJ_WIFI, OPS_SUBSCRIBE | OPS_READ,
			"{"
				"\"State\":{\"type\":\"string\",\"values\":[\"Disabled\",\"Unconnected\",\"Connected\"]},"
				"\"BSSID\":{\"type\":\"string\"}"
			"}",
			NULL);
}

bool ctx::device_status_wifi::get_current_state()
{
	int err;

	if (!is_initialized) {
		err = wifi_initialize();
		IF_FAIL_RETURN_TAG(err == WIFI_ERROR_NONE, false, _E, "wifi_initialize() failed");
	}

	err = wifi_is_activated(&is_activated);
	IF_FAIL_RETURN_TAG(err == WIFI_ERROR_NONE, false, _E, "wifi_is_activated() failed");

	err = wifi_get_connection_state(&conn_state);
	IF_FAIL_RETURN_TAG(err == WIFI_ERROR_NONE, false, _E, "wifi_get_connection_state() failed");

	if (is_activated) {
		if (conn_state == WIFI_CONNECTION_STATE_CONNECTED) {
			last_state = _CONNECTED;
			get_bssid();
		} else {
			last_state = _UNCONNECTED;
			clear_bssid();
		}
	} else {
		last_state = _DISABLED;
		clear_bssid();
	}

	if (!is_initialized)
		wifi_deinitialize();

	return true;
}

bool ctx::device_status_wifi::get_bssid()
{
	int err;
	char *str_buf = NULL;
	wifi_ap_h ap = NULL;

	err = wifi_get_connected_ap(&ap);
	IF_FAIL_RETURN_TAG(err == WIFI_ERROR_NONE, false, _E, "wifi_get_connected_ap() failed");

	wifi_ap_get_bssid(ap, &str_buf);
	bssid = (str_buf != NULL ? str_buf : "");
	g_free(str_buf);

	wifi_ap_destroy(ap);

	if (bssid.empty())
		_W("Failed to get BSSID");

	ctx::shared::wifi_bssid = bssid;
	_D("BSSID: %s", bssid.c_str());

	return !bssid.empty();
}

void ctx::device_status_wifi::clear_bssid()
{
	bssid.clear();
	ctx::shared::wifi_bssid.clear();
	_D("No WiFi connection");
}

bool ctx::device_status_wifi::get_response_packet(ctx::json &data)
{
	switch (last_state) {
	case _DISABLED:
		data.set(NULL, DEVICE_ST_STATE, DEVICE_ST_DISABLED);
		break;

	case _UNCONNECTED:
		data.set(NULL, DEVICE_ST_STATE, DEVICE_ST_UNCONNECTED);
		break;

	case _CONNECTED:
		data.set(NULL, DEVICE_ST_STATE, DEVICE_ST_CONNECTED);
		data.set(NULL, DEVICE_ST_BSSID, bssid);
		break;

	default:
		return false;
	}

	return true;
}

int ctx::device_status_wifi::read()
{
	IF_FAIL_RETURN(get_current_state(), ERR_OPERATION_FAILED);

	ctx::json data_read;

	if (get_response_packet(data_read)) {
		ctx::context_manager::reply_to_read(DEVICE_ST_SUBJ_WIFI, NULL, ERR_NONE, data_read);
		return ERR_NONE;
	}

	return ERR_OPERATION_FAILED;
}

bool ctx::device_status_wifi::start_monitor()
{
	IF_FAIL_RETURN(!is_initialized, true);

	int err;
	err = wifi_initialize();
	IF_FAIL_RETURN_TAG(err == WIFI_ERROR_NONE, false, _E, "wifi_initialize() failed");

	err = wifi_set_device_state_changed_cb(device_state_changed_cb, this);
	IF_FAIL_CATCH_TAG(err == WIFI_ERROR_NONE, _E, "wifi_set_device_state_changed_cb() failed");

	err = wifi_set_connection_state_changed_cb(connection_state_changed_cb, this);
	IF_FAIL_CATCH_TAG(err == WIFI_ERROR_NONE, _E, "wifi_set_connection_state_changed_cb() failed");

	is_initialized = true;
	return true;

CATCH:
	wifi_deinitialize();
	return false;
}

void ctx::device_status_wifi::stop_monitor()
{
	IF_FAIL_VOID(is_initialized);

	wifi_unset_device_state_changed_cb();
	wifi_unset_connection_state_changed_cb();
	wifi_deinitialize();
	is_initialized = false;
}

int ctx::device_status_wifi::subscribe()
{
#if 0
	IF_FAIL_RETURN(start_monitor(), ERR_OPERATION_FAILED);
	if (!get_current_state()) {
		stop_monitor();
		return ERR_OPERATION_FAILED;
	}
#endif

	return ERR_NONE;
}

int ctx::device_status_wifi::unsubscribe()
{
#if 0
	stop_monitor();
#endif
	return ERR_NONE;
}

void ctx::device_status_wifi::aggregate_updated_data()
{
	int prev_state = last_state;

	if (is_activated) {
		if (conn_state == WIFI_CONNECTION_STATE_CONNECTED) {
			last_state = _CONNECTED;
		} else {
			last_state = _UNCONNECTED;
		}
	} else {
		last_state = _DISABLED;
	}

	if (last_state != prev_state) {
		if (last_state == _CONNECTED) {
			get_bssid();
		} else {
			clear_bssid();
		}

		ctx::json data;
		if (being_subscribed && get_response_packet(data))
			context_manager::publish(DEVICE_ST_SUBJ_WIFI, NULL, ERR_NONE, data);
	}
}

void ctx::device_status_wifi::device_state_changed_cb(wifi_device_state_e state, void *user_data)
{
	device_status_wifi *instance = static_cast<device_status_wifi*>(user_data);
	instance->is_activated = (state == WIFI_DEVICE_STATE_ACTIVATED);
	instance->aggregate_updated_data();
}

void ctx::device_status_wifi::connection_state_changed_cb(wifi_connection_state_e state, wifi_ap_h ap, void *user_data)
{
	device_status_wifi *instance = static_cast<device_status_wifi*>(user_data);
	instance->conn_state = state;
	instance->aggregate_updated_data();
}
