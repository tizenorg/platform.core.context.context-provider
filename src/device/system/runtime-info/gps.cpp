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
#include "../system_types.h"
#include "gps.h"

GENERATE_PROVIDER_COMMON_IMPL(device_status_gps);

static const char* get_state_string(int gps_state)
{
	switch (gps_state) {
	case RUNTIME_INFO_GPS_STATUS_DISABLED:
		return DEVICE_ST_DISABLED;

	case RUNTIME_INFO_GPS_STATUS_SEARCHING:
		return DEVICE_ST_SEARCHING;

	case RUNTIME_INFO_GPS_STATUS_CONNECTED:
		return DEVICE_ST_CONNECTED;

	default:
		_E("Unknown GPS status: %d", gps_state);
		return NULL;
	}
}

ctx::device_status_gps::device_status_gps()
	: device_status_runtime_info(RUNTIME_INFO_KEY_GPS_STATUS)
{
}

ctx::device_status_gps::~device_status_gps()
{
}

bool ctx::device_status_gps::is_supported()
{
	return get_system_info_bool("tizen.org/feature/location.gps");
}

void ctx::device_status_gps::submit_trigger_item()
{
	context_manager::register_trigger_item(DEVICE_ST_SUBJ_GPS, OPS_SUBSCRIBE | OPS_READ,
			"{"
				"\"State\":{\"type\":\"string\",\"values\":[\"Disabled\",\"Searching\",\"Connected\"]}"
			"}",
			NULL);
}

void ctx::device_status_gps::handle_update()
{
	int gps_status;
	int ret = runtime_info_get_value_int(RUNTIME_INFO_KEY_GPS_STATUS, &gps_status);
	IF_FAIL_VOID_TAG(ret == RUNTIME_INFO_ERROR_NONE, _E, "Getting runtime info failed");

	ctx::Json data_read;

	const char* state_str = get_state_string(gps_status);
	IF_FAIL_VOID(state_str);

	data_read.set(NULL, DEVICE_ST_STATE, state_str);

	context_manager::publish(DEVICE_ST_SUBJ_GPS, NULL, ERR_NONE, data_read);
}

int ctx::device_status_gps::read()
{
	int gps_status;
	ctx::Json data_read;

	int ret = runtime_info_get_value_int(RUNTIME_INFO_KEY_GPS_STATUS, &gps_status);
	IF_FAIL_RETURN_TAG(ret == RUNTIME_INFO_ERROR_NONE, ERR_OPERATION_FAILED, _E, "Getting runtime info failed");

	const char* state_str = get_state_string(gps_status);
	IF_FAIL_RETURN(state_str, ERR_OPERATION_FAILED);

	data_read.set(NULL, DEVICE_ST_STATE, state_str);

	ctx::context_manager::reply_to_read(DEVICE_ST_SUBJ_GPS, NULL, ERR_NONE, data_read);
	return ERR_NONE;
}
