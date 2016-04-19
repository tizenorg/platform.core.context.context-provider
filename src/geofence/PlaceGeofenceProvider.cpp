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

#include <geofence_manager.h>
#include "GeofenceMonitorHandle.h"
#include "PlaceGeofenceProvider.h"

using namespace ctx;

PlaceGeofenceProvider::PlaceGeofenceProvider() :
	ContextProvider(PLACE_SUBJ_GEOFENCE)
{
}

PlaceGeofenceProvider::~PlaceGeofenceProvider()
{
	for (auto& it : __handleMap) {
		delete it.second;
	}

	__handleMap.clear();
}

bool PlaceGeofenceProvider::isSupported()
{
	bool supported = false;
	int ret = geofence_manager_is_supported(&supported);
	IF_FAIL_RETURN_TAG(ret == GEOFENCE_MANAGER_ERROR_NONE, false, _E, "geofence_manager_is_supported() failed");
	return supported;
}

void PlaceGeofenceProvider::submitTriggerItem()
{
	registerTriggerItem(OPS_SUBSCRIBE,
			"{"
				"\"Event\":{\"type\":\"string\",\"values\":[\"In\",\"Out\"]}"
			"}",
			"{"
				"\"PlaceId\":{\"type\":\"integer\",\"min\":1}"
			"}");
}

int PlaceGeofenceProvider::subscribe(Json option, Json *requestResult)
{
	int placeId = -1;
	option.get(NULL, PLACE_GEOFENCE_PLACE_ID, &placeId);
	IF_FAIL_RETURN_TAG(placeId != -1, ERR_INVALID_PARAMETER, _E, "Getting PlaceID failed");

	auto it = __handleMap.find(placeId);
	if (it != __handleMap.end()) {
		_D("Place ID %d is being monitored already", placeId);
		return ERR_NONE;
	}

	GeofenceMonitorHandle *handle = new(std::nothrow) GeofenceMonitorHandle(this);
	ASSERT_ALLOC(handle);

	bool ret = handle->startMonitor(placeId);
	if (!ret) {
		_E("Monitoring Place ID %d failed", placeId);
		delete handle;
		return ERR_OPERATION_FAILED;
	}

	__handleMap[placeId] = handle;

	return ERR_NONE;
}

int PlaceGeofenceProvider::unsubscribe(Json option)
{
	int placeId = -1;
	option.get(NULL, PLACE_GEOFENCE_PLACE_ID, &placeId);
	IF_FAIL_RETURN_TAG(placeId != -1, ERR_INVALID_PARAMETER, _E, "Getting PlaceID failed");

	auto it = __handleMap.find(placeId);
	if (it == __handleMap.end()) {
		_D("Place ID %d is not being monitored", placeId);
		return ERR_NONE;
	}

	delete it->second;
	__handleMap.erase(it);

	return ERR_NONE;
}
