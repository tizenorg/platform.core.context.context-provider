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

#include <ContextManager.h>
#include "../SystemTypes.h"
#include "Charger.h"

GENERATE_PROVIDER_COMMON_IMPL(DeviceStatusCharger);

ctx::DeviceStatusCharger::DeviceStatusCharger() :
	DeviceStatusRuntimeInfo(RUNTIME_INFO_KEY_CHARGER_CONNECTED)
{
}

ctx::DeviceStatusCharger::~DeviceStatusCharger()
{
}

bool ctx::DeviceStatusCharger::isSupported()
{
	return true;
}

void ctx::DeviceStatusCharger::submitTriggerItem()
{
	context_manager::registerTriggerItem(DEVICE_ST_SUBJ_CHARGER, OPS_SUBSCRIBE | OPS_READ,
			"{" TRIG_BOOL_ITEM_DEF("IsConnected") "}", NULL);
}

void ctx::DeviceStatusCharger::handleUpdate()
{
	bool chargerStatus = false;

	int ret = runtime_info_get_value_bool(RUNTIME_INFO_KEY_CHARGER_CONNECTED, &chargerStatus);
	IF_FAIL_VOID_TAG(ret == RUNTIME_INFO_ERROR_NONE, _E, "Getting runtime info failed");

	ctx::Json dataRead;
	dataRead.set(NULL, DEVICE_ST_IS_CONNECTED, chargerStatus ? DEVICE_ST_TRUE : DEVICE_ST_FALSE);

	context_manager::publish(DEVICE_ST_SUBJ_CHARGER, NULL, ERR_NONE, dataRead);
}

int ctx::DeviceStatusCharger::read()
{
	bool chargerStatus = false;
	ctx::Json dataRead;

	int ret = runtime_info_get_value_bool(RUNTIME_INFO_KEY_CHARGER_CONNECTED, &chargerStatus);
	IF_FAIL_RETURN_TAG(ret == RUNTIME_INFO_ERROR_NONE, ERR_OPERATION_FAILED, _E, "Getting runtime info failed");

	dataRead.set(NULL, DEVICE_ST_IS_CONNECTED, chargerStatus ? DEVICE_ST_TRUE : DEVICE_ST_FALSE);

	ctx::context_manager::replyToRead(DEVICE_ST_SUBJ_CHARGER, NULL, ERR_NONE, dataRead);
	return ERR_NONE;
}
