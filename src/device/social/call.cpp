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

#include <stdlib.h>
#include <json.h>
#include <context_mgr.h>
#include "social_types.h"
#include "call.h"

#define TELEPHONY_NOTI_ID_CNT 8
GENERATE_PROVIDER_COMMON_IMPL(social_status_call);

static bool telephony_initialized = false;
static telephony_noti_e call_noti_ids[] =
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
static ctx::json latest;

ctx::social_status_call::social_status_call()
{
	handle_list.count = 0;
	handle_list.handle = NULL;
}

ctx::social_status_call::~social_status_call()
{
}

bool ctx::social_status_call::is_supported()
{
	return get_system_info_bool("tizen.org/feature/network.telephony");
}

void ctx::social_status_call::submit_trigger_item()
{
	context_manager::register_trigger_item(SOCIAL_ST_SUBJ_CALL, OPS_SUBSCRIBE | OPS_READ,
			"{"
				"\"Medium\":{\"type\":\"string\",\"values\":[\"Voice\",\"Video\"]},"
				"\"State\":{\"type\":\"string\",\"values\":[\"Idle\",\"Connecting\",\"Connected\"]},"
				"\"Address\":{\"type\":\"string\"}"
			"}",
			NULL);
	//TODO remove Connecting, Connected
}

void ctx::social_status_call::call_event_cb(telephony_h handle, telephony_noti_e noti_id, void *data, void *user_data)
{
	social_status_call *instance = static_cast<social_status_call*>(user_data);
	instance->handle_call_event(handle, noti_id, data);
}

void ctx::social_status_call::handle_call_event(telephony_h handle, telephony_noti_e noti_id, void* id)
{

	json data;
	unsigned int count;
	telephony_call_h *call_list;

	// Call state
	switch (noti_id) {
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
		_E("Unkown noti id: %d", noti_id);
		return;
	}

	// Call type
	switch (noti_id) {
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
		_E("Unkown noti id: %d", noti_id);
		return;
	}

	int err = telephony_call_get_call_list(handle, &count, &call_list);
	IF_FAIL_VOID_TAG(err == TELEPHONY_ERROR_NONE, _E, "Getting call list failed");

	unsigned int call_id = *static_cast<unsigned int *>(id);
	for (unsigned int i = 0; i < count; i++) {
		unsigned int tmp_id;
		// Handle id
		if (!get_call_handle_id(call_list[i], tmp_id)) {
			continue;
		}

		if (call_id != tmp_id) {
			continue;
		}

		// Address
		std::string address;
		if (get_call_address(call_list[i], address)) {
			data.set(NULL, SOCIAL_ST_ADDRESS, address);
			break;
		}
	}

	if (latest != data) {
		context_manager::publish(SOCIAL_ST_SUBJ_CALL, NULL, ERR_NONE, data);
		latest = data.str();
	}
	telephony_call_release_call_list(count, &call_list);
}

bool ctx::social_status_call::init_telephony()
{
	IF_FAIL_RETURN(!telephony_initialized, true);

	int err = telephony_init(&handle_list);
	IF_FAIL_RETURN_TAG(err == TELEPHONY_ERROR_NONE, false, _E, "Initialization failed");

	telephony_initialized = true;
	return true;
}

void ctx::social_status_call::release_telephony()
{
	IF_FAIL_VOID(telephony_initialized);

	telephony_deinit(&handle_list);

	telephony_initialized = false;
}

bool ctx::social_status_call::set_callback()
{
	//TODO: Consider dual-sim devices
	IF_FAIL_RETURN_TAG(init_telephony(), false, _E, "Initialization failed");

	int err;

	for (unsigned int i = 0; i < handle_list.count; i++) {
		for (unsigned int j = 0; j < TELEPHONY_NOTI_ID_CNT; j++) {
			err = telephony_set_noti_cb(handle_list.handle[i], call_noti_ids[j], call_event_cb, this);
			IF_FAIL_CATCH(err == TELEPHONY_ERROR_NONE);
		}
	}

	return true;

CATCH:
	_E("Initialization failed");
	release_telephony();
	return false;
}

void ctx::social_status_call::unset_callback()
{
	for (unsigned int i = 0; i < handle_list.count; i++) {
		for (unsigned int j = 0; j < TELEPHONY_NOTI_ID_CNT; j++) {
			telephony_unset_noti_cb(handle_list.handle[i], call_noti_ids[j]);
		}
	}

	release_telephony();
}

bool ctx::social_status_call::get_call_state(telephony_call_h& handle, std::string& state)
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

bool ctx::social_status_call::get_call_type(telephony_call_h& handle, std::string& type)
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

bool ctx::social_status_call::get_call_address(telephony_call_h& handle, std::string& address)
{
	address.clear();

	char* number = NULL;
	int err = telephony_call_get_number(handle, &number);
	IF_FAIL_RETURN_TAG(err == TELEPHONY_ERROR_NONE, false, _E, "Getting address failed");

	if (number) {
		address = number;
		free(number);
		number = NULL;
	}

	IF_FAIL_RETURN_TAG(!address.empty(), false, _W, "Address is empty");

	return true;
}

bool ctx::social_status_call::get_call_handle_id(telephony_call_h& handle, unsigned int& id)
{
	int err = telephony_call_get_handle_id(handle, &id);
	IF_FAIL_RETURN_TAG(err == TELEPHONY_ERROR_NONE, false, _E, "Getting handle id failed");

	return true;
}

int ctx::social_status_call::subscribe()
{
	bool ret = set_callback();
	IF_FAIL_RETURN(ret, ERR_OPERATION_FAILED);
	return ERR_NONE;
}

int ctx::social_status_call::unsubscribe()
{
	unset_callback();
	return ERR_NONE;
}

bool ctx::social_status_call::read_current_status(telephony_h& handle, ctx::json& data)
{
	unsigned int count = 0;
	telephony_call_h *call_list = NULL;
	telephony_call_get_call_list(handle, &count, &call_list);

	// Default data
	data.set(NULL, SOCIAL_ST_STATE, SOCIAL_ST_IDLE);

	// Held & Dialing are ignored
	for (unsigned int i = 0; i < count; i++) {
		// Call state
		std::string state;
		if (get_call_state(call_list[i], state)) {
			// Skip Held & Dialing
			if (state.compare(SOCIAL_ST_HELD) == 0 || state.compare(SOCIAL_ST_DIALING) == 0)
				continue;

			data.set(NULL, SOCIAL_ST_STATE, state);
		}

		// Call type
		std::string type;
		if (get_call_type(call_list[i], type)) {
			data.set(NULL, SOCIAL_ST_MEDIUM, type);
		}

		// Address
		std::string address;
		if (get_call_address(call_list[i], address)) {
			data.set(NULL, SOCIAL_ST_ADDRESS, address);
		}

		if (state == SOCIAL_ST_ACTIVE) {
			break;
		}
	}

	telephony_call_release_call_list(count, &call_list);
	return true;
}

int ctx::social_status_call::read()
{
	bool temporary_handle = false;
	if (!telephony_initialized) {
		IF_FAIL_RETURN(init_telephony(), ERR_OPERATION_FAILED);
		temporary_handle = true;
	}

	bool ret = true;
	json data;
	data.set(NULL, SOCIAL_ST_STATE, SOCIAL_ST_IDLE);

	for (unsigned int i = 0; i < handle_list.count; i++) {
		telephony_sim_state_e state;
		int err = telephony_sim_get_state(handle_list.handle[i], &state);
		IF_FAIL_RETURN_TAG(err == TELEPHONY_ERROR_NONE, ERR_OPERATION_FAILED, _E, "Getting SIM status failed");

		if (state != TELEPHONY_SIM_STATE_AVAILABLE)
			continue;

		ret = read_current_status(handle_list.handle[i], data);
		break;
	}

	if (temporary_handle)
		release_telephony();

	if (ret) {
		ctx::context_manager::reply_to_read(SOCIAL_ST_SUBJ_CALL, NULL, ERR_NONE, data);
		return ERR_NONE;
	}

	return ERR_OPERATION_FAILED;
}
