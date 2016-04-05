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

#include "RuntimeInfoBase.h"

ctx::DeviceStatusRuntimeInfo::DeviceStatusRuntimeInfo(runtime_info_key_e key) :
	__infoKey(key)
{
}

runtime_info_key_e ctx::DeviceStatusRuntimeInfo::__getInfoKey()
{
	return __infoKey;
}

void ctx::DeviceStatusRuntimeInfo::updateCb(runtime_info_key_e runtimeKey, void* userData)
{
	DeviceStatusRuntimeInfo *instance = static_cast<DeviceStatusRuntimeInfo*>(userData);
	IF_FAIL_VOID_TAG(runtimeKey == instance->__getInfoKey(), _W, "Runtime info key mismatch");
	instance->handleUpdate();
}

int ctx::DeviceStatusRuntimeInfo::subscribe()
{
	int ret = runtime_info_set_changed_cb(__infoKey, updateCb, this);
	IF_FAIL_RETURN(ret == RUNTIME_INFO_ERROR_NONE, ERR_OPERATION_FAILED);
	return ERR_NONE;
}

int ctx::DeviceStatusRuntimeInfo::unsubscribe()
{
	int ret = runtime_info_unset_changed_cb(__infoKey);
	IF_FAIL_RETURN(ret == RUNTIME_INFO_ERROR_NONE, ERR_OPERATION_FAILED);
	return ERR_NONE;
}
