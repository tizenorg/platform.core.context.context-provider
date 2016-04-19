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

#include "Call.h"

#define TELEPHONY_NOTI_ID_CNT 8

using namespace ctx;

static bool __telephonyInitialized = false;
static telephony_noti_e __callNotiIds[] =
{
   TELEPHONY_NOTI_VOICE_CALL_STATUS_IDLE,
   TELEPHONY_NOTI_VOICE_CALL_STATUS_ACTIVE,
//   TELEPHONY_NOTI_VOICE_CALL_STATUS_HELD,
//   TELEPHONY_NOTI_VOICE_CALL_STATUS_DIALING,
   TELEPHONY_NOTI_VOICE_CALL_STATUS_ALERTING,
   TELEPHONY_NOTI_VOICE_CALL_STATUS_INCOMING,
   TELEPHONY_NOTI_VIDEO_CALL_STATUS_IDLE,
   TELEPHONY_NOTI_VIDEO_CALL_STATUS_ACTIVE,
//   TELEPHONY_NOTI_VIDEO_CALL_STATUS_DIALING,
   TELEPHONY_NOTI_VIDEO_CALL_STATUS_ALERTING,
   TELEPHONY_NOTI_VIDEO_CALL_STATUS_INCOMING,
};
static Json __latest;

SocialStatusCall::SocialStatusCall() :
	DeviceProviderBase(SOCIAL_ST_SUBJ_CALL)
{
	__handleList.count = 0;
	__handleList.handle = NULL;
}

SocialStatusCall::~SocialStatusCall()
{
}

bool SocialStatusCall::isSupported()
{
	return getSystemInfoBool("tizen.org/feature/network.telephony");
}

void SocialStatusCall::submitTriggerItem()
{
	registerTriggerItem(OPS_SUBSCRIBE | OPS_READ,
			"{"
				"\"Medium\":{\"type\":\"string\",\"values\":[\"Voice\",\"Video\"]},"
				"\"State\":{\"type\":\"string\",\"values\":[\"Idle\",\"Connecting\",\"Connected\"]},"
				"\"Address\":{\"type\":\"string\"}"
			"}",
			NULL);
	/* TODO remove Connecting, Connected */
}

void SocialStatusCall::__updateCb(telephony_h handle, telephony_noti_e notiId, void *data, void *userData)
{
	SocialStatusCall *instance = static_cast<SocialStatusCall*>(userData);
	instance->__handleUpdate(handle, notiId, data);
}

void SocialStatusCall::__handleUpdate(telephony_h handle, telephony_noti_e notiId, void* id)
{
	Json data;
	unsigned int count;
	telephony_call_h *callList;

	// Call state
	switch (notiId) {
	case TELEPHONY_NOTI_VOICE_CALL_STATUS_IDLE:
	case TELEPHONY_NOTI_VIDEO_CALL_STATUS_IDLE:
		data.set(NULL, SOCIAL_ST_STATE, SOCIAL_ST_IDLE);
		break;
	case TELEPHONY_NOTI_VOICE_CALL_STATUS_ACTIVE:
	case TELEPHONY_NOTI_VIDEO_CALL_STATUS_ACTIVE:
		data.set(NULL, SOCIAL_ST_STATE, SOCIAL_ST_ACTIVE);
		break;
	case TELEPHONY_NOTI_VOICE_CALL_STATUS_ALERTING:
	case TELEPHONY_NOTI_VIDEO_CALL_STATUS_ALERTING:
		data.set(NULL, SOCIAL_ST_STATE, SOCIAL_ST_ALERTING);
		break;
	case TELEPHONY_NOTI_VOICE_CALL_STATUS_INCOMING:
	case TELEPHONY_NOTI_VIDEO_CALL_STATUS_INCOMING:
		data.set(NULL, SOCIAL_ST_STATE, SOCIAL_ST_INCOMING);
		break;
/*	// Ignore below cases
	case TELEPHONY_NOTI_VOICE_CALL_STATUS_HELD:
	case TELEPHONY_NOTI_VOICE_CALL_STATUS_DIALING:
	case TELEPHONY_NOTI_VIDEO_CALL_STATUS_DIALING:*/
	default:
		_E("Unkown noti id: %d", notiId);
		return;
	}

	// Call type
	switch (notiId) {
	case TELEPHONY_NOTI_VOICE_CALL_STATUS_IDLE:
	case TELEPHONY_NOTI_VOICE_CALL_STATUS_ACTIVE:
	case TELEPHONY_NOTI_VOICE_CALL_STATUS_ALERTING:
	case TELEPHONY_NOTI_VOICE_CALL_STATUS_INCOMING:
		data.set(NULL, SOCIAL_ST_TYPE, SOCIAL_ST_VOICE);
		break;
	case TELEPHONY_NOTI_VIDEO_CALL_STATUS_IDLE:
	case TELEPHONY_NOTI_VIDEO_CALL_STATUS_ACTIVE:
	case TELEPHONY_NOTI_VIDEO_CALL_STATUS_ALERTING:
	case TELEPHONY_NOTI_VIDEO_CALL_STATUS_INCOMING:
		data.set(NULL, SOCIAL_ST_TYPE, SOCIAL_ST_VIDEO);
		break;
/*	// Ignore below cases
	case TELEPHONY_NOTI_VOICE_CALL_STATUS_HELD:
	case TELEPHONY_NOTI_VOICE_CALL_STATUS_DIALING:
	case TELEPHONY_NOTI_VIDEO_CALL_STATUS_DIALING:*/
	default:
		_E("Unkown noti id: %d", notiId);
		return;
	}

	int err = telephony_call_get_call_list(handle, &count, &callList);
	IF_FAIL_VOID_TAG(err == TELEPHONY_ERROR_NONE, _E, "Getting call list failed");

	unsigned int callId = *static_cast<unsigned int *>(id);
	for (unsigned int i = 0; i < count; i++) {
		unsigned int tempId;
		// Handle id
		if (!__getCallHandleId(callList[i], tempId)) {
			continue;
		}

		if (callId != tempId) {
			continue;
		}

		// Address
		std::string address;
		if (__getCallAddress(callList[i], address)) {
			data.set(NULL, SOCIAL_ST_ADDRESS, address);
			break;
		}
	}

	if (__latest != data) {
		publish(NULL, ERR_NONE, data);
		__latest = data.str();
	}
	telephony_call_release_call_list(count, &callList);
}

bool SocialStatusCall::__initTelephony()
{
	IF_FAIL_RETURN(!__telephonyInitialized, true);

	int err = telephony_init(&__handleList);
	IF_FAIL_RETURN_TAG(err == TELEPHONY_ERROR_NONE, false, _E, "Initialization failed");

	__telephonyInitialized = true;
	return true;
}

void SocialStatusCall::__releaseTelephony()
{
	IF_FAIL_VOID(__telephonyInitialized);

	telephony_deinit(&__handleList);

	__telephonyInitialized = false;
}

bool SocialStatusCall::__setCallback()
{
	/* TODO: Consider dual-sim devices */
	IF_FAIL_RETURN_TAG(__initTelephony(), false, _E, "Initialization failed");

	int err;

	for (unsigned int i = 0; i < __handleList.count; i++) {
		for (unsigned int j = 0; j < TELEPHONY_NOTI_ID_CNT; j++) {
			err = telephony_set_noti_cb(__handleList.handle[i], __callNotiIds[j], __updateCb, this);
			IF_FAIL_CATCH(err == TELEPHONY_ERROR_NONE);
		}
	}

	return true;

CATCH:
	_E("Initialization failed");
	__releaseTelephony();
	return false;
}

void SocialStatusCall::__unsetCallback()
{
	for (unsigned int i = 0; i < __handleList.count; i++) {
		for (unsigned int j = 0; j < TELEPHONY_NOTI_ID_CNT; j++) {
			telephony_unset_noti_cb(__handleList.handle[i], __callNotiIds[j]);
		}
	}

	__releaseTelephony();
}

bool SocialStatusCall::__getCallState(telephony_call_h& handle, std::string& state)
{
	state.clear();

	telephony_call_status_e st;
	int err = telephony_call_get_status(handle, &st);
	IF_FAIL_RETURN_TAG(err == TELEPHONY_ERROR_NONE, false, _E, "Getting state failed");

	switch (st) {
	case TELEPHONY_CALL_STATUS_ACTIVE:
		state = SOCIAL_ST_ACTIVE;
		break;
	case TELEPHONY_CALL_STATUS_HELD:
		state = SOCIAL_ST_HELD;
		break;
	case TELEPHONY_CALL_STATUS_DIALING:
		state = SOCIAL_ST_DIALING;
		break;
	case TELEPHONY_CALL_STATUS_ALERTING:
		state = SOCIAL_ST_ALERTING;
		break;
	case TELEPHONY_CALL_STATUS_INCOMING:
		state = SOCIAL_ST_INCOMING;
		break;
	default:
		state = SOCIAL_ST_IDLE;
	}

	IF_FAIL_RETURN_TAG(!state.empty(), false, _W, "State is empty");

	return true;
}

bool SocialStatusCall::__getCallType(telephony_call_h& handle, std::string& type)
{
	type.clear();

	telephony_call_type_e t;
	int err = telephony_call_get_type(handle, &t);
	IF_FAIL_RETURN_TAG(err == TELEPHONY_ERROR_NONE, false, _E, "Getting type failed");

	switch (t) {
	case TELEPHONY_CALL_TYPE_VOICE:
		type = SOCIAL_ST_VOICE;
		break;
	case TELEPHONY_CALL_TYPE_VIDEO:
		type = SOCIAL_ST_VIDEO;
		break;
	default:
		_E("Unknown type: %d", t);
		return false;
	}

	IF_FAIL_RETURN_TAG(!type.empty(), false, _W, "Type is empty");

	return true;
}

bool SocialStatusCall::__getCallAddress(telephony_call_h& handle, std::string& address)
{
	address.clear();

	char* number = NULL;
	int err = telephony_call_get_number(handle, &number);
	IF_FAIL_RETURN_TAG(err == TELEPHONY_ERROR_NONE, false, _E, "Getting address failed");

	if (number) {
		address = number;
		g_free(number);
		number = NULL;
	}

	IF_FAIL_RETURN_TAG(!address.empty(), false, _W, "Address is empty");

	return true;
}

bool SocialStatusCall::__getCallHandleId(telephony_call_h& handle, unsigned int& id)
{
	int err = telephony_call_get_handle_id(handle, &id);
	IF_FAIL_RETURN_TAG(err == TELEPHONY_ERROR_NONE, false, _E, "Getting handle id failed");

	return true;
}

int SocialStatusCall::subscribe()
{
	bool ret = __setCallback();
	IF_FAIL_RETURN(ret, ERR_OPERATION_FAILED);
	return ERR_NONE;
}

int SocialStatusCall::unsubscribe()
{
	__unsetCallback();
	return ERR_NONE;
}

bool SocialStatusCall::__readCurrentStatus(telephony_h& handle, Json* data)
{
	unsigned int count = 0;
	telephony_call_h *callList = NULL;
	telephony_call_get_call_list(handle, &count, &callList);

	// Default data
	data->set(NULL, SOCIAL_ST_STATE, SOCIAL_ST_IDLE);

	// Held & Dialing are ignored
	for (unsigned int i = 0; i < count; i++) {
		// Call state
		std::string state;
		if (__getCallState(callList[i], state)) {
			// Skip Held & Dialing
			if (state.compare(SOCIAL_ST_HELD) == 0 || state.compare(SOCIAL_ST_DIALING) == 0)
				continue;

			data->set(NULL, SOCIAL_ST_STATE, state);
		}

		// Call type
		std::string type;
		if (__getCallType(callList[i], type)) {
			data->set(NULL, SOCIAL_ST_MEDIUM, type);
		}

		// Address
		std::string address;
		if (__getCallAddress(callList[i], address)) {
			data->set(NULL, SOCIAL_ST_ADDRESS, address);
		}

		if (state == SOCIAL_ST_ACTIVE) {
			break;
		}
	}

	telephony_call_release_call_list(count, &callList);
	return true;
}

int SocialStatusCall::read()
{
	bool temporaryHandle = false;
	if (!__telephonyInitialized) {
		IF_FAIL_RETURN(__initTelephony(), ERR_OPERATION_FAILED);
		temporaryHandle = true;
	}

	bool ret = true;
	Json data;
	data.set(NULL, SOCIAL_ST_STATE, SOCIAL_ST_IDLE);

	for (unsigned int i = 0; i < __handleList.count; i++) {
		telephony_sim_state_e state;
		int err = telephony_sim_get_state(__handleList.handle[i], &state);
		IF_FAIL_RETURN_TAG(err == TELEPHONY_ERROR_NONE, ERR_OPERATION_FAILED, _E, "Getting SIM status failed");

		if (state != TELEPHONY_SIM_STATE_AVAILABLE)
			continue;

		ret = __readCurrentStatus(__handleList.handle[i], &data);
		break;
	}

	if (temporaryHandle)
		__releaseTelephony();

	if (ret) {
		replyToRead(NULL, ERR_NONE, data);
		return ERR_NONE;
	}

	return ERR_OPERATION_FAILED;
}
