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

#include <types_internal.h>
#include <Json.h>
#include <ContextManager.h>
#include "place_geofence_types.h"
#include "myplace_handle.h"

ctx::myplace_handle::myplace_handle()
	: _place_id(-1)
	, prev_state(GEOFENCE_STATE_UNCERTAIN)
	, geo_handle(NULL)
{
}

ctx::myplace_handle::~myplace_handle()
{
	stop_monitor();
}

bool ctx::myplace_handle::start_monitor(int place_id)
{
	_D("Starts to monitor Place-%d", place_id);

	IF_FAIL_RETURN(place_id >= 0, false);
	IF_FAIL_RETURN_TAG(geo_handle == NULL, false, _E, "Re-starting MyPlace monitor");

	geofence_manager_create(&geo_handle);
	IF_FAIL_RETURN_TAG(geo_handle, false, _E, "Geofence initialization failed");

	int ret;

	ret = geofence_manager_set_geofence_state_changed_cb(geo_handle, fence_state_cb, this);
	IF_FAIL_CATCH_TAG(ret == GEOFENCE_MANAGER_ERROR_NONE, _E, "Setting state callback failed");

	ret = geofence_manager_set_geofence_event_cb(geo_handle, fence_event_cb, this);
	IF_FAIL_CATCH_TAG(ret == GEOFENCE_MANAGER_ERROR_NONE, _E, "Setting event callback failed");

	ret = geofence_manager_foreach_place_geofence_list(geo_handle, place_id, fence_list_cb, this);
	IF_FAIL_CATCH_TAG(ret == GEOFENCE_MANAGER_ERROR_NONE, _E, "Getting fence list failed");

	_place_id = place_id;
	return true;

CATCH:
	stop_monitor();
	return false;
}

int ctx::myplace_handle::get_place_id()
{
	return _place_id;
}

void ctx::myplace_handle::stop_monitor()
{
	_D("Stops monitoring Place-%d", _place_id);

	//TODO: Do we need to stop all geofences explicitly?
	if (geo_handle) {
		geofence_manager_destroy(geo_handle);
		geo_handle = NULL;
	}

	geo_state_map.clear();
	_place_id = -1;
	prev_state = GEOFENCE_STATE_UNCERTAIN;
}

bool ctx::myplace_handle::start_fence(int fence_id)
{
	int ret;

	ret = geofence_manager_start(geo_handle, fence_id);
	IF_FAIL_RETURN_TAG(ret == GEOFENCE_MANAGER_ERROR_NONE, true, _W, "Starting failed");

	geofence_status_h status;
	ret = geofence_status_create(fence_id, &status);
	IF_FAIL_RETURN_TAG(ret == GEOFENCE_MANAGER_ERROR_NONE, true, _W, "Getting status failed");

	geofence_state_e state = GEOFENCE_STATE_UNCERTAIN;
	geofence_status_get_state(status, &state);
	geofence_status_destroy(status);

	geo_state_map[fence_id] = state;

	return true;
}

void ctx::myplace_handle::remove_fence(int fence_id)
{
	geofence_manager_stop(geo_handle, fence_id);
	geo_state_map.erase(fence_id);
}

void ctx::myplace_handle::update_fence(int fence_id, geofence_manage_e manage)
{
	switch (manage) {
		case GEOFENCE_MANAGE_PLACE_REMOVED:
			_W("[Place-%d] Removed", _place_id);
			stop_monitor();
			break;
		case GEOFENCE_MANAGE_FENCE_ADDED:
			_I("[Place %d] Fence-%d added", _place_id, fence_id);
			start_fence(fence_id);
			emit_state_change();
			break;
		case GEOFENCE_MANAGE_FENCE_REMOVED:
			_I("[Place-%d] Fence-%d removed", _place_id, fence_id);
			remove_fence(fence_id);
			emit_state_change();
			break;
		case GEOFENCE_MANAGE_FENCE_STARTED:
			_D("[Place-%d] Fence-%d started", _place_id, fence_id);
			break;
		case GEOFENCE_MANAGE_FENCE_STOPPED:
			_D("[Place-%d] Fence-%d stopped", _place_id, fence_id);
			//TODO: Do we need to restart this?
			break;
		default:
			_D("[Place-%d] Ignoring the manage event %d", _place_id, manage);
			break;
	}
}

void ctx::myplace_handle::update_state(int fence_id, geofence_state_e state)
{
	geo_state_map[fence_id] = state;
}

static const char* get_state_string(geofence_state_e state)
{
	switch (state) {
		case GEOFENCE_STATE_IN:
			return MYPLACE_EVENT_IN;
		case GEOFENCE_STATE_OUT:
			return MYPLACE_EVENT_OUT;
		case GEOFENCE_STATE_UNCERTAIN:
			return MYPLACE_EVENT_UNCERTAIN;
		default:
			return MYPLACE_EVENT_UNCERTAIN;
	}
}

void ctx::myplace_handle::emit_state_change()
{
	geofence_state_e current_state = GEOFENCE_STATE_UNCERTAIN;
	int out_count = 0;

	for (geo_state_map_t::iterator it = geo_state_map.begin(); it != geo_state_map.end(); ++it) {
		if (it->second == GEOFENCE_STATE_IN) {
			current_state = GEOFENCE_STATE_IN;
			break;
		} else if (it->second == GEOFENCE_STATE_OUT) {
			++ out_count;
		}
	}

	if (current_state != GEOFENCE_STATE_IN && out_count > 0) {
		current_state = GEOFENCE_STATE_OUT;
	}

	if (current_state == prev_state) {
		return;
	}

	prev_state = current_state;

	Json option;
	option.set(NULL, PLACE_STATUS_DATA_MYPLACE_ID, _place_id);

	Json data;
	data.set(NULL, PLACE_STATUS_DATA_MYPLACE_ID, _place_id);
	data.set(NULL, PLACE_STATUS_DATA_MYPLACE_EVENT, get_state_string(current_state));

	context_manager::publish(PLACE_SUBJ_GEOFENCE, option, ERR_NONE, data);
}

bool ctx::myplace_handle::fence_list_cb(int geofence_id, geofence_h fence, int fence_index, int fence_cnt, void* user_data)
{
	_D("FenceID: %d, Index: %d, Count: %d", geofence_id, fence_index, fence_cnt);
	IF_FAIL_RETURN(fence_cnt > 0, false);

	myplace_handle *handle = reinterpret_cast<myplace_handle*>(user_data);
	return handle->start_fence(geofence_id);
}

void ctx::myplace_handle::fence_event_cb(int place_id, int geofence_id, geofence_manager_error_e error, geofence_manage_e manage, void* user_data)
{
	IF_FAIL_VOID_TAG(error == GEOFENCE_MANAGER_ERROR_NONE, _W, "Geofence error: %d", error);

	myplace_handle *handle = reinterpret_cast<myplace_handle*>(user_data);

	IF_FAIL_VOID_TAG(place_id == handle->get_place_id(), _W, "Mismatched Place ID");

	handle->update_fence(geofence_id, manage);
}

void ctx::myplace_handle::fence_state_cb(int geofence_id, geofence_state_e state, void* user_data)
{
	myplace_handle *handle = reinterpret_cast<myplace_handle*>(user_data);
	handle->update_state(geofence_id, state);
	handle->emit_state_change();
}
