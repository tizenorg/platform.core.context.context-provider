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

#include <context_mgr.h>
#include "system_types.h"
#include "headphone.h"

#define HANDLING_DELAY 2000
#define MAX_HANDLING_COUNT 3

GENERATE_PROVIDER_COMMON_IMPL(device_status_headphone);

ctx::device_status_headphone::device_status_headphone()
	: connected(false)
	, audio_jack_state(RUNTIME_INFO_AUDIO_JACK_STATUS_UNCONNECTED)
	, bt_audio_state(false)
	, bt_audio_callback_on(false)
	, bt_event_handler_added(false)
	, bt_event_handling_count(0)
{
}

ctx::device_status_headphone::~device_status_headphone()
{
}

bool ctx::device_status_headphone::is_supported()
{
	return true;
}

void ctx::device_status_headphone::submit_trigger_item()
{
	context_manager::register_trigger_item(DEVICE_ST_SUBJ_HEADPHONE, OPS_SUBSCRIBE | OPS_READ,
			"{"
				TRIG_BOOL_ITEM_DEF("IsConnected") ","
				"\"Type\":{\"type\":\"string\",\"values\":[\"Normal\",\"Headset\",\"Bluetooth\"]}"
			"}",
			NULL);
}

int ctx::device_status_headphone::subscribe()
{
	connected = get_current_status();

	// Wired headphone
	int ret = runtime_info_set_changed_cb(RUNTIME_INFO_KEY_AUDIO_JACK_STATUS, on_audio_jack_state_changed, this);
	IF_FAIL_RETURN(ret == RUNTIME_INFO_ERROR_NONE, ERR_OPERATION_FAILED);

	// Bluetooth headphone
	set_bt_audio_callback();

	return ERR_NONE;
}

int ctx::device_status_headphone::unsubscribe()
{
	runtime_info_unset_changed_cb(RUNTIME_INFO_KEY_AUDIO_JACK_STATUS);
	unset_bt_audio_callback();

	return ERR_NONE;
}

int ctx::device_status_headphone::read()
{
	if (!being_subscribed)
		connected = get_current_status();

	Json data;
	generate_data_packet(data);
	ctx::context_manager::reply_to_read(DEVICE_ST_SUBJ_HEADPHONE, NULL, ERR_NONE, data);

	return ERR_NONE;
}

void ctx::device_status_headphone::set_bt_audio_callback()
{
	IF_FAIL_VOID(!bt_audio_callback_on);
	int ret;

	ret = bt_initialize();
	if (ret != BT_ERROR_NONE) {
		_W("Bluetooth initialization failed");
		return;
	}

	ret = bt_device_set_connection_state_changed_cb(on_bt_connection_changed, this);
	if (ret != BT_ERROR_NONE) {
		bt_deinitialize();
		return;
	}

	bt_audio_callback_on = true;
}

void ctx::device_status_headphone::unset_bt_audio_callback()
{
	IF_FAIL_VOID(bt_audio_callback_on);

	bt_device_unset_connection_state_changed_cb();
	bt_deinitialize();

	bt_audio_callback_on = false;
}

void ctx::device_status_headphone::set_bt_audio_state(bool state)
{
	bt_audio_state = state;
}

bool ctx::device_status_headphone::get_current_status()
{
	int ret;

	// Wired audio
	ret = runtime_info_get_value_int(RUNTIME_INFO_KEY_AUDIO_JACK_STATUS, &audio_jack_state);
	IF_FAIL_RETURN(ret == ERR_NONE, connected);

	// Bluetooth audio
	bt_audio_state = false;
	ret = bt_initialize();
	if (ret == BT_ERROR_NONE) {
		bt_adapter_foreach_bonded_device(on_bt_bond, this);
		bt_deinitialize();
	}

	return ((audio_jack_state != RUNTIME_INFO_AUDIO_JACK_STATUS_UNCONNECTED) || bt_audio_state);
}

void ctx::device_status_headphone::generate_data_packet(ctx::Json &data)
{
	data.set(NULL, DEVICE_ST_IS_CONNECTED, connected ? DEVICE_ST_TRUE : DEVICE_ST_FALSE);

	switch (audio_jack_state) {
	case RUNTIME_INFO_AUDIO_JACK_STATUS_CONNECTED_3WIRE:
		data.set(NULL, DEVICE_ST_TYPE, DEVICE_ST_NORMAL);
		break;
	case RUNTIME_INFO_AUDIO_JACK_STATUS_CONNECTED_4WIRE:
		data.set(NULL, DEVICE_ST_TYPE, DEVICE_ST_HEADSET);
		break;
	default:
		if (bt_audio_state)
			data.set(NULL, DEVICE_ST_TYPE, DEVICE_ST_BLUETOOTH);
		break;
	}
}

bool ctx::device_status_headphone::handle_event()
{
	bool prev_state = connected;
	connected = ((audio_jack_state != RUNTIME_INFO_AUDIO_JACK_STATUS_UNCONNECTED) || bt_audio_state);

	IF_FAIL_RETURN(prev_state != connected, false);

	ctx::Json data;
	generate_data_packet(data);
	ctx::context_manager::publish(DEVICE_ST_SUBJ_HEADPHONE, NULL, ERR_NONE, data);
	return true;
}

void ctx::device_status_headphone::handle_audio_jack_event()
{
	int ret = runtime_info_get_value_int(RUNTIME_INFO_KEY_AUDIO_JACK_STATUS, &audio_jack_state);
	IF_FAIL_VOID_TAG(ret == ERR_NONE, _E, "Getting runtime info failed");
	handle_event();
}

void ctx::device_status_headphone::on_audio_jack_state_changed(runtime_info_key_e runtime_key, void* user_data)
{
	_D("EarJack");
	ctx::device_status_headphone *instance = static_cast<ctx::device_status_headphone*>(user_data);
	instance->handle_audio_jack_event();
}

void ctx::device_status_headphone::on_bt_connection_changed(bool connected, bt_device_connection_info_s *conn_info, void *user_data)
{
	ctx::device_status_headphone *instance = static_cast<ctx::device_status_headphone*>(user_data);
	IF_FAIL_VOID(connected != instance->bt_audio_state);
	IF_FAIL_VOID(!instance->bt_event_handler_added);

	if (connected) {
		_D("BT state checking scheduled");
		instance->bt_event_handler_added = true;
		instance->bt_event_handling_count = 0;
		g_timeout_add(HANDLING_DELAY, handle_bt_event, user_data);
	} else {
		handle_bt_event(user_data);
	}
}

gboolean ctx::device_status_headphone::handle_bt_event(gpointer data)
{
	_D("BT state checking started");
	ctx::device_status_headphone *instance = static_cast<ctx::device_status_headphone*>(data);
	instance->bt_event_handler_added = false;

	instance->set_bt_audio_state(false);
	int err = bt_adapter_foreach_bonded_device(on_bt_bond, data);
	IF_FAIL_RETURN_TAG(err == BT_ERROR_NONE, FALSE, _E, "bt_adapter_foreach_bonded_device() failed");

	instance->bt_event_handling_count++;

	if (instance->handle_event() || instance->bt_event_handling_count >= MAX_HANDLING_COUNT)
		return FALSE;

	return TRUE;
}

bool ctx::device_status_headphone::on_bt_bond(bt_device_info_s *device_info, void* user_data)
{
	if (device_info->bt_class.major_device_class != BT_MAJOR_DEVICE_CLASS_AUDIO_VIDEO)
		return true;

	bool st = false;
	int err = bt_device_is_profile_connected(device_info->remote_address, BT_PROFILE_A2DP, &st);
	IF_FAIL_RETURN_TAG(err == BT_ERROR_NONE, false, _E, "bt_device_is_profile_connected() failed");

	if (st) {
		ctx::device_status_headphone *instance = static_cast<ctx::device_status_headphone*>(user_data);
		instance->set_bt_audio_state(true);
		return false;
	}

	return true;
}
