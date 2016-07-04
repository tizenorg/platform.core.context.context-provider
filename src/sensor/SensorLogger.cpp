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
#include "TypesInternal.h"
#include "TimeUtil.h"
#include "SensorLogger.h"

using namespace ctx;

SensorLogger::SensorLogger() :
	__lastRemovalTime(0)
{
}

SensorLogger::~SensorLogger()
{
}

void SensorLogger::flushCache(bool force)
{
}

bool SensorLogger::executeQuery(const char *query)
{
	return __dbMgr.execute(0, query, NULL);
}

void SensorLogger::removeExpired(const char *subject, const char *tableName, const char *timeKey)
{
	uint64_t timestamp = TimeUtil::getTime();

	if (timestamp - __lastRemovalTime < SEC_TO_MS(SECONDS_PER_HOUR))
		return;

	char *query = sqlite3_mprintf(
			"DELETE FROM %s WHERE %s < " \
			"%llu - (SELECT MAX(" KEY_RETENTION ") * 1000 FROM " CLIENT_INFO \
			" WHERE " KEY_SUBJECT "='%s')",
			tableName, timeKey, timestamp, subject);

	__dbMgr.execute(0, query, NULL);
	sqlite3_free(query);

	__lastRemovalTime = timestamp;
}
