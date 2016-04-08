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
#include "DeviceProviderBase.h"

ctx::DeviceProviderBase::DeviceProviderBase() :
	__beingSubscribed(false)
{
}

int ctx::DeviceProviderBase::subscribe(const char *subject, ctx::Json option, ctx::Json *requestResult)
{
	IF_FAIL_RETURN(!__beingSubscribed, ERR_NONE);

	int ret = subscribe();

	if (ret == ERR_NONE)
		__beingSubscribed = true;
	else
		destroySelf();

	return ret;
}

int ctx::DeviceProviderBase::unsubscribe(const char *subject, ctx::Json option)
{
	int ret = ERR_NONE;

	if (__beingSubscribed)
		ret = unsubscribe();

	destroySelf();
	return ret;
}

int ctx::DeviceProviderBase::read(const char *subject, ctx::Json option, ctx::Json *requestResult)
{
	int ret = read();

	if (!__beingSubscribed)
		destroySelf();

	return ret;
}

int ctx::DeviceProviderBase::write(const char *subject, ctx::Json data, ctx::Json *requestResult)
{
	int ret = write();

	if (!__beingSubscribed)
		destroySelf();

	return ret;
}

int ctx::DeviceProviderBase::subscribe()
{
	return ERR_NOT_SUPPORTED;
}

int ctx::DeviceProviderBase::unsubscribe()
{
	return ERR_NOT_SUPPORTED;
}

int ctx::DeviceProviderBase::read()
{
	return ERR_NOT_SUPPORTED;
}

int ctx::DeviceProviderBase::write()
{
	return ERR_NOT_SUPPORTED;
}

bool ctx::DeviceProviderBase::getSystemInfoBool(const char *key)
{
	bool supported = false;
	int ret = system_info_get_platform_bool(key, &supported);
	IF_FAIL_RETURN_TAG(ret == SYSTEM_INFO_ERROR_NONE, false, _E, "system_info_get_platform_bool() failed");
	return supported;
}
