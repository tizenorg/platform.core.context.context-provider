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
#include "media_stats_provider.h"
#include "db_handle.h"
#include "media_content_monitor.h"

static ctx::media_content_monitor *content_mon = NULL;

ctx::media_statistics_provider *ctx::media_statistics_provider::__instance = NULL;

ctx::media_statistics_provider::media_statistics_provider()
{
}

ctx::media_statistics_provider::~media_statistics_provider()
{
	delete content_mon;
	content_mon = NULL;
}

ctx::ContextProviderBase *ctx::media_statistics_provider::create(void *data)
{
	IF_FAIL_RETURN(!__instance, __instance);

	__instance = new(std::nothrow) media_statistics_provider();
	IF_FAIL_RETURN_TAG(__instance, NULL, _E, "Memory allocation failed");

	_I(BLUE("Created"));

	if (!__instance->init()) {
		destroy(data);
		return NULL;
	}

	return __instance;
}

void ctx::media_statistics_provider::destroy(void *data)
{
	IF_FAIL_VOID(__instance);
	delete __instance;
	__instance = NULL;
	_I(BLUE("Destroyed"));
}

bool ctx::media_statistics_provider::is_supported(const char* subject)
{
	return true;
}

void ctx::media_statistics_provider::submit_trigger_item()
{
	context_manager::registerTriggerItem(MEDIA_SUBJ_MUSIC_FREQUENCY, OPS_READ,
			"{" TRIG_DEF_TOTAL_COUNT "}",
			"{" TRIG_DEF_TIME_OF_DAY "," TRIG_DEF_DAY_OF_WEEK "}");

	context_manager::registerTriggerItem(MEDIA_SUBJ_VIDEO_FREQUENCY, OPS_READ,
			"{" TRIG_DEF_TOTAL_COUNT "}",
			"{" TRIG_DEF_TIME_OF_DAY "," TRIG_DEF_DAY_OF_WEEK "}");
}

bool ctx::media_statistics_provider::init()
{
	content_mon = new(std::nothrow) ctx::media_content_monitor();
	IF_FAIL_RETURN_TAG(content_mon, false, _E, "Memory allocation failed");
	return true;
}

int ctx::media_statistics_provider::subscribe(const char* subject, ctx::Json option, ctx::Json* requestResult)
{
	return ERR_NOT_SUPPORTED;
}

int ctx::media_statistics_provider::unsubscribe(const char* subject, ctx::Json option)
{
	return ERR_NOT_SUPPORTED;
}

int ctx::media_statistics_provider::read(const char* subject, ctx::Json option, ctx::Json* requestResult)
{
	media_db_handle *handle = new(std::nothrow) media_db_handle();
	IF_FAIL_RETURN_TAG(handle, ERR_OPERATION_FAILED, _E, "Memory allocation failed");

	int err = handle->read(subject, option);
	if (err != ERR_NONE) {
		delete handle;
		return err;
	}

	return ERR_NONE;
}

int ctx::media_statistics_provider::write(const char* subject, ctx::Json data, ctx::Json* requestResult)
{
	return ERR_NOT_SUPPORTED;
}
