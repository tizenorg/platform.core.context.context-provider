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

#include <ContextManager.h>
#include "SystemTypes.h"
#include "Headphone.h"

#define HANDLING_DELAY 2000
#define MAX_HANDLING_COUNT 3

GENERATE_PROVIDER_COMMON_IMPL(DeviceStatusHeadphone);

ctx::DeviceStatusHeadphone::DeviceStatusHeadphone() :
	__connected(false),
	__audioJackState(RUNTIME_INFO_AUDIO_JACK_STATUS_UNCONNECTED),
	__btAudioState(false),
	__btAudioCallbackOn(false),
	__btEventHandlerAdded(false),
	__btEventHandlingCount(0)
{
}

ctx::DeviceStatusHeadphone::~DeviceStatusHeadphone()
{
}

bool ctx::DeviceStatusHeadphone::isSupported()
{
	return true;
}

void ctx::DeviceStatusHeadphone::submitTriggerItem()
{
	context_manager::registerTriggerItem(DEVICE_ST_SUBJ_HEADPHONE, OPS_SUBSCRIBE | OPS_READ,
			"{"
				TRIG_BOOL_ITEM_DEF("IsConnected") ","
				"\"Type\":{\"type\":\"string\",\"values\":[\"Normal\",\"Headset\",\"Bluetooth\"]}"
			"}",
			NULL);
}

int ctx::DeviceStatusHeadphone::subscribe()
{
	__connected = __getCurrentStatus();

	// Wired headphone
	int ret = runtime_info_set_changed_cb(RUNTIME_INFO_KEY_AUDIO_JACK_STATUS, __onAudioJackStateChanged, this);
	IF_FAIL_RETURN(ret == RUNTIME_INFO_ERROR_NONE, ERR_OPERATION_FAILED);

	// Bluetooth headphone
	__setBtAudioCallback();

	return ERR_NONE;
}

int ctx::DeviceStatusHeadphone::unsubscribe()
{
	runtime_info_unset_changed_cb(RUNTIME_INFO_KEY_AUDIO_JACK_STATUS);
	__unsetBtAudioCallback();

	return ERR_NONE;
}

int ctx::DeviceStatusHeadphone::read()
{
	if (!__beingSubscribed)
		__connected = __getCurrentStatus();

	Json data;
	__generateDataPacket(&data);
	ctx::context_manager::replyToRead(DEVICE_ST_SUBJ_HEADPHONE, NULL, ERR_NONE, data);

	return ERR_NONE;
}

void ctx::DeviceStatusHeadphone::__setBtAudioCallback()
{
	IF_FAIL_VOID(!__btAudioCallbackOn);
	int ret;

	ret = bt_initialize();
	if (ret != BT_ERROR_NONE) {
		_W("Bluetooth initialization failed");
		return;
	}

	ret = bt_device_set_connection_state_changed_cb(__onBtConnectionChanged, this);
	if (ret != BT_ERROR_NONE) {
		bt_deinitialize();
		return;
	}

	__btAudioCallbackOn = true;
}

void ctx::DeviceStatusHeadphone::__unsetBtAudioCallback()
{
	IF_FAIL_VOID(__btAudioCallbackOn);

	bt_device_unset_connection_state_changed_cb();
	bt_deinitialize();

	__btAudioCallbackOn = false;
}

void ctx::DeviceStatusHeadphone::__setBtAudioState(bool state)
{
	__btAudioState = state;
}

bool ctx::DeviceStatusHeadphone::__getCurrentStatus()
{
	int ret;

	// Wired audio
	ret = runtime_info_get_value_int(RUNTIME_INFO_KEY_AUDIO_JACK_STATUS, &__audioJackState);
	IF_FAIL_RETURN(ret == ERR_NONE, __connected);

	// Bluetooth audio
	__btAudioState = false;
	ret = bt_initialize();
	if (ret == BT_ERROR_NONE) {
		bt_adapter_foreach_bonded_device(__onBtBond, this);
		bt_deinitialize();
	}

	return ((__audioJackState != RUNTIME_INFO_AUDIO_JACK_STATUS_UNCONNECTED) || __btAudioState);
}

void ctx::DeviceStatusHeadphone::__generateDataPacket(ctx::Json* data)
{
	data->set(NULL, DEVICE_ST_IS_CONNECTED, __connected ? DEVICE_ST_TRUE : DEVICE_ST_FALSE);

	switch (__audioJackState) {
	case RUNTIME_INFO_AUDIO_JACK_STATUS_CONNECTED_3WIRE:
		data->set(NULL, DEVICE_ST_TYPE, DEVICE_ST_NORMAL);
		break;
	case RUNTIME_INFO_AUDIO_JACK_STATUS_CONNECTED_4WIRE:
		data->set(NULL, DEVICE_ST_TYPE, DEVICE_ST_HEADSET);
		break;
	default:
		if (__btAudioState)
			data->set(NULL, DEVICE_ST_TYPE, DEVICE_ST_BLUETOOTH);
		break;
	}
}

bool ctx::DeviceStatusHeadphone::__handleUpdate()
{
	bool prevState = __connected;
	__connected = ((__audioJackState != RUNTIME_INFO_AUDIO_JACK_STATUS_UNCONNECTED) || __btAudioState);

	IF_FAIL_RETURN(prevState != __connected, false);

	ctx::Json data;
	__generateDataPacket(&data);
	ctx::context_manager::publish(DEVICE_ST_SUBJ_HEADPHONE, NULL, ERR_NONE, data);
	return true;
}

void ctx::DeviceStatusHeadphone::__handleAudioJackEvent()
{
	int ret = runtime_info_get_value_int(RUNTIME_INFO_KEY_AUDIO_JACK_STATUS, &__audioJackState);
	IF_FAIL_VOID_TAG(ret == ERR_NONE, _E, "Getting runtime info failed");
	__handleUpdate();
}

void ctx::DeviceStatusHeadphone::__onAudioJackStateChanged(runtime_info_key_e runtimeKey, void* userData)
{
	_D("EarJack");
	ctx::DeviceStatusHeadphone *instance = static_cast<ctx::DeviceStatusHeadphone*>(userData);
	instance->__handleAudioJackEvent();
}

void ctx::DeviceStatusHeadphone::__onBtConnectionChanged(bool connected, bt_device_connection_info_s *connInfo, void *userData)
{
	ctx::DeviceStatusHeadphone *instance = static_cast<ctx::DeviceStatusHeadphone*>(userData);
	IF_FAIL_VOID(connected != instance->__btAudioState);
	IF_FAIL_VOID(!instance->__btEventHandlerAdded);

	if (connected) {
		_D("BT state checking scheduled");
		instance->__btEventHandlerAdded = true;
		instance->__btEventHandlingCount = 0;
		g_timeout_add(HANDLING_DELAY, __handleBtEvent, userData);
	} else {
		__handleBtEvent(userData);
	}
}

gboolean ctx::DeviceStatusHeadphone::__handleBtEvent(gpointer data)
{
	_D("BT state checking started");
	ctx::DeviceStatusHeadphone *instance = static_cast<ctx::DeviceStatusHeadphone*>(data);
	instance->__btEventHandlerAdded = false;

	instance->__setBtAudioState(false);
	int err = bt_adapter_foreach_bonded_device(__onBtBond, data);
	IF_FAIL_RETURN_TAG(err == BT_ERROR_NONE, FALSE, _E, "bt_adapter_foreach_bonded_device() failed");

	instance->__btEventHandlingCount++;

	if (instance->__handleUpdate() || instance->__btEventHandlingCount >= MAX_HANDLING_COUNT)
		return FALSE;

	return TRUE;
}

bool ctx::DeviceStatusHeadphone::__onBtBond(bt_device_info_s *deviceInfo, void* userData)
{
	if (deviceInfo->bt_class.major_device_class != BT_MAJOR_DEVICE_CLASS_AUDIO_VIDEO)
		return true;

	bool st = false;
	int err = bt_device_is_profile_connected(deviceInfo->remote_address, BT_PROFILE_A2DP, &st);
	IF_FAIL_RETURN_TAG(err == BT_ERROR_NONE, false, _E, "bt_device_is_profile_connected() failed");

	if (st) {
		ctx::DeviceStatusHeadphone *instance = static_cast<ctx::DeviceStatusHeadphone*>(userData);
		instance->__setBtAudioState(true);
		return false;
	}

	return true;
}
