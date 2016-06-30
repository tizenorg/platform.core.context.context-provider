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
#include "SleepDetector.h"
#include "SleepMonitor.h"
#include "SleepLogger.h"

#define TIME_GAP 5000

using namespace ctx;

SleepLogger::SleepLogger() :
	__sleepDetector(NULL),
	__sleepMonitor(NULL),
	__startTime(0),
	__endTime(0)
{
	/* Create the log table */
	executeQuery(
			"CREATE TABLE IF NOT EXISTS " SLEEP_MONITOR_RECORD " (" \
				KEY_START_TIME " INTEGER NOT NULL, " \
				KEY_END_TIME " INTEGER NOT NULL PRIMARY KEY, " \
				KEY_STATE " INTEGER NOT NULL DEFAULT 1" \
			")");

	ClientInfo clientInfo;
	if (clientInfo.exist(SUBJ_SENSOR_SLEEP_MONITOR))
		start();
}

SleepLogger::~SleepLogger()
{
	stop();
}

bool SleepLogger::start()
{
	if (!__sleepDetector) {
		__sleepDetector = new(std::nothrow) SleepDetector(this);
		__sleepMonitor = new(std::nothrow) SleepMonitor(this);

		if (!__sleepDetector || !__sleepMonitor) {
			_E("Memory allocation failed");

			delete __sleepDetector;
			__sleepDetector = NULL;

			delete __sleepMonitor;
			__sleepMonitor = NULL;

			return false;
		}
	}

	if (__sleepDetector->isRunning()) {
		_D("Started already");
		return true;
	}

	_I(GREEN("Start to record"));

	__startTime = 0;
	__endTime = 0;
	__resetInsertionQuery();

	return dynamic_cast<SleepDetector*>(__sleepDetector)->start();
}

void SleepLogger::stop()
{
	IF_FAIL_VOID_TAG(__sleepDetector, _D, "Stopped already");
	_I(GREEN("Stop recording"));

	dynamic_cast<SleepDetector*>(__sleepDetector)->stop();
	dynamic_cast<SleepMonitor*>(__sleepMonitor)->stop();

	__appendQuery(__startTime, __endTime);
	flush();

	delete __sleepDetector;
	__sleepDetector = NULL;

	delete __sleepMonitor;
	__sleepMonitor = NULL;
}

void SleepLogger::fallAsleep(uint64_t timestamp)
{
	_D("Fall asleep");
	record(timestamp, TimeUtil::getTime(), STATE_SLEEP);
	dynamic_cast<SleepMonitor*>(__sleepMonitor)->start();
}

void SleepLogger::wakeUp(uint64_t timestamp)
{
	_D("Wake up");
	dynamic_cast<SleepMonitor*>(__sleepMonitor)->lazyStop();
}

void SleepLogger::record(uint64_t startTime, uint64_t endTime, int state)
{
	IF_FAIL_VOID(state == STATE_SLEEP);	/* For now, we don't need to record the awaken state */
	IF_FAIL_VOID_TAG(startTime < endTime, _W, "Invalid parameter");

	if (__startTime == 0) {
		__startTime = startTime;
		__endTime = endTime;
		_D("Initial event: %llu ~ %llu", __startTime, __endTime);
		return;
	}

	if (startTime < __endTime + TIME_GAP) {
		__endTime = MAX(endTime, __endTime);
		_D("Merged event: %llu ~ %llu", __startTime, __endTime);
		return;
	}

	__appendQuery(__startTime, __endTime);
	__startTime = startTime;
	__endTime = endTime;
	_D("Reset event: %llu ~ %llu", __startTime, __endTime);
}

void SleepLogger::flush()
{
	__insertionQuery.resize(__insertionQuery.size() - 1);
	if (__insertionQuery.at(__insertionQuery.size() - 1) == ')')
		executeQuery(__insertionQuery.c_str());

	__resetInsertionQuery();
}

void SleepLogger::__resetInsertionQuery()
{
	__insertionQuery =
		"INSERT INTO " SLEEP_MONITOR_RECORD \
			" (" KEY_START_TIME ", " KEY_END_TIME ") VALUES ";
}

void SleepLogger::__appendQuery(uint64_t startTime, uint64_t endTime)
{
	IF_FAIL_VOID(startTime > 0 && endTime > startTime);

	char buffer[64];
	g_snprintf(buffer, sizeof(buffer), "(%llu, %llu),", startTime, endTime);
	__insertionQuery += buffer;
}
