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

#include <context_mgr.h>
#include "../system_types.h"
#include "charger.h"

GENERATE_PROVIDER_COMMON_IMPL(device_status_charger);

ctx::device_status_charger::device_status_charger()
	: device_status_runtime_info(RUNTIME_INFO_KEY_CHARGER_CONNECTED)
{
}

ctx::device_status_charger::~device_status_charger()
{
}

bool ctx::device_status_charger::is_supported()
{
	return true;
}

void ctx::device_status_charger::submit_trigger_item()
{
	context_manager::register_trigger_item(DEVICE_ST_SUBJ_CHARGER, OPS_SUBSCRIBE | OPS_READ,
			"{" TRIG_BOOL_ITEM_DEF("IsConnected") "}", NULL);
}

void ctx::device_status_charger::handle_update()
{
	bool charger_status = false;

	int ret = runtime_info_get_value_bool(RUNTIME_INFO_KEY_CHARGER_CONNECTED, &charger_status);
	IF_FAIL_VOID_TAG(ret == RUNTIME_INFO_ERROR_NONE, _E, "Getting runtime info failed");

	ctx::Json data_read;
	data_read.set(NULL, DEVICE_ST_IS_CONNECTED, charger_status ? DEVICE_ST_TRUE : DEVICE_ST_FALSE);

	context_manager::publish(DEVICE_ST_SUBJ_CHARGER, NULL, ERR_NONE, data_read);
}

int ctx::device_status_charger::read()
{
	bool charger_status = false;
	ctx::Json data_read;

	int ret = runtime_info_get_value_bool(RUNTIME_INFO_KEY_CHARGER_CONNECTED, &charger_status);
	IF_FAIL_RETURN_TAG(ret == RUNTIME_INFO_ERROR_NONE, ERR_OPERATION_FAILED, _E, "Getting runtime info failed");

	data_read.set(NULL, DEVICE_ST_IS_CONNECTED, charger_status ? DEVICE_ST_TRUE : DEVICE_ST_FALSE);

	ctx::context_manager::reply_to_read(DEVICE_ST_SUBJ_CHARGER, NULL, ERR_NONE, data_read);
	return ERR_NONE;
}
