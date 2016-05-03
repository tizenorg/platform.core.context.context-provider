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

#include <Util.h>
#include "Usb.h"

using namespace ctx;

UsbStateProvider::UsbStateProvider()	:
	RuntimeInfoProvider(SUBJ_STATE_USB, RUNTIME_INFO_KEY_USB_CONNECTED)
{
}

UsbStateProvider::~UsbStateProvider()
{
}

bool UsbStateProvider::isSupported()
{
	return util::getSystemInfoBool("tizen.org/feature/usb.host");
}

void UsbStateProvider::handleUpdate()
{
	bool status = false;

	int ret = runtime_info_get_value_bool(RUNTIME_INFO_KEY_USB_CONNECTED, &status);
	IF_FAIL_VOID_TAG(ret == RUNTIME_INFO_ERROR_NONE, _E, "Getting runtime info failed");

	Json dataRead;
	dataRead.set(NULL, KEY_IS_CONNECTED, status ? VAL_TRUE : VAL_FALSE);

	publish(NULL, ERR_NONE, dataRead);
}

int UsbStateProvider::read()
{
	bool status = false;
	Json dataRead;

	int ret = runtime_info_get_value_bool(RUNTIME_INFO_KEY_USB_CONNECTED, &status);
	IF_FAIL_RETURN_TAG(ret == RUNTIME_INFO_ERROR_NONE, ERR_OPERATION_FAILED, _E, "Getting runtime info failed");

	dataRead.set(NULL, KEY_IS_CONNECTED, status ? VAL_TRUE : VAL_FALSE);

	replyToRead(NULL, ERR_NONE, dataRead);
	return ERR_NONE;
}
