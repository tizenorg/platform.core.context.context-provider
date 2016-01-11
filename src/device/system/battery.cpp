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
#include "system_types.h"
#include "battery.h"

GENERATE_PROVIDER_COMMON_IMPL(device_status_battery);

ctx::device_status_battery::device_status_battery()
{
}

ctx::device_status_battery::~device_status_battery()
{
}

bool ctx::device_status_battery::is_supported()
{
	return true;
}

void ctx::device_status_battery::submit_trigger_item()
{
	context_manager::register_trigger_item(DEVICE_ST_SUBJ_BATTERY, OPS_SUBSCRIBE | OPS_READ,
			"{"
				"\"Level\":{\"type\":\"string\",\"values\":[\"Empty\",\"Critical\",\"Low\",\"Normal\",\"High\",\"Full\"]},"
				TRIG_BOOL_ITEM_DEF("IsCharging")
			"}",
			NULL);
}

void ctx::device_status_battery::update_cb(device_callback_e device_type, void* value, void* user_data)
{
	IF_FAIL_VOID(device_type == DEVICE_CALLBACK_BATTERY_LEVEL);

	device_status_battery *instance = static_cast<device_status_battery*>(user_data);
	instance->handle_update(device_type, value);
}

void ctx::device_status_battery::handle_update(device_callback_e device_type, void* value)
{
	intptr_t level = (intptr_t)value;

	const char* level_string = trans_to_string(level);
	IF_FAIL_VOID(level_string);

	ctx::json data_read;
	data_read.set(NULL, DEVICE_ST_LEVEL, level_string);

	bool charging_state = false;
	int ret = device_battery_is_charging(&charging_state);
	IF_FAIL_VOID_TAG(ret == DEVICE_ERROR_NONE, _E, "Getting state failed");

	data_read.set(NULL, DEVICE_ST_IS_CHARGING, charging_state ? DEVICE_ST_TRUE : DEVICE_ST_FALSE);
	ctx::context_manager::publish(DEVICE_ST_SUBJ_BATTERY, NULL, ERR_NONE, data_read);
}

const char* ctx::device_status_battery::trans_to_string(intptr_t level)
{
	switch (level) {
	case DEVICE_BATTERY_LEVEL_EMPTY:
		return DEVICE_ST_EMPTY;

	case DEVICE_BATTERY_LEVEL_CRITICAL:
		return DEVICE_ST_CRITICAL;

	case DEVICE_BATTERY_LEVEL_LOW:
		return DEVICE_ST_LOW;

	case DEVICE_BATTERY_LEVEL_HIGH:
		return DEVICE_ST_NORMAL;

	case DEVICE_BATTERY_LEVEL_FULL:
		{
			int percent;
			device_battery_get_percent(&percent);

			if (percent == 100) {
				return DEVICE_ST_FULL;
			} else {
				return DEVICE_ST_HIGH;
			}
		}
		break;

	default:
		_E("Invalid battery level");
		return NULL;
	}
}

int ctx::device_status_battery::subscribe()
{
	int ret = device_add_callback(DEVICE_CALLBACK_BATTERY_LEVEL, update_cb, this);
	IF_FAIL_RETURN(ret == DEVICE_ERROR_NONE, ERR_OPERATION_FAILED);
	return ERR_NONE;
}

int ctx::device_status_battery::unsubscribe()
{
	int ret = device_remove_callback(DEVICE_CALLBACK_BATTERY_LEVEL, update_cb);
	IF_FAIL_RETURN(ret == DEVICE_ERROR_NONE, ERR_OPERATION_FAILED);
	return ERR_NONE;
}

int ctx::device_status_battery::read()
{
	device_battery_level_e level;
	ctx::json data_read;

	int ret = device_battery_get_level_status(&level);
	IF_FAIL_RETURN(ret == DEVICE_ERROR_NONE, ERR_OPERATION_FAILED);

	const char* level_string = trans_to_string(level);
	IF_FAIL_RETURN(level_string, ERR_OPERATION_FAILED);

	data_read.set(NULL, DEVICE_ST_LEVEL, level_string);

	bool charging_state = false;
	ret = device_battery_is_charging(&charging_state);
	IF_FAIL_RETURN(ret == DEVICE_ERROR_NONE, ERR_OPERATION_FAILED);

	data_read.set(NULL, DEVICE_ST_IS_CHARGING, charging_state ? DEVICE_ST_TRUE : DEVICE_ST_FALSE);

	ctx::context_manager::reply_to_read(DEVICE_ST_SUBJ_BATTERY, NULL, ERR_NONE, data_read);
	return ERR_NONE;
}
