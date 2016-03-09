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
#include "provider_base.h"

ctx::device_provider_base::device_provider_base()
	: being_subscribed(false)
{
}

int ctx::device_provider_base::subscribe(const char *subject, ctx::Json option, ctx::Json *request_result)
{
	IF_FAIL_RETURN(!being_subscribed, ERR_NONE);

	int ret = subscribe();

	if (ret == ERR_NONE)
		being_subscribed = true;
	else
		destroy_self();

	return ret;
}

int ctx::device_provider_base::unsubscribe(const char *subject, ctx::Json option)
{
	int ret = ERR_NONE;

	if (being_subscribed)
		ret = unsubscribe();

	destroy_self();
	return ret;
}

int ctx::device_provider_base::read(const char *subject, ctx::Json option, ctx::Json *request_result)
{
	int ret = read();

	if (!being_subscribed)
		destroy_self();

	return ret;
}

int ctx::device_provider_base::write(const char *subject, ctx::Json data, ctx::Json *request_result)
{
	int ret = write();

	if (!being_subscribed)
		destroy_self();

	return ret;
}

int ctx::device_provider_base::subscribe()
{
	return ERR_NOT_SUPPORTED;
}

int ctx::device_provider_base::unsubscribe()
{
	return ERR_NOT_SUPPORTED;
}

int ctx::device_provider_base::read()
{
	return ERR_NOT_SUPPORTED;
}

int ctx::device_provider_base::write()
{
	return ERR_NOT_SUPPORTED;
}

bool ctx::device_provider_base::get_system_info_bool(const char *key)
{
	bool supported = false;
	int ret = system_info_get_platform_bool(key, &supported);
	IF_FAIL_RETURN_TAG(ret == SYSTEM_INFO_ERROR_NONE, false, _E, "system_info_get_platform_bool() failed");
	return supported;
}
