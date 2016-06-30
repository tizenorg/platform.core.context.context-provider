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

#include <sqlite3.h>
#include <SensorRecorderTypes.h>
#include "../TypesInternal.h"
#include "SleepQuerier.h"

#define PROJECTION \
	KEY_STATE ", " \
	KEY_START_TIME  ", " \
	KEY_END_TIME

using namespace ctx;

SleepQuerier::SleepQuerier(ContextProvider *provider, Json option) :
	Querier(provider, option)
{
}

SleepQuerier::~SleepQuerier()
{
}

int SleepQuerier::queryRaw(int startTime, int endTime)
{
	return query(startTime, endTime);
}

int SleepQuerier::query(int startTime, int endTime)
{
	char *sql = sqlite3_mprintf(
			"SELECT " PROJECTION \
			" FROM " SLEEP_MONITOR_RECORD \
			" WHERE " KEY_END_TIME " > %llu AND " KEY_START_TIME " <= %llu",
			SEC_TO_MS(static_cast<uint64_t>(startTime)), SEC_TO_MS(static_cast<uint64_t>(endTime)));

	int ret = Querier::query(sql);
	sqlite3_free(sql);

	return ret;
}
