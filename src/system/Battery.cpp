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

BatteryStateProvider::BatteryStateProvider()
	: BasicProvider(SUBJ_STATE_BATTERY)
{
}

BatteryStateProvider::~BatteryStateProvider()
{
}

bool BatteryStateProvider::isSupported()
{
	return true;
}

void BatteryStateProvider::__updateCb(device_callback_e deviceType, void* value, void* userData)
{
	IF_FAIL_VOID(deviceType == DEVICE_CALLBACK_BATTERY_LEVEL);

	BatteryStateProvider *instance = static_cast<BatteryStateProvider*>(userData);
	instance->__handleUpdate(deviceType, value);
}

void BatteryStateProvider::__handleUpdate(device_callback_e deviceType, void* value)
{
	intptr_t level = (intptr_t)value;

	const char* levelString = __transToString(level);
	IF_FAIL_VOID(levelString);

	Json dataRead;
	dataRead.set(NULL, KEY_LEVEL, levelString);

	bool chargingState = false;
	int ret = device_battery_is_charging(&chargingState);
	IF_FAIL_VOID_TAG(ret == DEVICE_ERROR_NONE, _E, "Getting state failed");

	dataRead.set(NULL, KEY_IS_CHARGING, chargingState ? VAL_TRUE : VAL_FALSE);
	publish(NULL, ERR_NONE, dataRead);
}

const char* BatteryStateProvider::__transToString(intptr_t level)
{
	switch (level) {
	case DEVICE_BATTERY_LEVEL_EMPTY:
		return VAL_EMPTY;

	case DEVICE_BATTERY_LEVEL_CRITICAL:
		return VAL_CRITICAL;

	case DEVICE_BATTERY_LEVEL_LOW:
		return VAL_LOW;

	case DEVICE_BATTERY_LEVEL_HIGH:
		return VAL_NORMAL;

	case DEVICE_BATTERY_LEVEL_FULL:
	{
		int percent;
		device_battery_get_percent(&percent);

		if (percent == 100) {
			return VAL_FULL;
		} else {
			return VAL_HIGH;
		}
		break;
	}

	default:
		_E("Invalid battery level");
		return NULL;
	}
}

int BatteryStateProvider::subscribe()
{
	int ret = device_add_callback(DEVICE_CALLBACK_BATTERY_LEVEL, __updateCb, this);
	IF_FAIL_RETURN(ret == DEVICE_ERROR_NONE, ERR_OPERATION_FAILED);
	return ERR_NONE;
}

int BatteryStateProvider::unsubscribe()
{
	int ret = device_remove_callback(DEVICE_CALLBACK_BATTERY_LEVEL, __updateCb);
	IF_FAIL_RETURN(ret == DEVICE_ERROR_NONE, ERR_OPERATION_FAILED);
	return ERR_NONE;
}

int BatteryStateProvider::read()
{
	device_battery_level_e level;
	Json dataRead;

	int ret = device_battery_get_level_status(&level);
	IF_FAIL_RETURN(ret == DEVICE_ERROR_NONE, ERR_OPERATION_FAILED);

	const char* levelString = __transToString(level);
	IF_FAIL_RETURN(levelString, ERR_OPERATION_FAILED);

	dataRead.set(NULL, KEY_LEVEL, levelString);

	bool chargingState = false;
	ret = device_battery_is_charging(&chargingState);
	IF_FAIL_RETURN(ret == DEVICE_ERROR_NONE, ERR_OPERATION_FAILED);

	dataRead.set(NULL, KEY_IS_CHARGING, chargingState ? VAL_TRUE : VAL_FALSE);

	replyToRead(NULL, ERR_NONE, dataRead);
	return ERR_NONE;
}
