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
#include "ClientInfo.h"
#include "PressureLogger.h"

#define INSERTION_THRESHOLD	20000
#define SAMPLING_INTERVAL	60000
#define BATCH_LATENCY		INT_MAX
#define MAX_QUERY_LENGTH	1000

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
}

bool PressureLogger::start()
{
	if (SensorProxy::isRunning())
		return true;

	_I(GREEN("Start to record"));

	__lastInsertionTime = getTime();
	__resetInsertionQuery();

	return SensorProxy::start();
}

void PressureLogger::stop()
{
	_I(GREEN("Stop recording"));

	SensorProxy::stop();
}

void PressureLogger::onEvent(sensor_data_t *eventData)
{
	uint64_t receivedTime = getTime();
	__record(eventData, receivedTime);
	removeExpired(SUBJ_SENSOR_PRESSURE, PRESSURE_RECORD, KEY_UNIV_TIME);
}

void PressureLogger::__record(sensor_data_t *eventData, uint64_t receivedTime)
{
	char buffer[64];
	g_snprintf(buffer, sizeof(buffer), "(%llu, %.5f),",
			getTime(eventData->timestamp), eventData->values[0]);

	__insertionQuery += buffer;

	if (receivedTime - __lastInsertionTime < INSERTION_THRESHOLD && __insertionQuery.size() < MAX_QUERY_LENGTH)
		return;

	__insertionQuery.resize(__insertionQuery.size() - 1);
	if (__insertionQuery.at(__insertionQuery.size() - 1) == ')')
		executeQuery(__insertionQuery.c_str());

	__lastInsertionTime = receivedTime;
	__resetInsertionQuery();
}

void PressureLogger::__resetInsertionQuery()
{
	__insertionQuery =
		"INSERT INTO " PRESSURE_RECORD \
			" (" KEY_UNIV_TIME ", " KEY_PRESSURE ") VALUES ";
}
