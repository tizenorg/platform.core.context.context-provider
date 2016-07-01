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
#include "../ClientInfo.h"
#include "../TimeUtil.h"
#include "PressureLogger.h"

#define SAMPLING_INTERVAL	60000
#define BATCH_LATENCY		INT_MAX

#define RESAMPLING_INTERVAL	20000
#define CACHE_LIMIT			20

using namespace ctx;

PressureLogger::PressureLogger()
{
	setSensor(PRESSURE_SENSOR);
	setPowerSave(false);
	setSamplingInterval(SAMPLING_INTERVAL);
	setBatchLatency(BATCH_LATENCY);

	/* Create the log table */
	executeQuery(
			"CREATE TABLE IF NOT EXISTS " PRESSURE_RECORD " (" \
				KEY_UNIV_TIME " INTEGER NOT NULL PRIMARY KEY, " \
				KEY_PRESSURE " REAL NOT NULL" \
			")");

	ClientInfo clientInfo;
	if (clientInfo.exist(SUBJ_SENSOR_PRESSURE))
		start();
}

PressureLogger::~PressureLogger()
{
	stop();
}

bool PressureLogger::start()
{
	IF_FAIL_RETURN_TAG(!isRunning(), true, _D, "Started already");
	_I(GREEN("Start to record"));

	__lastEventTime = 0;
	__cacheCount = 0;
	__resetInsertionQuery();

	return listen();
}

void PressureLogger::stop()
{
	IF_FAIL_VOID_TAG(isRunning(), _D, "Stopped already");
	_I(GREEN("Stop recording"));

	unlisten();
	flushCache(true);
}

void PressureLogger::flushCache(bool force)
{
	IF_FAIL_VOID(force || __cacheCount > CACHE_LIMIT);

	__insertionQuery.resize(__insertionQuery.size() - 1);
	if (__insertionQuery.at(__insertionQuery.size() - 1) == ')')
		executeQuery(__insertionQuery.c_str());

	__cacheCount = 0;
	__resetInsertionQuery();
}

void PressureLogger::onEvent(sensor_data_t *eventData)
{
	uint64_t eventTime = TimeUtil::getTime(eventData->timestamp);
	__record(eventData->values[0], eventTime);
	removeExpired(SUBJ_SENSOR_PRESSURE, PRESSURE_RECORD, KEY_UNIV_TIME);
}

void PressureLogger::__record(float pressure, uint64_t eventTime)
{
	IF_FAIL_VOID(eventTime - __lastEventTime >= RESAMPLING_INTERVAL);

	char buffer[64];
	g_snprintf(buffer, sizeof(buffer), "(%llu, %.5f),", eventTime, pressure);
	_D("[%u] %s", __cacheCount, buffer);

	__insertionQuery += buffer;
	__lastEventTime = eventTime;
	++__cacheCount;

	flushCache();
}

void PressureLogger::__resetInsertionQuery()
{
	__insertionQuery =
		"INSERT INTO " PRESSURE_RECORD \
			" (" KEY_UNIV_TIME ", " KEY_PRESSURE ") VALUES ";
}
