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
#include <Json.h>
#include <context_mgr.h>
#include "app_stats_provider.h"
#include "db_handle.h"

#include "db_init.h"
#include "install_monitor.h"
#include "active_window_monitor.h"

static ctx::app_install_monitor *install_mon = NULL;
static ctx::app_use_monitor *launch_mon = NULL;

ctx::app_statistics_provider *ctx::app_statistics_provider::__instance = NULL;

ctx::app_statistics_provider::app_statistics_provider()
{
}

ctx::app_statistics_provider::~app_statistics_provider()
{
	delete install_mon;
	delete launch_mon;
	install_mon = NULL;
	launch_mon = NULL;
}

ctx::context_provider_iface *ctx::app_statistics_provider::create(void *data)
{
	IF_FAIL_RETURN(!__instance, __instance);

	__instance = new(std::nothrow) app_statistics_provider();
	IF_FAIL_RETURN_TAG(__instance, NULL, _E, "Memory allocation failed");

	if (!__instance->init()) {
		destroy(data);
		return NULL;
	}

	_I(BLUE("Created"));
	return __instance;
}

void ctx::app_statistics_provider::destroy(void *data)
{
	IF_FAIL_VOID(__instance);
	delete __instance;
	__instance = NULL;
	_I(BLUE("Destroyed"));
}

bool ctx::app_statistics_provider::is_supported(const char* subject)
{
	return true;
}

void ctx::app_statistics_provider::submit_trigger_item()
{
	context_manager::register_trigger_item(APP_SUBJ_FREQUENCY, OPS_READ,
			"{" TRIG_DEF_RANK "," TRIG_DEF_TOTAL_COUNT "}",
			"{"
				"\"AppId\":{\"type\":\"string\"},"
				TRIG_DEF_TIME_OF_DAY "," TRIG_DEF_DAY_OF_WEEK
			"}");
}

bool ctx::app_statistics_provider::init()
{
	app_db_initializer *initializer = new(std::nothrow) app_db_initializer();
	IF_FAIL_RETURN_TAG(initializer, false, _E, "Memory allocation failed");

	install_mon = new(std::nothrow) ctx::app_install_monitor();
	launch_mon = new(std::nothrow) ctx::app_use_monitor();
	IF_FAIL_CATCH_TAG(install_mon && launch_mon, _E, "Memory allocation failed");
	return true;

CATCH:
	delete install_mon;
	delete launch_mon;
	install_mon = NULL;
	launch_mon = NULL;
	return false;
}

int ctx::app_statistics_provider::subscribe(const char* subject, ctx::Json option, ctx::Json* request_result)
{
	return ERR_NOT_SUPPORTED;
}

int ctx::app_statistics_provider::unsubscribe(const char* subject, ctx::Json option)
{
	return ERR_NOT_SUPPORTED;
}

int ctx::app_statistics_provider::read(const char* subject, ctx::Json option, ctx::Json* request_result)
{
	ctx::app_db_handle *handle = new(std::nothrow) ctx::app_db_handle();
	IF_FAIL_RETURN_TAG(handle, ERR_OPERATION_FAILED, _E, "Memory allocation failed");

	int err = handle->read(subject, option);
	if (err != ERR_NONE) {
		delete handle;
		return err;
	}

	return ERR_NONE;
}

int ctx::app_statistics_provider::write(const char* subject, ctx::Json data, ctx::Json* request_result)
{
	return ERR_NOT_SUPPORTED;
}
