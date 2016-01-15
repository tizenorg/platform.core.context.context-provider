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

#include "base_rtinfo.h"

ctx::device_status_runtime_info::device_status_runtime_info(runtime_info_key_e key)
	: info_key(key)
{
}

runtime_info_key_e ctx::device_status_runtime_info::get_info_key()
{
	return info_key;
}

void ctx::device_status_runtime_info::update_cb(runtime_info_key_e key, void* user_data)
{
	device_status_runtime_info *instance = static_cast<device_status_runtime_info*>(user_data);
	IF_FAIL_VOID_TAG(key == instance->get_info_key(), _W, "Runtime info key mismatch");
	instance->handle_update();
}

int ctx::device_status_runtime_info::subscribe()
{
	int ret = runtime_info_set_changed_cb(info_key, update_cb, this);
	IF_FAIL_RETURN(ret == RUNTIME_INFO_ERROR_NONE, ERR_OPERATION_FAILED);
	return ERR_NONE;
}

int ctx::device_status_runtime_info::unsubscribe()
{
	int ret = runtime_info_unset_changed_cb(info_key);
	IF_FAIL_RETURN(ret == RUNTIME_INFO_ERROR_NONE, ERR_OPERATION_FAILED);
	return ERR_NONE;
}
