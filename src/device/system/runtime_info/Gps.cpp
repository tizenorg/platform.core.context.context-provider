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
#include "../SystemTypes.h"
#include "Gps.h"

GENERATE_PROVIDER_COMMON_IMPL(DeviceStatusGps);

static const char* __getStatusString(int gpsStatus)
{
	switch (gpsStatus) {
	case RUNTIME_INFO_GPS_STATUS_DISABLED:
		return DEVICE_ST_DISABLED;

	case RUNTIME_INFO_GPS_STATUS_SEARCHING:
		return DEVICE_ST_SEARCHING;

	case RUNTIME_INFO_GPS_STATUS_CONNECTED:
		return DEVICE_ST_CONNECTED;

	default:
		_E("Unknown GPS status: %d", gpsStatus);
		return NULL;
	}
}

ctx::DeviceStatusGps::DeviceStatusGps() :
	DeviceStatusRuntimeInfo(RUNTIME_INFO_KEY_GPS_STATUS)
{
}

ctx::DeviceStatusGps::~DeviceStatusGps()
{
}

bool ctx::DeviceStatusGps::isSupported()
{
	return getSystemInfoBool("tizen.org/feature/location.gps");
}

void ctx::DeviceStatusGps::submitTriggerItem()
{
	context_manager::registerTriggerItem(DEVICE_ST_SUBJ_GPS, OPS_SUBSCRIBE | OPS_READ,
			"{"
				"\"State\":{\"type\":\"string\",\"values\":[\"Disabled\",\"Searching\",\"Connected\"]}"
			"}",
			NULL);
}

void ctx::DeviceStatusGps::handleUpdate()
{
	int gpsStatus;
	int ret = runtime_info_get_value_int(RUNTIME_INFO_KEY_GPS_STATUS, &gpsStatus);
	IF_FAIL_VOID_TAG(ret == RUNTIME_INFO_ERROR_NONE, _E, "Getting runtime info failed");

	ctx::Json dataRead;

	const char* stateStr = __getStatusString(gpsStatus);
	IF_FAIL_VOID(stateStr);

	dataRead.set(NULL, DEVICE_ST_STATE, stateStr);

	context_manager::publish(DEVICE_ST_SUBJ_GPS, NULL, ERR_NONE, dataRead);
}

int ctx::DeviceStatusGps::read()
{
	int gpsStatus;
	ctx::Json dataRead;

	int ret = runtime_info_get_value_int(RUNTIME_INFO_KEY_GPS_STATUS, &gpsStatus);
	IF_FAIL_RETURN_TAG(ret == RUNTIME_INFO_ERROR_NONE, ERR_OPERATION_FAILED, _E, "Getting runtime info failed");

	const char* stateStr = __getStatusString(gpsStatus);
	IF_FAIL_RETURN(stateStr, ERR_OPERATION_FAILED);

	dataRead.set(NULL, DEVICE_ST_STATE, stateStr);

	ctx::context_manager::replyToRead(DEVICE_ST_SUBJ_GPS, NULL, ERR_NONE, dataRead);
	return ERR_NONE;
}
