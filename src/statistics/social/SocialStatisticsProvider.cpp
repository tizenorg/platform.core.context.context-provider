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
#include "SocialStatisticsProvider.h"
#include "DbHandle.h"
#include "LogAggregator.h"

static ctx::ContactLogAggregator *__aggregator = NULL;

ctx::SocialStatisticsProvider *ctx::SocialStatisticsProvider::__instance = NULL;

ctx::SocialStatisticsProvider::SocialStatisticsProvider()
{
}

ctx::SocialStatisticsProvider::~SocialStatisticsProvider()
{
	delete __aggregator;
}

ctx::ContextProviderBase *ctx::SocialStatisticsProvider::create(void *data)
{
	IF_FAIL_RETURN(!__instance, __instance);

	__instance = new(std::nothrow) SocialStatisticsProvider();
	IF_FAIL_RETURN_TAG(__instance, NULL, _E, "Memory allocation failed");

	_I(BLUE("Created"));

	if (!__instance->__init()) {
		destroy(data);
		return NULL;
	}

	return __instance;
}

void ctx::SocialStatisticsProvider::destroy(void *data)
{
	IF_FAIL_VOID(__instance);
	delete __instance;
	__instance = NULL;
	_I(BLUE("Destroyed"));
}

bool ctx::SocialStatisticsProvider::isSupported(const char* subject)
{
	return true;
}

void ctx::SocialStatisticsProvider::submitTriggerItem()
{
	context_manager::registerTriggerItem(SOCIAL_SUBJ_FREQUENCY, OPS_READ,
			"{" TRIG_DEF_RANK "," TRIG_DEF_TOTAL_COUNT "}",
			"{"
				"\"Address\":{\"type\":\"string\"},"
				TRIG_DEF_TIME_OF_DAY "," TRIG_DEF_DAY_OF_WEEK
			"}");
}

bool ctx::SocialStatisticsProvider::__init()
{
	__aggregator = new(std::nothrow) ContactLogAggregator();
	IF_FAIL_RETURN_TAG(__aggregator, false, _E, "Memory allocation failed");
	return true;
}

int ctx::SocialStatisticsProvider::subscribe(const char* subject, ctx::Json option, ctx::Json* requestResult)
{
	return ERR_NOT_SUPPORTED;
}

int ctx::SocialStatisticsProvider::unsubscribe(const char* subject, ctx::Json option)
{
	return ERR_NOT_SUPPORTED;
}

int ctx::SocialStatisticsProvider::read(const char* subject, ctx::Json option, ctx::Json* requestResult)
{
	ctx::SocialDbHandle *handle = new(std::nothrow) ctx::SocialDbHandle();
	IF_FAIL_RETURN_TAG(handle, ERR_OPERATION_FAILED, _E, "Memory allocation failed");

	int err = handle->read(subject, option);
	if (err != ERR_NONE) {
		delete handle;
		return err;
	}

	return ERR_NONE;
}

int ctx::SocialStatisticsProvider::write(const char* subject, ctx::Json data, ctx::Json* requestResult)
{
	return ERR_NOT_SUPPORTED;
}
