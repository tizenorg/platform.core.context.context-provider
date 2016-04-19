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

#include <system_info.h>
#include "ActivityTypes.h"
#include "ActivityBase.h"

using namespace ctx;

UserActivityBase::UserActivityBase(const char *subject, activity_type_e type)	:
	DeviceProviderBase(subject),
	__activityType(type),
	__activityHandle(NULL)
{
}

UserActivityBase::~UserActivityBase()
{
	if (__activityHandle)
		activity_release(__activityHandle);
}

bool UserActivityBase::isSupported()
{
	return getSystemInfoBool("tizen.org/feature/sensor.activity_recognition");
}

void UserActivityBase::submitTriggerItem()
{
	registerTriggerItem(OPS_SUBSCRIBE,
			"{\"Event\":{\"type\":\"string\", \"values\":[\"Detected\"]}}",
			"{\"Accuracy\":{\"type\":\"string\", \"values\":[\"Low\", \"Normal\", \"High\"]}}"
			);
}

void UserActivityBase::__updateCb(activity_type_e activity, const activity_data_h data, double timestamp, activity_error_e error, void* userData)
{
	IF_FAIL_VOID_TAG(error == ACTIVITY_ERROR_NONE, _E, "Error: %d", error);

	UserActivityBase *instance = static_cast<UserActivityBase*>(userData);
	instance->__handleUpdate(activity, data, timestamp);
}

void UserActivityBase::__handleUpdate(activity_type_e activity, const activity_data_h data, double timestamp)
{
	IF_FAIL_VOID_TAG(activity == __activityType, _E, "Invalid activity: %d", activity);

	Json dataRead;
	dataRead.set(NULL, USER_ACT_EVENT, USER_ACT_DETECTED);

	activity_accuracy_e accuracy = ACTIVITY_ACCURACY_LOW;
	activity_get_accuracy(data, &accuracy);

	switch (accuracy) {
	case ACTIVITY_ACCURACY_HIGH:
		dataRead.set(NULL, USER_ACT_ACCURACY, USER_ACT_HIGH);
		break;
	case ACTIVITY_ACCURACY_MID:
		dataRead.set(NULL, USER_ACT_ACCURACY, USER_ACT_NORMAL);
		break;
	default:
		dataRead.set(NULL, USER_ACT_ACCURACY, USER_ACT_LOW);
		break;
	}

	publish(NULL, ERR_NONE, dataRead);
}

int UserActivityBase::subscribe()
{
	IF_FAIL_RETURN(__activityHandle == NULL, ERR_NONE);

	activity_create(&__activityHandle);
	IF_FAIL_RETURN_TAG(__activityHandle, ERR_OPERATION_FAILED, _E, "Memory allocation failed");

	int ret = activity_start_recognition(__activityHandle, __activityType, __updateCb, this);
	if (ret != ACTIVITY_ERROR_NONE) {
		_E("Recognition starting failed");
		activity_release(__activityHandle);
		__activityHandle = NULL;
		return ERR_OPERATION_FAILED;
	}

	return ERR_NONE;
}

int UserActivityBase::unsubscribe()
{
	IF_FAIL_RETURN(__activityHandle, ERR_NONE);

	activity_stop_recognition(__activityHandle);
	activity_release(__activityHandle);
	__activityHandle = NULL;

	return ERR_NONE;
}
