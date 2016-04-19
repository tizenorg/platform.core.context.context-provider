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

#include "Usb.h"

using namespace ctx;

DeviceStatusUsb::DeviceStatusUsb()	:
	DeviceStatusRuntimeInfo(DEVICE_ST_SUBJ_USB, RUNTIME_INFO_KEY_USB_CONNECTED)
{
}

DeviceStatusUsb::~DeviceStatusUsb()
{
}

bool DeviceStatusUsb::isSupported()
{
	return getSystemInfoBool("tizen.org/feature/usb.host");
}

void DeviceStatusUsb::submitTriggerItem()
{
	registerTriggerItem(OPS_SUBSCRIBE | OPS_READ,
			"{" TRIG_BOOL_ITEM_DEF("IsConnected") "}", NULL);
}

void DeviceStatusUsb::handleUpdate()
{
	bool status = false;

	int ret = runtime_info_get_value_bool(RUNTIME_INFO_KEY_USB_CONNECTED, &status);
	IF_FAIL_VOID_TAG(ret == RUNTIME_INFO_ERROR_NONE, _E, "Getting runtime info failed");

	Json dataRead;
	dataRead.set(NULL, DEVICE_ST_IS_CONNECTED, status ? DEVICE_ST_TRUE : DEVICE_ST_FALSE);

	publish(NULL, ERR_NONE, dataRead);
}

int DeviceStatusUsb::read()
{
	bool status = false;
	Json dataRead;

	int ret = runtime_info_get_value_bool(RUNTIME_INFO_KEY_USB_CONNECTED, &status);
	IF_FAIL_RETURN_TAG(ret == RUNTIME_INFO_ERROR_NONE, ERR_OPERATION_FAILED, _E, "Getting runtime info failed");

	dataRead.set(NULL, DEVICE_ST_IS_CONNECTED, status ? DEVICE_ST_TRUE : DEVICE_ST_FALSE);

	replyToRead(NULL, ERR_NONE, dataRead);
	return ERR_NONE;
}
