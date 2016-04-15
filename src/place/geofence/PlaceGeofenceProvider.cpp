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

#include <Types.h>
#include <Json.h>
#include <ContextManager.h>
#include "PlaceGeofenceProvider.h"

ctx::PlaceGeofenceProvider *ctx::PlaceGeofenceProvider::__instance = NULL;

ctx::PlaceGeofenceProvider::PlaceGeofenceProvider()
{
}

ctx::PlaceGeofenceProvider::~PlaceGeofenceProvider()
{
	for (auto it = __handleMap.begin(); it != __handleMap.end(); ++it) {
		delete it->second;
	}

	__handleMap.clear();
}

ctx::ContextProviderBase *ctx::PlaceGeofenceProvider::create(void *data)
{
	IF_FAIL_RETURN(!__instance, __instance);
	__instance = new(std::nothrow) PlaceGeofenceProvider();
	IF_FAIL_RETURN_TAG(__instance, NULL, _E, "Memory allocation failed");
	_I(BLUE("Created"));
	return __instance;
}

void ctx::PlaceGeofenceProvider::destroy(void *data)
{
	IF_FAIL_VOID(__instance);
	delete __instance;
	__instance = NULL;
	_I(BLUE("Destroyed"));
}

bool ctx::PlaceGeofenceProvider::isSupported()
{
	bool supported = false;
	int ret = geofence_manager_is_supported(&supported);
	IF_FAIL_RETURN_TAG(ret == GEOFENCE_MANAGER_ERROR_NONE, false, _E, "geofence_manager_is_supported() failed");
	return supported;
}

void ctx::PlaceGeofenceProvider::submitTriggerItem()
{
	context_manager::registerTriggerItem(PLACE_SUBJ_GEOFENCE, OPS_SUBSCRIBE,
			"{"
				"\"Event\":{\"type\":\"string\",\"values\":[\"In\",\"Out\"]}"
			"}",
			"{"
				"\"PlaceId\":{\"type\":\"integer\",\"min\":1}"
			"}");
}

void ctx::PlaceGeofenceProvider::__destroyIfUnused()
{
	IF_FAIL_VOID(__handleMap.empty());
	destroy(NULL);
}


int ctx::PlaceGeofenceProvider::subscribe(const char *subject, ctx::Json option, ctx::Json *requestResult)
{
	int ret = __subscribe(option);
	__destroyIfUnused();
	return ret;
}

int ctx::PlaceGeofenceProvider::unsubscribe(const char *subject, ctx::Json option)
{
	int ret = __unsubscribe(option);
	__destroyIfUnused();
	return ret;
}

int ctx::PlaceGeofenceProvider::read(const char *subject, ctx::Json option, ctx::Json *requestResult)
{
	__destroyIfUnused();
	return ERR_NOT_SUPPORTED;
}

int ctx::PlaceGeofenceProvider::write(const char *subject, ctx::Json data, ctx::Json *requestResult)
{
	__destroyIfUnused();
	return ERR_NOT_SUPPORTED;
}

int ctx::PlaceGeofenceProvider::__subscribe(ctx::Json option)
{
	int placeId = -1;
	option.get(NULL, PLACE_GEOFENCE_PLACE_ID, &placeId);
	IF_FAIL_RETURN_TAG(placeId != -1, ERR_INVALID_PARAMETER, _E, "Getting PlaceID failed");

	auto it = __handleMap.find(placeId);
	if (it != __handleMap.end()) {
		_D("Place ID %d is being monitored already", placeId);
		return ERR_NONE;
	}

	GeofenceMonitorHandle *handle = new(std::nothrow) GeofenceMonitorHandle();
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

int ctx::PlaceGeofenceProvider::__unsubscribe(ctx::Json option)
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
