/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd.
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

#include <Types.h>
#include <Json.h>
#include "AppInactiveDetector.h"
#include "AppInactiveDetectorTypes.h"
#include "InactiveDetector.h"

ctx::AppInactiveDetectorProvider *ctx::AppInactiveDetectorProvider::__instance = NULL;

ctx::AppInactiveDetectorProvider::AppInactiveDetectorProvider()
	: ContextProvider(APP_INACTIVE_SUBJ_GET_APPS_INACTIVE)
{
}

ctx::AppInactiveDetectorProvider::~AppInactiveDetectorProvider()
{
}

ctx::ContextProvider *ctx::AppInactiveDetectorProvider::create(void *data)
{
	IF_FAIL_RETURN(!__instance, __instance);
	__instance = new(std::nothrow) AppInactiveDetectorProvider();
	IF_FAIL_RETURN_TAG(__instance, NULL, _E, "Memory allocation failed");
	_I(BLUE("Created"));
	return __instance;
}

void ctx::AppInactiveDetectorProvider::destroy(void *data)
{
	IF_FAIL_VOID(__instance);
	delete __instance;
	__instance = NULL;
	_I(BLUE("Destroyed"));
}

int ctx::AppInactiveDetectorProvider::subscribe(ctx::Json option, ctx::Json* requestResult)
{
	return ERR_NOT_SUPPORTED;
}

int ctx::AppInactiveDetectorProvider::unsubscribe(ctx::Json option)
{
	return ERR_NOT_SUPPORTED;
}

int ctx::AppInactiveDetectorProvider::read(ctx::Json option, ctx::Json* requestResult)
{
	_I(BLUE("Read"));
	_J("Option", option);

	int error = __engine->read(APP_INACTIVE_SUBJ_GET_APPS_INACTIVE, option);
	return error == ERR_NONE ? ERR_NONE : error;
}

int ctx::AppInactiveDetectorProvider::write(ctx::Json data, ctx::Json* requestResult)
{
	return ERR_NOT_SUPPORTED;
}

bool ctx::AppInactiveDetectorProvider::isSupported()
{
	return true;
}
