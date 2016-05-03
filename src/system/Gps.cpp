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

#include <Util.h>
#include "Gps.h"

using namespace ctx;

static const char* __getStatusString(int gpsStatus)
{
	switch (gpsStatus) {
	case RUNTIME_INFO_GPS_STATUS_DISABLED:
		return VAL_DISABLED;

	case RUNTIME_INFO_GPS_STATUS_SEARCHING:
		return VAL_SEARCHING;

	case RUNTIME_INFO_GPS_STATUS_CONNECTED:
		return VAL_CONNECTED;

	default:
		_E("Unknown GPS status: %d", gpsStatus);
		return NULL;
	}
}

GpsStateProvider::GpsStateProvider() :
	RuntimeInfoProvider(SUBJ_STATE_GPS, RUNTIME_INFO_KEY_GPS_STATUS)
{
}

GpsStateProvider::~GpsStateProvider()
{
}

bool GpsStateProvider::isSupported()
{
	return util::getSystemInfoBool("tizen.org/feature/location.gps");
}

void GpsStateProvider::handleUpdate()
{
	int gpsStatus;
	int ret = runtime_info_get_value_int(RUNTIME_INFO_KEY_GPS_STATUS, &gpsStatus);
	IF_FAIL_VOID_TAG(ret == RUNTIME_INFO_ERROR_NONE, _E, "Getting runtime info failed");

	Json dataRead;

	const char* stateStr = __getStatusString(gpsStatus);
	IF_FAIL_VOID(stateStr);

	dataRead.set(NULL, KEY_STATE, stateStr);

	publish(NULL, ERR_NONE, dataRead);
}

int GpsStateProvider::read()
{
	int gpsStatus;
	Json dataRead;

	int ret = runtime_info_get_value_int(RUNTIME_INFO_KEY_GPS_STATUS, &gpsStatus);
	IF_FAIL_RETURN_TAG(ret == RUNTIME_INFO_ERROR_NONE, ERR_OPERATION_FAILED, _E, "Getting runtime info failed");

	const char* stateStr = __getStatusString(gpsStatus);
	IF_FAIL_RETURN(stateStr, ERR_OPERATION_FAILED);

	dataRead.set(NULL, KEY_STATE, stateStr);

	replyToRead(NULL, ERR_NONE, dataRead);
	return ERR_NONE;
}
