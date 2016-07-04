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
#include "PedometerLogger.h"

#define MIN_INTERVAL	60000

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
	stop();
}

bool PedometerLogger::start()
{
	IF_FAIL_RETURN_TAG(!isRunning(), true, _D, "Started already");
	_I(GREEN("Start to record"));

	if (listen()) {
		flush();
		return true;
	}

	return false;
}

void PedometerLogger::stop()
{
	IF_FAIL_VOID_TAG(isRunning(), _D, "Stopped already");
	_I(GREEN("Stop recording"));

	unlisten();
	__firstEvent = true;
}

void PedometerLogger::flushCache(bool force)
{
	DataRecord record;
	record.walkSteps = __lastRecord.walkSteps - __baseline.walkSteps;
	record.runSteps  = __lastRecord.runSteps - __baseline.runSteps;
	record.distance  = __lastRecord.distance - __baseline.distance;
	record.calories  = __lastRecord.calories - __baseline.calories;

	if (record.walkSteps + record.runSteps > 0) {
		char *query = sqlite3_mprintf(
				"INSERT INTO " PEDOMETER_RECORD " (" \
					KEY_START_TIME ", " \
					KEY_END_TIME ", " \
					KEY_WALK_STEPS ", " \
					KEY_RUN_STEPS ", " \
					KEY_DISTANCE ", " \
					KEY_CALORIES ") " \
					"VALUES (%llu, %llu, %u, %u, %.3f, %.3f)",
				__baseline.timestamp, __lastRecord.timestamp, record.walkSteps, record.runSteps, record.distance, record.calories);
		executeQuery(query);
		sqlite3_free(query);
	}

	__baseline = __lastRecord;
}

void PedometerLogger::onEvent(sensor_data_t *eventData)
{
	sensor_pedometer_data_t *pedometerData = reinterpret_cast<sensor_pedometer_data_t*>(eventData);
	uint64_t timestamp = TimeUtil::getTime();

	if (__firstEvent) {
		__firstEvent = false;
		__setRecord(__lastRecord, timestamp, pedometerData);
		__baseline = __lastRecord;

		_SD("Baseline: %u, %u, %.3f, %.3f",
				__baseline.walkSteps, __baseline.runSteps, __baseline.distance, __baseline.calories);

	} else if (pedometerData->diffs_count == 0) {
		_SD("Single: %.0f, %.0f, %.3f, %.3f",
				eventData->values[1], eventData->values[2], eventData->values[3], eventData->values[4]);
		__recordSingle(pedometerData, timestamp);

	} else {
		_SD("Batch [%d]: %.0f, %.0f, %.3f, %.3f",
				pedometerData->diffs_count,
				eventData->values[1], eventData->values[2], eventData->values[3], eventData->values[4]);
		__recordBatch(pedometerData, timestamp);
	}

	removeExpired(SUBJ_SENSOR_PEDOMETER, PEDOMETER_RECORD, KEY_END_TIME);
}

void PedometerLogger::__recordSingle(sensor_pedometer_data_t *eventData, uint64_t timestamp)
{
	if (timestamp - __baseline.timestamp >= MIN_INTERVAL)
		flushCache();

	__setRecord(__lastRecord, timestamp, eventData);
}

void PedometerLogger::__recordBatch(sensor_pedometer_data_t *eventData, uint64_t timestamp)
{
	flushCache();

	std::string query("INSERT INTO " PEDOMETER_RECORD " (" \
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

	__setRecord(__lastRecord, timestamp, eventData);
	__baseline = __lastRecord;
}

void PedometerLogger::__setRecord(DataRecord &record, uint64_t timestamp, sensor_pedometer_data_t *eventData)
{
	record.timestamp = timestamp;
	record.walkSteps = static_cast<unsigned int>(eventData->values[1]);
	record.runSteps  = static_cast<unsigned int>(eventData->values[2]);
	record.distance  = eventData->values[3];
	record.calories  = eventData->values[4];
}
