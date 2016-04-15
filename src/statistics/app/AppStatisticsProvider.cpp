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

#include <Types.h>
#include <Json.h>
#include <ContextManager.h>
#include "AppStatisticsProvider.h"
#include "DbHandle.h"

#include "DbInit.h"
#include "InstallMonitor.h"
#include "ActiveWindowMonitor.h"

static ctx::AppInstallMonitor *__installMon = NULL;
static ctx::AppUseMonitor *__launchMon = NULL;

ctx::AppStatisticsProvider *ctx::AppStatisticsProvider::__instance = NULL;

ctx::AppStatisticsProvider::AppStatisticsProvider()
{
}

ctx::AppStatisticsProvider::~AppStatisticsProvider()
{
	delete __installMon;
	delete __launchMon;
	__installMon = NULL;
	__launchMon = NULL;
}

ctx::ContextProviderBase *ctx::AppStatisticsProvider::create(void *data)
{
	IF_FAIL_RETURN(!__instance, __instance);

	__instance = new(std::nothrow) AppStatisticsProvider();
	IF_FAIL_RETURN_TAG(__instance, NULL, _E, "Memory allocation failed");

	if (!__instance->__init()) {
		destroy(data);
		return NULL;
	}

	_I(BLUE("Created"));
	return __instance;
}

void ctx::AppStatisticsProvider::destroy(void *data)
{
	IF_FAIL_VOID(__instance);
	delete __instance;
	__instance = NULL;
	_I(BLUE("Destroyed"));
}

bool ctx::AppStatisticsProvider::isSupported(const char* subject)
{
	return true;
}

void ctx::AppStatisticsProvider::submitTriggerItem()
{
	context_manager::registerTriggerItem(APP_SUBJ_FREQUENCY, OPS_READ,
			"{" TRIG_DEF_RANK "," TRIG_DEF_TOTAL_COUNT "}",
			"{"
				"\"AppId\":{\"type\":\"string\"},"
				TRIG_DEF_TIME_OF_DAY "," TRIG_DEF_DAY_OF_WEEK
			"}");
}

bool ctx::AppStatisticsProvider::__init()
{
	AppDbInitializer *initializer = new(std::nothrow) AppDbInitializer();
	IF_FAIL_RETURN_TAG(initializer, false, _E, "Memory allocation failed");

	__installMon = new(std::nothrow) ctx::AppInstallMonitor();
	__launchMon = new(std::nothrow) ctx::AppUseMonitor();
	IF_FAIL_CATCH_TAG(__installMon && __launchMon, _E, "Memory allocation failed");
	return true;

CATCH:
	delete __installMon;
	delete __launchMon;
	__installMon = NULL;
	__launchMon = NULL;
	return false;
}

int ctx::AppStatisticsProvider::subscribe(const char* subject, ctx::Json option, ctx::Json* requestResult)
{
	return ERR_NOT_SUPPORTED;
}

int ctx::AppStatisticsProvider::unsubscribe(const char* subject, ctx::Json option)
{
	return ERR_NOT_SUPPORTED;
}

int ctx::AppStatisticsProvider::read(const char* subject, ctx::Json option, ctx::Json* requestResult)
{
	ctx::AppDbHandle *handle = new(std::nothrow) ctx::AppDbHandle();
	IF_FAIL_RETURN_TAG(handle, ERR_OPERATION_FAILED, _E, "Memory allocation failed");

	int err = handle->read(subject, option);
	if (err != ERR_NONE) {
		delete handle;
		return err;
	}

	return ERR_NONE;
}

int ctx::AppStatisticsProvider::write(const char* subject, ctx::Json data, ctx::Json* requestResult)
{
	return ERR_NOT_SUPPORTED;
}
