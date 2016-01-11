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
#include "system_types.h"
#include "psmode.h"

GENERATE_PROVIDER_COMMON_IMPL(device_status_psmode);

ctx::device_status_psmode::device_status_psmode()
{
}

ctx::device_status_psmode::~device_status_psmode()
{
}

bool ctx::device_status_psmode::is_supported()
{
	return true;
}

void ctx::device_status_psmode::submit_trigger_item()
{
	context_manager::register_trigger_item(DEVICE_ST_SUBJ_PSMODE, OPS_SUBSCRIBE | OPS_READ,
			"{" TRIG_BOOL_ITEM_DEF("IsEnabled") "}", NULL);
}

void ctx::device_status_psmode::update_cb(keynode_t *node, void* user_data)
{
	device_status_psmode *instance = static_cast<device_status_psmode*>(user_data);
	instance->handle_update(node);
}

void ctx::device_status_psmode::handle_update(keynode_t *node)
{
	int status;
	ctx::json data_read;

	status = vconf_keynode_get_int(node);
	IF_FAIL_VOID_TAG(status >= 0, _E, "Getting state failed");

	data_read.set(NULL, DEVICE_ST_IS_ENABLED, status == 0 ? DEVICE_ST_FALSE : DEVICE_ST_TRUE);

	context_manager::publish(DEVICE_ST_SUBJ_PSMODE, NULL, ERR_NONE, data_read);
}

int ctx::device_status_psmode::subscribe()
{
	int ret = vconf_notify_key_changed(VCONFKEY_SETAPPL_PSMODE, update_cb, this);
	IF_FAIL_RETURN(ret == VCONF_OK, ERR_OPERATION_FAILED);
	return ERR_NONE;
}

int ctx::device_status_psmode::unsubscribe()
{
	int ret = vconf_ignore_key_changed(VCONFKEY_SETAPPL_PSMODE, update_cb);
	IF_FAIL_RETURN(ret == VCONF_OK, ERR_OPERATION_FAILED);
	return ERR_NONE;
}

int ctx::device_status_psmode::read()
{
	int mode;
	int ret = vconf_get_int(VCONFKEY_SETAPPL_PSMODE, &mode);
	IF_FAIL_RETURN(ret == VCONF_OK, ERR_OPERATION_FAILED);

	ctx::json data_read;
	data_read.set(NULL, DEVICE_ST_IS_ENABLED, mode == 0 ? DEVICE_ST_FALSE : DEVICE_ST_TRUE);

	ctx::context_manager::reply_to_read(DEVICE_ST_SUBJ_PSMODE, NULL, ERR_NONE, data_read);
	return ERR_NONE;
}
