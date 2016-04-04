/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd.
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

#include <ContextManager.h>
#include <TimerManager.h>
#include "system_types.h"
#include "time.h"

GENERATE_PROVIDER_COMMON_IMPL(device_status_time);

ctx::device_status_time::device_status_time()
{
}

ctx::device_status_time::~device_status_time()
{
}

bool ctx::device_status_time::is_supported()
{
	return true;
}

void ctx::device_status_time::submit_trigger_item()
{
	context_manager::registerTriggerItem(DEVICE_ST_SUBJ_TIME, OPS_READ,
			"{"
				"\"TimeOfDay\":{\"type\":\"integer\",\"min\":0,\"max\":1439},"
				"\"DayOfWeek\":{\"type\":\"string\",\"values\":[\"Mon\",\"Tue\",\"Wed\",\"Thu\",\"Fri\",\"Sat\",\"Sun\",\"Weekday\",\"Weekend\"]},"
				"\"DayOfMonth\":{\"type\":\"integer\",\"min\":1,\"max\":31}"
			"}",
			NULL);
}

int ctx::device_status_time::subscribe()
{
	return ERR_NOT_SUPPORTED;
}

int ctx::device_status_time::unsubscribe()
{
	return ERR_NOT_SUPPORTED;
}

int ctx::device_status_time::read()
{
	time_t rawtime;
	struct tm timeinfo;

	time(&rawtime);
	tzset();
	localtime_r(&rawtime, &timeinfo);

	int day_of_month = timeinfo.tm_mday;
	int minute_of_day = timeinfo.tm_hour * 60 + timeinfo.tm_min;
	std::string day_of_week = ctx::TimerManager::dowToStr(0x01 << timeinfo.tm_wday);

	ctx::Json dataRead;
	dataRead.set(NULL, DEVICE_ST_DAY_OF_MONTH, day_of_month);
	dataRead.set(NULL, DEVICE_ST_DAY_OF_WEEK, day_of_week);
	dataRead.set(NULL, DEVICE_ST_TIME_OF_DAY, minute_of_day);

	_I("Time: %02d:%02d, Day of Week: %s, Day of Month: %d", timeinfo.tm_hour, timeinfo.tm_min, day_of_week.c_str(), day_of_month);

	ctx::context_manager::replyToRead(DEVICE_ST_SUBJ_TIME, NULL, ERR_NONE, dataRead);

	return ERR_NONE;
}
