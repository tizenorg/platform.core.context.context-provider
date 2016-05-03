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

#include <TimerManager.h>
#include "Time.h"

using namespace ctx;

TimeProvider::TimeProvider() :
	BasicProvider(SUBJ_STATE_TIME)
{
}

TimeProvider::~TimeProvider()
{
}

bool TimeProvider::isSupported()
{
	return true;
}

int TimeProvider::subscribe()
{
	return ERR_NOT_SUPPORTED;
}

int TimeProvider::unsubscribe()
{
	return ERR_NOT_SUPPORTED;
}

int TimeProvider::read()
{
	time_t rawtime;
	struct tm timeInfo;

	time(&rawtime);
	tzset();
	localtime_r(&rawtime, &timeInfo);

	int dayOfMonth = timeInfo.tm_mday;
	int minuteOfDay = timeInfo.tm_hour * 60 + timeInfo.tm_min;
	std::string dayOfWeek = TimerManager::dowToStr(0x01 << timeInfo.tm_wday);

	Json dataRead;
	dataRead.set(NULL, KEY_DAY_OF_MONTH, dayOfMonth);
	dataRead.set(NULL, KEY_DAY_OF_WEEK, dayOfWeek);
	dataRead.set(NULL, KEY_TIME_OF_DAY, minuteOfDay);

	_I("Time: %02d:%02d, Day of Week: %s, Day of Month: %d", timeInfo.tm_hour, timeInfo.tm_min, dayOfWeek.c_str(), dayOfMonth);

	replyToRead(NULL, ERR_NONE, dataRead);

	return ERR_NONE;
}
