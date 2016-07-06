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

#include <climits>
#include <sqlite3.h>
#include <SensorRecorderTypes.h>
#include "../TypesInternal.h"
#include "../ClientInfo.h"
#include "../TimeUtil.h"
#include "HeartRateLogger.h"

#define SAMPLING_INTERVAL	200		/* ms */
#define VALID_HR_LB			30		/* BPM */
#define MIN_VALID_COUNT		3
#define MAX_TIMER_INTERVAL	1440	/* minutes */
#define MEASURING_LIMIT		10000	/* ms */

using namespace ctx;

HeartRateLogger::HeartRateLogger() :
	__timerMgr(NULL),
	__timerId(-1),
	__timerInterval(INT_MAX),
	__expiredTime(0)
{
	setSensor(HRM_SENSOR);
	setPowerSave(false);
	setSamplingInterval(SAMPLING_INTERVAL);

	/* Create the log table */
	executeQuery(
			"CREATE TABLE IF NOT EXISTS " HEART_RATE_RECORD " (" \
				KEY_UNIV_TIME " INTEGER NOT NULL PRIMARY KEY, " \
				KEY_HEART_RATE " REAL NOT NULL" \
			")");

	ClientInfo clientInfo;
	if (clientInfo.exist(SUBJ_SENSOR_HEART_RATE))
		start();
}

HeartRateLogger::~HeartRateLogger()
{
	stop();
}

bool HeartRateLogger::start()
{
	std::vector<Json> options;
	ClientInfo clientInfo;
	float interval = MAX_TIMER_INTERVAL;

	if (clientInfo.get(SUBJ_SENSOR_HEART_RATE, options) != ERR_NONE)
		return false;

	clientInfo.getParam(options, KEY_INTERVAL, &interval, NULL);

	if (!__timerMgr) {
		__timerMgr = new(std::nothrow) TimerManager;
		IF_FAIL_RETURN_TAG(__timerMgr, false, _E, "Memory allocation failed");
	}

	if (interval == __timerInterval)
		return true;

	__timerInterval = interval;

	_I(GREEN("Start to record (at every %d minutes)"), __timerInterval);

	if (__timerId > 0)
		__timerMgr->remove(__timerId);

	__timerId = __timerMgr->setFor(__timerInterval, this);

	if (__timerId < 0) {
		_E("Setting timer failed");
		__timerInterval = INT_MAX;
		return false;
	}

	return true;
}

void HeartRateLogger::stop()
{
	_I(GREEN("Stop recording"));

	if (__timerMgr)
		delete __timerMgr;

	__timerMgr = NULL;
	__timerId = -1;
	__timerInterval = INT_MAX;

	unlisten();
}

void HeartRateLogger::flushCache(bool force)
{
}

bool HeartRateLogger::onTimerExpired(int timerId)
{
	IF_FAIL_RETURN(!isRunning(), true);

	if (!listen())
		_W("Starting sensor failed");

	__expiredTime = TimeUtil::getTime();
	_I("Measuring starts at %llu", __expiredTime);
	return true;
}

void HeartRateLogger::onEvent(sensor_data_t *eventData)
{
	static int validCnt = 0;
	uint64_t receivedTime = TimeUtil::getTime();

	IF_FAIL_CATCH_TAG(receivedTime - __expiredTime < MEASURING_LIMIT, _I, "Measuring failed (timeout)");

	if (eventData->values[0] > VALID_HR_LB)
		++validCnt;
	else
		validCnt = 0;

	if (validCnt < MIN_VALID_COUNT)
		return;

	__record(eventData->values[0], receivedTime);

CATCH:
	removeExpired(SUBJ_SENSOR_HEART_RATE, HEART_RATE_RECORD, KEY_UNIV_TIME);
	unlisten();
}

void HeartRateLogger::__record(float heartrate, uint64_t eventTime)
{
	char *query = sqlite3_mprintf(
			"INSERT INTO " HEART_RATE_RECORD \
				" (" KEY_UNIV_TIME ", " KEY_HEART_RATE ") VALUES (%llu, %.3f)",
			eventTime, heartrate);
	executeQuery(query);
	sqlite3_free(query);
}
