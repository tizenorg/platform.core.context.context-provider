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

#include "Battery.h"

using namespace ctx;

DeviceStatusBattery::DeviceStatusBattery()
	: BasicProvider(DEVICE_ST_SUBJ_BATTERY)
{
}

DeviceStatusBattery::~DeviceStatusBattery()
{
}

bool DeviceStatusBattery::isSupported()
{
	return true;
}

void DeviceStatusBattery::submitTriggerItem()
{
	registerTriggerItem(OPS_SUBSCRIBE | OPS_READ,
			"{"
				"\"Level\":{\"type\":\"string\",\"values\":[\"Empty\",\"Critical\",\"Low\",\"Normal\",\"High\",\"Full\"]},"
				TRIG_BOOL_ITEM_DEF("IsCharging")
			"}",
			NULL);
}

void DeviceStatusBattery::__updateCb(device_callback_e deviceType, void* value, void* userData)
{
	IF_FAIL_VOID(deviceType == DEVICE_CALLBACK_BATTERY_LEVEL);

	DeviceStatusBattery *instance = static_cast<DeviceStatusBattery*>(userData);
	instance->__handleUpdate(deviceType, value);
}

void DeviceStatusBattery::__handleUpdate(device_callback_e deviceType, void* value)
{
	intptr_t level = (intptr_t)value;

	const char* levelString = __transToString(level);
	IF_FAIL_VOID(levelString);

	Json dataRead;
	dataRead.set(NULL, DEVICE_ST_LEVEL, levelString);

	bool chargingState = false;
	int ret = device_battery_is_charging(&chargingState);
	IF_FAIL_VOID_TAG(ret == DEVICE_ERROR_NONE, _E, "Getting state failed");

	dataRead.set(NULL, DEVICE_ST_IS_CHARGING, chargingState ? DEVICE_ST_TRUE : DEVICE_ST_FALSE);
	publish(NULL, ERR_NONE, dataRead);
}

const char* DeviceStatusBattery::__transToString(intptr_t level)
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
		break;
	}

	default:
		_E("Invalid battery level");
		return NULL;
	}
}

int DeviceStatusBattery::subscribe()
{
	int ret = device_add_callback(DEVICE_CALLBACK_BATTERY_LEVEL, __updateCb, this);
	IF_FAIL_RETURN(ret == DEVICE_ERROR_NONE, ERR_OPERATION_FAILED);
	return ERR_NONE;
}

int DeviceStatusBattery::unsubscribe()
{
	int ret = device_remove_callback(DEVICE_CALLBACK_BATTERY_LEVEL, __updateCb);
	IF_FAIL_RETURN(ret == DEVICE_ERROR_NONE, ERR_OPERATION_FAILED);
	return ERR_NONE;
}

int DeviceStatusBattery::read()
{
	device_battery_level_e level;
	Json dataRead;

	int ret = device_battery_get_level_status(&level);
	IF_FAIL_RETURN(ret == DEVICE_ERROR_NONE, ERR_OPERATION_FAILED);

	const char* levelString = __transToString(level);
	IF_FAIL_RETURN(levelString, ERR_OPERATION_FAILED);

	dataRead.set(NULL, DEVICE_ST_LEVEL, levelString);

	bool chargingState = false;
	ret = device_battery_is_charging(&chargingState);
	IF_FAIL_RETURN(ret == DEVICE_ERROR_NONE, ERR_OPERATION_FAILED);

	dataRead.set(NULL, DEVICE_ST_IS_CHARGING, chargingState ? DEVICE_ST_TRUE : DEVICE_ST_FALSE);

	replyToRead(NULL, ERR_NONE, dataRead);
	return ERR_NONE;
}
