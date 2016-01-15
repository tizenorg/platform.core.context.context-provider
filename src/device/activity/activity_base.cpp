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
#include <context_mgr.h>
#include "activity_types.h"
#include "activity_base.h"

ctx::user_activity_base::user_activity_base(const char *subj, activity_type_e type)
	: activity_type(type)
	, activity_handle(NULL)
	, subject(subj)
{
}

ctx::user_activity_base::~user_activity_base()
{
	if (activity_handle)
		activity_release(activity_handle);
}

void ctx::user_activity_base::event_cb(activity_type_e activity, const activity_data_h data, double timestamp, activity_error_e error, void* user_data)
{
	IF_FAIL_VOID_TAG(error == ACTIVITY_ERROR_NONE, _E, "Error: %d", error);

	user_activity_base *instance = static_cast<user_activity_base*>(user_data);
	instance->handle_event(activity, data, timestamp);
}

void ctx::user_activity_base::handle_event(activity_type_e activity, const activity_data_h data, double timestamp)
{
	IF_FAIL_VOID_TAG(activity == activity_type, _E, "Invalid activity: %d", activity);

	ctx::json data_read;
	data_read.set(NULL, USER_ACT_EVENT, USER_ACT_DETECTED);

	activity_accuracy_e accuracy = ACTIVITY_ACCURACY_LOW;
	activity_get_accuracy(data, &accuracy);

	switch (accuracy) {
	case ACTIVITY_ACCURACY_HIGH:
		data_read.set(NULL, USER_ACT_ACCURACY, USER_ACT_HIGH);
		break;
	case ACTIVITY_ACCURACY_MID:
		data_read.set(NULL, USER_ACT_ACCURACY, USER_ACT_NORMAL);
		break;
	default:
		data_read.set(NULL, USER_ACT_ACCURACY, USER_ACT_LOW);
		break;
	}

	context_manager::publish(subject.c_str(), NULL, ERR_NONE, data_read);
}

int ctx::user_activity_base::subscribe()
{
	IF_FAIL_RETURN(activity_handle == NULL, ERR_NONE);

	_D("Starting to monitor %s", subject.c_str());

	activity_create(&activity_handle);
	IF_FAIL_RETURN_TAG(activity_handle, ERR_OPERATION_FAILED, _E, "Memory allocation failed");

	int ret = activity_start_recognition(activity_handle, activity_type, event_cb, this);
	if (ret != ACTIVITY_ERROR_NONE) {
		_E("Recognition starting failed");
		activity_release(activity_handle);
		activity_handle = NULL;
		return ERR_OPERATION_FAILED;
	}

	return ERR_NONE;
}

int ctx::user_activity_base::unsubscribe()
{
	IF_FAIL_RETURN(activity_handle, ERR_NONE);

	_D("Stop monitoring %s", subject.c_str());

	activity_stop_recognition(activity_handle);
	activity_release(activity_handle);
	activity_handle = NULL;

	return ERR_NONE;
}
