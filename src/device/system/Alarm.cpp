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

#include "SystemTypes.h"
#include "Alarm.h"

using namespace ctx;

DeviceStatusAlarm::DeviceStatusAlarm()
	: ContextProvider(DEVICE_ST_SUBJ_ALARM)
{
}

DeviceStatusAlarm::~DeviceStatusAlarm()
{
	__clear();

	for (auto it = __optionSet.begin(); it != __optionSet.end(); ++it) {
		delete *it;
	}
	__optionSet.clear();
}

bool DeviceStatusAlarm::isSupported()
{
	return true;
}

void DeviceStatusAlarm::submitTriggerItem()
{
	registerTriggerItem(OPS_SUBSCRIBE,
			"{"
				"\"TimeOfDay\":{\"type\":\"integer\",\"min\":0,\"max\":1439},"
				"\"DayOfWeek\":{\"type\":\"string\",\"values\":[\"Mon\",\"Tue\",\"Wed\",\"Thu\",\"Fri\",\"Sat\",\"Sun\",\"Weekday\",\"Weekend\"]}"
			"}",
			NULL);
}

int DeviceStatusAlarm::subscribe(Json option, Json *requestResult)
{
	int dow = __getArrangedDayOfWeek(option);

	int time;
	for (int i = 0; option.getAt(NULL, DEVICE_ST_TIME_OF_DAY, i, &time); i++) {
		__add(time, dow);
	}

	Json* elem = new(std::nothrow) Json(option);
	if (elem) {
		__optionSet.insert(elem);
	} else {
		unsubscribe(option);
		_E("Memory allocation failed");
		return ERR_OUT_OF_MEMORY;
	}

	return ERR_NONE;
}

int DeviceStatusAlarm::unsubscribe(Json option)
{
	int dow = __getArrangedDayOfWeek(option);

	int time;
	for (int i = 0; option.getAt(NULL, DEVICE_ST_TIME_OF_DAY, i, &time); i++) {
		__remove(time, dow);
	}

	OptionSet::iterator target = __findOption(option);
	if (target != __optionSet.end()) {
		delete (*target);
		__optionSet.erase(target);
	}

	return ERR_NONE;
}

int DeviceStatusAlarm::__getArrangedDayOfWeek(Json& option)
{
	int dow = 0;

	std::string tempDay;
	for (int i = 0; option.getAt(NULL, DEVICE_ST_DAY_OF_WEEK, i, &tempDay); i++) {
		dow |= TimerManager::dowToInt(tempDay);
	}
	_D("Requested day of week (%#x)", dow);

	return dow;
}

DeviceStatusAlarm::RefCountArray::RefCountArray()
{
	memset(count, 0, sizeof(int) * DAYS_PER_WEEK);
}

int DeviceStatusAlarm::__mergeDayOfWeek(int* refCnt)
{
	int dayOfWeek = 0;

	for (int d = 0; d < DAYS_PER_WEEK; ++d) {
		if (refCnt[d] > 0) {
			dayOfWeek |= (0x01 << d);
		}
	}

	return dayOfWeek;
}

bool DeviceStatusAlarm::__add(int minute, int dayOfWeek)
{
	IF_FAIL_RETURN_TAG(minute >=0 && minute < 1440 &&
			dayOfWeek > 0 && dayOfWeek <= static_cast<int>(DayOfWeek::EVERYDAY),
			false, _E, "Invalid parameter");

	RefCountArray &ref = __refCountMap[minute];

	for (int d = 0; d < DAYS_PER_WEEK; ++d) {
		if ((dayOfWeek & (0x01 << d)) != 0) {
			ref.count[d] += 1;
		}
	}

	return __resetTimer(minute);
}

bool DeviceStatusAlarm::__remove(int minute, int dayOfWeek)
{
	IF_FAIL_RETURN_TAG(minute >= 0 && minute < 1440 &&
			dayOfWeek > 0 && dayOfWeek <= static_cast<int>(DayOfWeek::EVERYDAY),
			false, _E, "Invalid parameter");

	RefCountArray &ref = __refCountMap[minute];

	for (int d = 0; d < DAYS_PER_WEEK; ++d) {
		if ((dayOfWeek & (0x01 << d)) != 0 && ref.count[d] > 0) {
			ref.count[d] -= 1;
		}
	}

	return __resetTimer(minute);
}

bool DeviceStatusAlarm::__resetTimer(int minute)
{
	int dayOfWeek = __mergeDayOfWeek(__refCountMap[minute].count);
	TimerState &timer = __timerStateMap[minute];

	if (dayOfWeek == timer.dayOfWeek) {
		/* Necessary timers are already running... */
		return true;
	}

	if (dayOfWeek == 0 && timer.timerId > 0) {
		/* Turn off the timer at hour, if it is not necessray anymore. */
		__timerManager.remove(timer.timerId);
		__timerStateMap.erase(minute);
		__refCountMap.erase(minute);
		return true;
	}

	if (timer.timerId > 0) {
		/* Turn off the current timer, to set a new one. */
		__timerManager.remove(timer.timerId);
		timer.timerId = -1;
		timer.dayOfWeek = 0;
	}

	/* Create a new timer, w.r.t. the new dayOfWeek value. */
	int h = minute / 60;
	int m = minute - h * 60;
	int tid = __timerManager.setAt(h, m, static_cast<DayOfWeek>(dayOfWeek), this);
	IF_FAIL_RETURN_TAG(tid > 0, false, _E, "Timer setting failed");

	timer.timerId = tid;
	timer.dayOfWeek = dayOfWeek;

	return true;
}

void DeviceStatusAlarm::__clear()
{
	for (auto it = __timerStateMap.begin(); it != __timerStateMap.end(); ++it) {
		if (it->second.timerId > 0) {
			__timerManager.remove(it->second.timerId);
		}
	}

	__timerStateMap.clear();
	__refCountMap.clear();
}

bool DeviceStatusAlarm::onTimerExpired(int timerId)
{
	time_t rawTime;
	struct tm timeInfo;

	time(&rawTime);
	tzset();
	localtime_r(&rawTime, &timeInfo);

	int hour = timeInfo.tm_hour;
	int min = timeInfo.tm_min;
	int dayOfWeek = (0x01 << timeInfo.tm_wday);

	__handleUpdate(hour, min, dayOfWeek);

	return true;
}

void DeviceStatusAlarm::__handleUpdate(int hour, int min, int dayOfWeek)
{
	_I("Time: %02d:%02d, Day of Week: %#x", hour, min, dayOfWeek);

	Json dataRead;
	int resultTime = hour * 60 + min;
	std::string resultDay = TimerManager::dowToStr(dayOfWeek);
	dataRead.set(NULL, DEVICE_ST_TIME_OF_DAY, resultTime);
	dataRead.set(NULL, DEVICE_ST_DAY_OF_WEEK, resultDay);

	for (auto it = __optionSet.begin(); it != __optionSet.end(); ++it) {
		Json option = (**it);
		if (__isMatched(option, resultTime, resultDay)) {
			publish(option, ERR_NONE, dataRead);
		}
	}
}

bool DeviceStatusAlarm::__isMatched(Json& option, int time, std::string day)
{
	bool ret = false;
	int optionTime;
	for (int i = 0; option.getAt(NULL, DEVICE_ST_TIME_OF_DAY, i, &optionTime); i++){
		if (time == optionTime) {
			ret = true;
			break;
		}
	}
	IF_FAIL_RETURN(ret, false);

	std::string optionDay;
	for (int i = 0; option.getAt(NULL, DEVICE_ST_DAY_OF_WEEK, i, &optionDay); i++){
		if (day == optionDay) {
			return true;
		}
	}

	return false;
}

DeviceStatusAlarm::OptionSet::iterator DeviceStatusAlarm::__findOption(Json& option)
{
	for (auto it = __optionSet.begin(); it != __optionSet.end(); ++it) {
		if (option == (**it))
			return it;
	}
	return __optionSet.end();
}
