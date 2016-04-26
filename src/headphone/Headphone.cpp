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

#include "Headphone.h"

#define HANDLING_DELAY 2000
#define MAX_HANDLING_COUNT 3

using namespace ctx;

HeadphoneStateProvider::HeadphoneStateProvider() :
	BasicProvider(SUBJ_STATE_HEADPHONE),
	__connected(false),
	__audioJackState(RUNTIME_INFO_AUDIO_JACK_STATUS_UNCONNECTED),
	__btAudioState(false),
	__btAudioCallbackOn(false),
	__btEventHandlerAdded(false),
	__btEventHandlingCount(0)
{
}

HeadphoneStateProvider::~HeadphoneStateProvider()
{
}

bool HeadphoneStateProvider::isSupported()
{
	return true;
}

int HeadphoneStateProvider::subscribe()
{
	__connected = __getCurrentStatus();

	// Wired headphone
	int ret = runtime_info_set_changed_cb(RUNTIME_INFO_KEY_AUDIO_JACK_STATUS, __onAudioJackStateChanged, this);
	IF_FAIL_RETURN(ret == RUNTIME_INFO_ERROR_NONE, ERR_OPERATION_FAILED);

	// Bluetooth headphone
	__setBtAudioCallback();

	return ERR_NONE;
}

int HeadphoneStateProvider::unsubscribe()
{
	runtime_info_unset_changed_cb(RUNTIME_INFO_KEY_AUDIO_JACK_STATUS);
	__unsetBtAudioCallback();

	return ERR_NONE;
}

int HeadphoneStateProvider::read()
{
	if (!__beingSubscribed)
		__connected = __getCurrentStatus();

	Json data;
	__generateDataPacket(&data);
	replyToRead(NULL, ERR_NONE, data);

	return ERR_NONE;
}

void HeadphoneStateProvider::__setBtAudioCallback()
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

void HeadphoneStateProvider::__unsetBtAudioCallback()
{
	IF_FAIL_VOID(__btAudioCallbackOn);

	bt_device_unset_connection_state_changed_cb();
	bt_deinitialize();

	__btAudioCallbackOn = false;
}

void HeadphoneStateProvider::__setBtAudioState(bool state)
{
	__btAudioState = state;
}

bool HeadphoneStateProvider::__getCurrentStatus()
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

void HeadphoneStateProvider::__generateDataPacket(Json* data)
{
	data->set(NULL, KEY_IS_CONNECTED, __connected ? VAL_TRUE : VAL_FALSE);

	switch (__audioJackState) {
	case RUNTIME_INFO_AUDIO_JACK_STATUS_CONNECTED_3WIRE:
		data->set(NULL, KEY_TYPE, VAL_NORMAL);
		break;
	case RUNTIME_INFO_AUDIO_JACK_STATUS_CONNECTED_4WIRE:
		data->set(NULL, KEY_TYPE, VAL_HEADSET);
		break;
	default:
		if (__btAudioState)
			data->set(NULL, KEY_TYPE, VAL_BLUETOOTH);
		break;
	}
}

bool HeadphoneStateProvider::__handleUpdate()
{
	bool prevState = __connected;
	__connected = ((__audioJackState != RUNTIME_INFO_AUDIO_JACK_STATUS_UNCONNECTED) || __btAudioState);

	IF_FAIL_RETURN(prevState != __connected, false);

	Json data;
	__generateDataPacket(&data);
	publish(NULL, ERR_NONE, data);
	return true;
}

void HeadphoneStateProvider::__handleAudioJackEvent()
{
	int ret = runtime_info_get_value_int(RUNTIME_INFO_KEY_AUDIO_JACK_STATUS, &__audioJackState);
	IF_FAIL_VOID_TAG(ret == ERR_NONE, _E, "Getting runtime info failed");
	__handleUpdate();
}

void HeadphoneStateProvider::__onAudioJackStateChanged(runtime_info_key_e runtimeKey, void* userData)
{
	_D("EarJack");
	HeadphoneStateProvider *instance = static_cast<HeadphoneStateProvider*>(userData);
	instance->__handleAudioJackEvent();
}

void HeadphoneStateProvider::__onBtConnectionChanged(bool connected, bt_device_connection_info_s *connInfo, void *userData)
{
	HeadphoneStateProvider *instance = static_cast<HeadphoneStateProvider*>(userData);
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

gboolean HeadphoneStateProvider::__handleBtEvent(gpointer data)
{
	_D("BT state checking started");
	HeadphoneStateProvider *instance = static_cast<HeadphoneStateProvider*>(data);
	instance->__btEventHandlerAdded = false;

	instance->__setBtAudioState(false);
	int err = bt_adapter_foreach_bonded_device(__onBtBond, data);
	IF_FAIL_RETURN_TAG(err == BT_ERROR_NONE, FALSE, _E, "bt_adapter_foreach_bonded_device() failed");

	instance->__btEventHandlingCount++;

	if (instance->__handleUpdate() || instance->__btEventHandlingCount >= MAX_HANDLING_COUNT)
		return FALSE;

	return TRUE;
}

bool HeadphoneStateProvider::__onBtBond(bt_device_info_s *deviceInfo, void* userData)
{
	if (deviceInfo->bt_class.major_device_class != BT_MAJOR_DEVICE_CLASS_AUDIO_VIDEO)
		return true;

	bool st = false;
	int err = bt_device_is_profile_connected(deviceInfo->remote_address, BT_PROFILE_A2DP, &st);
	IF_FAIL_RETURN_TAG(err == BT_ERROR_NONE, false, _E, "bt_device_is_profile_connected() failed");

	if (st) {
		HeadphoneStateProvider *instance = static_cast<HeadphoneStateProvider*>(userData);
		instance->__setBtAudioState(true);
		return false;
	}

	return true;
}
