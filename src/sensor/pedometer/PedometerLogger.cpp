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
#include "PedometerLogger.h"

using namespace ctx;

PedometerLogger::PedometerLogger() :
	__firstEvent(true)
{
	setSensor(HUMAN_PEDOMETER_SENSOR);
	setPowerSave(false);

	/* Create the log table */
	executeQuery(
			"CREATE TABLE IF NOT EXISTS " PEDOMETER_RECORD " (" \
				KEY_START_TIME " INTEGER NOT NULL, " \
				KEY_END_TIME " INTEGER NOT NULL PRIMARY KEY, " \
				KEY_WALK_STEPS " INTEGER NOT NULL, " \
				KEY_RUN_STEPS " INTEGER NOT NULL, " \
				KEY_DISTANCE " REAL NOT NULL, " \
				KEY_CALORIES " REAL NOT NULL" \
			")");

	ClientInfo clientInfo;
	if (clientInfo.exist(SUBJ_SENSOR_PEDOMETER))
		start();
}

PedometerLogger::~PedometerLogger()
{
}

bool PedometerLogger::start()
{
	if (SensorProxy::isRunning())
		return true;

	_I(GREEN("Start to record"));

	if (SensorProxy::start()) {
		flush();
		return true;
	}

	return false;
}

void PedometerLogger::stop()
{
	_I(GREEN("Stop recording"));

	SensorProxy::stop();
	__firstEvent = true;
}

void PedometerLogger::onEvent(sensor_data_t *eventData)
{
	sensor_pedometer_data_t *pedometerData = reinterpret_cast<sensor_pedometer_data_t*>(eventData);
	uint64_t timestamp = getTime();

	if (__firstEvent) {
		_D("Baseline event");
		__firstEvent = false;
	} else if (pedometerData->diffs_count == 0) {
		_D("Single event");
		__recordSingle(pedometerData, timestamp);
	} else {
		_D("Batch event");
		__recordBatch(pedometerData, timestamp);
	}

	__baseline.timestamp = timestamp;
	__baseline.walkSteps = eventData->values[1];
	__baseline.runSteps  = eventData->values[2];
	__baseline.distance  = eventData->values[3];
	__baseline.calories  = eventData->values[4];

	_D("Baseline: %u, %u, %.3f, %.3f",
			__baseline.walkSteps, __baseline.runSteps, __baseline.distance, __baseline.calories);

	removeExpired(SUBJ_SENSOR_PEDOMETER, PEDOMETER_RECORD, KEY_END_TIME);
}

void PedometerLogger::__recordSingle(sensor_pedometer_data_t *eventData, uint64_t timestamp)
{
	DataRecord record;
	record.walkSteps = static_cast<unsigned int>(eventData->values[1]) - __baseline.walkSteps;
	record.runSteps  = static_cast<unsigned int>(eventData->values[2]) - __baseline.runSteps;
	record.distance  = eventData->values[3] - __baseline.distance;
	record.calories  = eventData->values[4] - __baseline.calories;

	IF_FAIL_VOID_TAG(record.walkSteps + record.runSteps > 0, _D, "Skipping zero-count event");

	char *query = sqlite3_mprintf(
			"INSERT INTO " PEDOMETER_RECORD "(" \
				KEY_START_TIME ", " \
				KEY_END_TIME ", " \
				KEY_WALK_STEPS ", " \
				KEY_RUN_STEPS ", " \
				KEY_DISTANCE ", " \
				KEY_CALORIES ") " \
				"VALUES (%llu, %llu, %u, %u, %.3f, %.3f)",
			__baseline.timestamp, timestamp, record.walkSteps, record.runSteps, record.distance, record.calories);
	executeQuery(query);
	sqlite3_free(query);
}

void PedometerLogger::__recordBatch(sensor_pedometer_data_t *eventData, uint64_t timestamp)
{
	std::string query("INSERT INTO " PEDOMETER_RECORD "(" \
				KEY_START_TIME ", " \
				KEY_END_TIME ", " \
				KEY_WALK_STEPS ", " \
				KEY_RUN_STEPS ", " \
				KEY_DISTANCE ", " \
				KEY_CALORIES ") VALUES ");
	char buffer[256];

	for (int i = 0; i < eventData->diffs_count; ++i) {
		if (eventData->diffs[i].walk_steps + eventData->diffs[i].run_steps == 0) {
			_D("Skipping zero-count event");
			continue;
		}

		/* TODO: check the timestamps.. they look strange.. */
		g_snprintf(buffer, sizeof(buffer), "(%llu, %llu, %d, %d, %.3f, %.3f),",
				i == 0 ? __baseline.timestamp : SEC_TO_MS(static_cast<uint64_t>(eventData->diffs[i-1].timestamp)),
				SEC_TO_MS(static_cast<uint64_t>(eventData->diffs[i].timestamp)),
				eventData->diffs[i].walk_steps,
				eventData->diffs[i].run_steps,
				eventData->diffs[i].distance,
				eventData->diffs[i].calories);
		query += buffer;
	}

	query.resize(query.size() - 1);
	IF_FAIL_VOID_TAG(query.at(query.size() - 1) == ')', _D, "No records");

	executeQuery(query.c_str());
}
