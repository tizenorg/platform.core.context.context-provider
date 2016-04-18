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

#include "AppStatsProvider.h"
#include "DbHandle.h"

using namespace ctx;

AppStatsProvider::AppStatsProvider(const char *subject) :
	ContextProvider(subject)
{
}

AppStatsProvider::~AppStatsProvider()
{
}

void AppStatsProvider::submitTriggerItem()
{
}

int AppStatsProvider::read(Json option, Json *requestResult)
{
	AppDbHandle *handle = new(std::nothrow) AppDbHandle(this);
	IF_FAIL_RETURN_TAG(handle, ERR_OPERATION_FAILED, _E, "Memory allocation failed");

	int err = handle->read(option);
	if (err != ERR_NONE) {
		delete handle;
		return err;
	}

	return ERR_NONE;
}

void AppFreqProvider::submitTriggerItem()
{
	registerTriggerItem(OPS_READ,
			"{" TRIG_DEF_RANK "," TRIG_DEF_TOTAL_COUNT "}",
			"{"
				"\"AppId\":{\"type\":\"string\"},"
				TRIG_DEF_TIME_OF_DAY "," TRIG_DEF_DAY_OF_WEEK
			"}");
}
