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
#include <json.h>
#include <context_mgr.h>
#include "social_stats_provider.h"
#include "db_handle.h"
#include "log_aggregator.h"

static ctx::contact_log_aggregator *aggregator = NULL;

ctx::social_statistics_provider *ctx::social_statistics_provider::__instance = NULL;

ctx::social_statistics_provider::social_statistics_provider()
{
}

ctx::social_statistics_provider::~social_statistics_provider()
{
	delete aggregator;
}

ctx::context_provider_iface *ctx::social_statistics_provider::create(void *data)
{
	IF_FAIL_RETURN(!__instance, __instance);

	__instance = new(std::nothrow) social_statistics_provider();
	IF_FAIL_RETURN_TAG(__instance, NULL, _E, "Memory allocation failed");

	_I(BLUE("Created"));

	if (!__instance->init()) {
		destroy(data);
		return NULL;
	}

	return __instance;
}

void ctx::social_statistics_provider::destroy(void *data)
{
	IF_FAIL_VOID(__instance);
	delete __instance;
	__instance = NULL;
	_I(BLUE("Destroyed"));
}

bool ctx::social_statistics_provider::is_supported(const char* subject)
{
	return true;
}

void ctx::social_statistics_provider::submit_trigger_item()
{
	context_manager::register_trigger_item(SOCIAL_SUBJ_FREQUENCY, OPS_READ,
			"{" TRIG_DEF_RANK "," TRIG_DEF_TOTAL_COUNT "}",
			"{"
				"\"Address\":{\"type\":\"string\"},"
				TRIG_DEF_TIME_OF_DAY "," TRIG_DEF_DAY_OF_WEEK
			"}");
}

bool ctx::social_statistics_provider::init()
{
	aggregator = new(std::nothrow) contact_log_aggregator();
	IF_FAIL_RETURN_TAG(aggregator, false, _E, "Memory allocation failed");
	return true;
}

int ctx::social_statistics_provider::subscribe(const char* subject, ctx::json option, ctx::json* request_result)
{
	return ERR_NOT_SUPPORTED;
}

int ctx::social_statistics_provider::unsubscribe(const char* subject, ctx::json option)
{
	return ERR_NOT_SUPPORTED;
}

int ctx::social_statistics_provider::read(const char* subject, ctx::json option, ctx::json* request_result)
{
	ctx::social_db_handle *handle = new(std::nothrow) ctx::social_db_handle();
	IF_FAIL_RETURN_TAG(handle, ERR_OPERATION_FAILED, _E, "Memory allocation failed");

	int err = handle->read(subject, option);
	if (err != ERR_NONE) {
		delete handle;
		return err;
	}

	return ERR_NONE;
}

int ctx::social_statistics_provider::write(const char* subject, ctx::json data, ctx::json* request_result)
{
	return ERR_NOT_SUPPORTED;
}
