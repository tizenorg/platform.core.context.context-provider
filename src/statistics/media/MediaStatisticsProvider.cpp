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

#include <glib.h>
#include <string>
#include <ContextManager.h>
#include "MediaStatisticsProvider.h"
#include "DbHandle.h"
#include "MediaContentMonitor.h"

static ctx::MediaContentMonitor *__contentMon = NULL;

ctx::MediaStatisticsProvider *ctx::MediaStatisticsProvider::__instance = NULL;

ctx::MediaStatisticsProvider::MediaStatisticsProvider()
{
}

ctx::MediaStatisticsProvider::~MediaStatisticsProvider()
{
	delete __contentMon;
	__contentMon = NULL;
}

ctx::ContextProvider *ctx::MediaStatisticsProvider::create(void *data)
{
	IF_FAIL_RETURN(!__instance, __instance);

	__instance = new(std::nothrow) MediaStatisticsProvider();
	IF_FAIL_RETURN_TAG(__instance, NULL, _E, "Memory allocation failed");

	_I(BLUE("Created"));

	if (!__instance->__init()) {
		destroy(data);
		return NULL;
	}

	return __instance;
}

void ctx::MediaStatisticsProvider::destroy(void *data)
{
	IF_FAIL_VOID(__instance);
	delete __instance;
	__instance = NULL;
	_I(BLUE("Destroyed"));
}

bool ctx::MediaStatisticsProvider::isSupported(const char* subject)
{
	return true;
}

void ctx::MediaStatisticsProvider::submitTriggerItem()
{
	context_manager::registerTriggerItem(MEDIA_SUBJ_MUSIC_FREQUENCY, OPS_READ,
			"{" TRIG_DEF_TOTAL_COUNT "}",
			"{" TRIG_DEF_TIME_OF_DAY "," TRIG_DEF_DAY_OF_WEEK "}");

	context_manager::registerTriggerItem(MEDIA_SUBJ_VIDEO_FREQUENCY, OPS_READ,
			"{" TRIG_DEF_TOTAL_COUNT "}",
			"{" TRIG_DEF_TIME_OF_DAY "," TRIG_DEF_DAY_OF_WEEK "}");
}

bool ctx::MediaStatisticsProvider::__init()
{
	__contentMon = new(std::nothrow) ctx::MediaContentMonitor();
	IF_FAIL_RETURN_TAG(__contentMon, false, _E, "Memory allocation failed");
	return true;
}

int ctx::MediaStatisticsProvider::subscribe(const char* subject, ctx::Json option, ctx::Json* requestResult)
{
	return ERR_NOT_SUPPORTED;
}

int ctx::MediaStatisticsProvider::unsubscribe(const char* subject, ctx::Json option)
{
	return ERR_NOT_SUPPORTED;
}

int ctx::MediaStatisticsProvider::read(const char* subject, ctx::Json option, ctx::Json* requestResult)
{
	MediaDbHandle *handle = new(std::nothrow) MediaDbHandle();
	IF_FAIL_RETURN_TAG(handle, ERR_OPERATION_FAILED, _E, "Memory allocation failed");

	int err = handle->read(subject, option);
	if (err != ERR_NONE) {
		delete handle;
		return err;
	}

	return ERR_NONE;
}

int ctx::MediaStatisticsProvider::write(const char* subject, ctx::Json data, ctx::Json* requestResult)
{
	return ERR_NOT_SUPPORTED;
}
