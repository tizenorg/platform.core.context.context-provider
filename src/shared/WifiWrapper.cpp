/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd.
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

#include <wifi.h>
#include <Types.h>
#include "WifiWrapper.h"

using namespace ctx;

unsigned int WifiWrapper::__referenceCount = 0;
bool WifiWrapper::__initialized = false;

SO_EXPORT WifiWrapper::WifiWrapper()
{
	if (++__referenceCount == 1)
		__init();

	_D("#instances = %d", __referenceCount);
}

SO_EXPORT WifiWrapper::~WifiWrapper()
{
	if (--__referenceCount == 0)
		__release();

	_D("#instances = %d", __referenceCount);
}

SO_EXPORT void WifiWrapper::__init()
{
	int err = wifi_initialize();
	IF_FAIL_VOID_TAG(err == WIFI_ERROR_NONE, _E, "wifi_initialize() failed");
}

SO_EXPORT void WifiWrapper::__release()
{
	/* Cleanup the remaining jobs */

	wifi_deinitialize();
}
