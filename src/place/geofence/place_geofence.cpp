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

#include <types_internal.h>
#include <Json.h>
#include <ContextManager.h>
#include "place_geofence.h"

ctx::place_geofence_provider *ctx::place_geofence_provider::__instance = NULL;

ctx::place_geofence_provider::place_geofence_provider()
{
}

ctx::place_geofence_provider::~place_geofence_provider()
{
	for (handle_map_t::iterator it = __handle_map.begin(); it != __handle_map.end(); ++it) {
		delete it->second;
	}

	__handle_map.clear();
}

ctx::ContextProviderBase *ctx::place_geofence_provider::create(void *data)
{
	IF_FAIL_RETURN(!__instance, __instance);
	__instance = new(std::nothrow) place_geofence_provider();
	IF_FAIL_RETURN_TAG(__instance, NULL, _E, "Memory allocation failed");
	_I(BLUE("Created"));
	return __instance;
}

void ctx::place_geofence_provider::destroy(void *data)
{
	IF_FAIL_VOID(__instance);
	delete __instance;
	__instance = NULL;
	_I(BLUE("Destroyed"));
}

bool ctx::place_geofence_provider::is_supported()
{
	bool supported = false;
	int ret = geofence_manager_is_supported(&supported);
	IF_FAIL_RETURN_TAG(ret == GEOFENCE_MANAGER_ERROR_NONE, false, _E, "geofence_manager_is_supported() failed");
	return supported;
}

void ctx::place_geofence_provider::submit_trigger_item()
{
	context_manager::registerTriggerItem(PLACE_SUBJ_GEOFENCE, OPS_SUBSCRIBE,
			"{"
				"\"Event\":{\"type\":\"string\",\"values\":[\"In\",\"Out\"]}"
			"}",
			"{"
				"\"PlaceId\":{\"type\":\"integer\",\"min\":1}"
			"}");
}

void ctx::place_geofence_provider::__destroy_if_unused()
{
	IF_FAIL_VOID(__handle_map.empty());
	destroy(NULL);
}


int ctx::place_geofence_provider::subscribe(const char *subject, ctx::Json option, ctx::Json *requestResult)
{
	int ret = __subscribe(option);
	__destroy_if_unused();
	return ret;
}

int ctx::place_geofence_provider::unsubscribe(const char *subject, ctx::Json option)
{
	int ret = __unsubscribe(option);
	__destroy_if_unused();
	return ret;
}

int ctx::place_geofence_provider::read(const char *subject, ctx::Json option, ctx::Json *requestResult)
{
	__destroy_if_unused();
	return ERR_NOT_SUPPORTED;
}

int ctx::place_geofence_provider::write(const char *subject, ctx::Json data, ctx::Json *requestResult)
{
	__destroy_if_unused();
	return ERR_NOT_SUPPORTED;
}

int ctx::place_geofence_provider::__subscribe(ctx::Json option)
{
	int pid = -1;
	option.get(NULL, PLACE_STATUS_OPT_MYPLACE_ID, &pid);
	IF_FAIL_RETURN_TAG(pid != -1, ERR_INVALID_PARAMETER, _E, "Getting PlaceID failed");

	handle_map_t::iterator it = __handle_map.find(pid);
	if (it != __handle_map.end()) {
		_D("Place ID %d is being monitored already", pid);
		return ERR_NONE;
	}

	myplace_handle *handle = new(std::nothrow) myplace_handle();
	ASSERT_ALLOC(handle);

	bool ret = handle->start_monitor(pid);
	if (!ret) {
		_E("Monitoring Place ID %d failed", pid);
		delete handle;
		return ERR_OPERATION_FAILED;
	}

	__handle_map[pid] = handle;

	return ERR_NONE;
}

int ctx::place_geofence_provider::__unsubscribe(ctx::Json option)
{
	int pid = -1;
	option.get(NULL, PLACE_STATUS_OPT_MYPLACE_ID, &pid);
	IF_FAIL_RETURN_TAG(pid != -1, ERR_INVALID_PARAMETER, _E, "Getting PlaceID failed");

	handle_map_t::iterator it = __handle_map.find(pid);
	if (it == __handle_map.end()) {
		_D("Place ID %d is not being monitored", pid);
		return ERR_NONE;
	}

	delete it->second;
	__handle_map.erase(it);

	return ERR_NONE;
}
