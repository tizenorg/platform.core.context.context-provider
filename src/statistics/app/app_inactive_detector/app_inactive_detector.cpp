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
#include <json.h>
#include "app_inactive_detector.h"
#include "app_inactive_detector_types.h"
#include "inactive_detector.h"


ctx::app_inactive_detector_provider *ctx::app_inactive_detector_provider::__instance = NULL;

ctx::app_inactive_detector_provider::app_inactive_detector_provider()
{

}

ctx::app_inactive_detector_provider::~app_inactive_detector_provider()
{

}

ctx::context_provider_iface *ctx::app_inactive_detector_provider::create(void *data)
{
	IF_FAIL_RETURN(!__instance, __instance);
	__instance = new(std::nothrow) app_inactive_detector_provider();
	IF_FAIL_RETURN_TAG(__instance, NULL, _E, "Memory allocation failed");
	_I(BLUE("Created"));
	return __instance;
}

void ctx::app_inactive_detector_provider::destroy(void *data)
{
	IF_FAIL_VOID(__instance);
	delete __instance;
	__instance = NULL;
	_I(BLUE("Destroyed"));
}

int ctx::app_inactive_detector_provider::subscribe(const char *subject, ctx::json option, ctx::json* request_result)
{
	return ERR_NOT_SUPPORTED;
}

int ctx::app_inactive_detector_provider::unsubscribe(const char *subject, ctx::json option)
{
	return ERR_NOT_SUPPORTED;
}

int ctx::app_inactive_detector_provider::read(const char *subject, ctx::json option, ctx::json* request_result)
{
	_I(BLUE("Read"));
	_J("Option", option);

	int error = engine->read(subject, option);
	return error == ERR_NONE ? ERR_NONE : error;
}

int ctx::app_inactive_detector_provider::write(const char *subject, ctx::json data, ctx::json* request_result)
{
	return ERR_NOT_SUPPORTED;
}

bool ctx::app_inactive_detector_provider::is_supported()
{
	return true;
}