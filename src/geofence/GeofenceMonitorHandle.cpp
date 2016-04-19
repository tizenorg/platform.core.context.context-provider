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

#include <Types.h>
#include <Json.h>
#include "PlaceGeofenceProvider.h"
#include "GeofenceMonitorHandle.h"

using namespace ctx;

GeofenceMonitorHandle::GeofenceMonitorHandle(PlaceGeofenceProvider *provider) :
	__provider(provider),
	__placeId(-1),
	__prevState(GEOFENCE_STATE_UNCERTAIN),
	__geoHandle(NULL)
{
}

GeofenceMonitorHandle::~GeofenceMonitorHandle()
{
	__stopMonitor();
}

bool GeofenceMonitorHandle::startMonitor(int placeId)
{
	_D("Starts to monitor Place-%d", placeId);

	IF_FAIL_RETURN(placeId >= 0, false);
	IF_FAIL_RETURN_TAG(__geoHandle == NULL, false, _E, "Re-starting MyPlace monitor");

	geofence_manager_create(&__geoHandle);
	IF_FAIL_RETURN_TAG(__geoHandle, false, _E, "Geofence initialization failed");

	int ret;

	ret = geofence_manager_set_geofence_state_changed_cb(__geoHandle, __fenceStateCb, this);
	IF_FAIL_CATCH_TAG(ret == GEOFENCE_MANAGER_ERROR_NONE, _E, "Setting state callback failed");

	ret = geofence_manager_set_geofence_event_cb(__geoHandle, __fenceEventCb, this);
	IF_FAIL_CATCH_TAG(ret == GEOFENCE_MANAGER_ERROR_NONE, _E, "Setting event callback failed");

	ret = geofence_manager_foreach_place_geofence_list(__geoHandle, placeId, __fenceListCb, this);
	IF_FAIL_CATCH_TAG(ret == GEOFENCE_MANAGER_ERROR_NONE, _E, "Getting fence list failed");

	__placeId = placeId;
	return true;

CATCH:
	__stopMonitor();
	return false;
}

int GeofenceMonitorHandle::getPlaceId()
{
	return __placeId;
}

void GeofenceMonitorHandle::__stopMonitor()
{
	_D("Stops monitoring Place-%d", __placeId);

	//TODO: Do we need to stop all geofences explicitly?
	if (__geoHandle) {
		geofence_manager_destroy(__geoHandle);
		__geoHandle = NULL;
	}

	__geoStateMap.clear();
	__placeId = -1;
	__prevState = GEOFENCE_STATE_UNCERTAIN;
}

bool GeofenceMonitorHandle::__startFence(int fenceId)
{
	int ret;

	ret = geofence_manager_start(__geoHandle, fenceId);
	IF_FAIL_RETURN_TAG(ret == GEOFENCE_MANAGER_ERROR_NONE, true, _W, "Starting failed");

	geofence_status_h status;
	ret = geofence_status_create(fenceId, &status);
	IF_FAIL_RETURN_TAG(ret == GEOFENCE_MANAGER_ERROR_NONE, true, _W, "Getting status failed");

	geofence_state_e state = GEOFENCE_STATE_UNCERTAIN;
	geofence_status_get_state(status, &state);
	geofence_status_destroy(status);

	__geoStateMap[fenceId] = state;

	return true;
}

void GeofenceMonitorHandle::__removeFence(int fenceId)
{
	geofence_manager_stop(__geoHandle, fenceId);
	__geoStateMap.erase(fenceId);
}

void GeofenceMonitorHandle::__updateFence(int fenceId, geofence_manage_e manage)
{
	switch (manage) {
	case GEOFENCE_MANAGE_PLACE_REMOVED:
		_W("[Place-%d] Removed", __placeId);
		__stopMonitor();
		break;
	case GEOFENCE_MANAGE_FENCE_ADDED:
		_I("[Place %d] Fence-%d added", __placeId, fenceId);
		__startFence(fenceId);
		__emitStateChange();
		break;
	case GEOFENCE_MANAGE_FENCE_REMOVED:
		_I("[Place-%d] Fence-%d removed", __placeId, fenceId);
		__removeFence(fenceId);
		__emitStateChange();
		break;
	case GEOFENCE_MANAGE_FENCE_STARTED:
		_D("[Place-%d] Fence-%d started", __placeId, fenceId);
		break;
	case GEOFENCE_MANAGE_FENCE_STOPPED:
		_D("[Place-%d] Fence-%d stopped", __placeId, fenceId);
		//TODO: Do we need to restart this?
		break;
	default:
		_D("[Place-%d] Ignoring the manage event %d", __placeId, manage);
		break;
	}
}

void GeofenceMonitorHandle::__updateState(int fenceId, geofence_state_e state)
{
	__geoStateMap[fenceId] = state;
}

void GeofenceMonitorHandle::__emitStateChange()
{
	geofence_state_e currentState = GEOFENCE_STATE_UNCERTAIN;
	int outCount = 0;

	for (auto it = __geoStateMap.begin(); it != __geoStateMap.end(); ++it) {
		if (it->second == GEOFENCE_STATE_IN) {
			currentState = GEOFENCE_STATE_IN;
			break;
		} else if (it->second == GEOFENCE_STATE_OUT) {
			++outCount;
		}
	}

	if (currentState != GEOFENCE_STATE_IN && outCount > 0) {
		currentState = GEOFENCE_STATE_OUT;
	}

	if (currentState == __prevState) {
		return;
	}

	__prevState = currentState;

	Json option;
	option.set(NULL, PLACE_GEOFENCE_PLACE_ID, __placeId);

	Json data;
	data.set(NULL, PLACE_GEOFENCE_PLACE_ID, __placeId);
	data.set(NULL, PLACE_GEOFENCE_EVENT, __getStateString(currentState));

	__provider->publish(option, ERR_NONE, data);
}

const char* GeofenceMonitorHandle::__getStateString(geofence_state_e state)
{
	switch (state) {
	case GEOFENCE_STATE_IN:
		return PLACE_GEOFENCE_IN;
	case GEOFENCE_STATE_OUT:
		return PLACE_GEOFENCE_OUT;
	case GEOFENCE_STATE_UNCERTAIN:
		return PLACE_GEOFENCE_UNCERTAIN;
	default:
		return PLACE_GEOFENCE_UNCERTAIN;
	}
}

bool GeofenceMonitorHandle::__fenceListCb(int geofenceId, geofence_h fence, int fenceIndex, int fenceCount, void* userData)
{
	_D("FenceID: %d, Index: %d, Count: %d", geofenceId, fenceIndex, fenceCount);
	IF_FAIL_RETURN(fenceCount > 0, false);

	GeofenceMonitorHandle *handle = reinterpret_cast<GeofenceMonitorHandle*>(userData);
	return handle->__startFence(geofenceId);
}

void GeofenceMonitorHandle::__fenceEventCb(int placeId, int geofenceId, geofence_manager_error_e error, geofence_manage_e manage, void* userData)
{
	IF_FAIL_VOID_TAG(error == GEOFENCE_MANAGER_ERROR_NONE, _W, "Geofence error: %d", error);

	GeofenceMonitorHandle *handle = reinterpret_cast<GeofenceMonitorHandle*>(userData);

	IF_FAIL_VOID_TAG(placeId == handle->getPlaceId(), _W, "Mismatched Place ID");

	handle->__updateFence(geofenceId, manage);
}

void GeofenceMonitorHandle::__fenceStateCb(int geofenceId, geofence_state_e state, void* userData)
{
	GeofenceMonitorHandle *handle = reinterpret_cast<GeofenceMonitorHandle*>(userData);
	handle->__updateState(geofenceId, state);
	handle->__emitStateChange();
}
