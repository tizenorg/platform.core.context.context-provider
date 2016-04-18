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

#include "SystemTypes.h"
#include "Psmode.h"

using namespace ctx;

DeviceStatusPsmode::DeviceStatusPsmode() :
	DeviceProviderBase(DEVICE_ST_SUBJ_PSMODE)
{
}

DeviceStatusPsmode::~DeviceStatusPsmode()
{
}

bool DeviceStatusPsmode::isSupported()
{
	return true;
}

void DeviceStatusPsmode::submitTriggerItem()
{
	registerTriggerItem(OPS_SUBSCRIBE | OPS_READ,
			"{" TRIG_BOOL_ITEM_DEF("IsEnabled") "}", NULL);
}

void DeviceStatusPsmode::__updateCb(keynode_t *node, void* userData)
{
	DeviceStatusPsmode *instance = static_cast<DeviceStatusPsmode*>(userData);
	instance->__handleUpdate(node);
}

void DeviceStatusPsmode::__handleUpdate(keynode_t *node)
{
	int status;
	Json dataRead;

	status = vconf_keynode_get_int(node);
	IF_FAIL_VOID_TAG(status >= 0, _E, "Getting state failed");

	dataRead.set(NULL, DEVICE_ST_IS_ENABLED, status == 0 ? DEVICE_ST_FALSE : DEVICE_ST_TRUE);

	publish(NULL, ERR_NONE, dataRead);
}

int DeviceStatusPsmode::subscribe()
{
	int ret = vconf_notify_key_changed(VCONFKEY_SETAPPL_PSMODE, __updateCb, this);
	IF_FAIL_RETURN(ret == VCONF_OK, ERR_OPERATION_FAILED);
	return ERR_NONE;
}

int DeviceStatusPsmode::unsubscribe()
{
	int ret = vconf_ignore_key_changed(VCONFKEY_SETAPPL_PSMODE, __updateCb);
	IF_FAIL_RETURN(ret == VCONF_OK, ERR_OPERATION_FAILED);
	return ERR_NONE;
}

int DeviceStatusPsmode::read()
{
	int mode;
	int ret = vconf_get_int(VCONFKEY_SETAPPL_PSMODE, &mode);
	IF_FAIL_RETURN(ret == VCONF_OK, ERR_OPERATION_FAILED);

	Json dataRead;
	dataRead.set(NULL, DEVICE_ST_IS_ENABLED, mode == 0 ? DEVICE_ST_FALSE : DEVICE_ST_TRUE);

	replyToRead(NULL, ERR_NONE, dataRead);
	return ERR_NONE;
}
