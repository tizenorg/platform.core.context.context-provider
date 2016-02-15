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

#include <context_mgr.h>
#include <timer_mgr.h>
#include <timer_util.h>
#include "system_types.h"
#include "alarm.h"

GENERATE_PROVIDER_COMMON_IMPL(device_status_alarm);

ctx::device_status_alarm::device_status_alarm()
{
}

ctx::device_status_alarm::~device_status_alarm()
{
	clear();

	for (ctx::device_status_alarm::option_t::iterator it = option_set.begin(); it != option_set.end(); ++it) {
		delete *it;
	}
	option_set.clear();
}

bool ctx::device_status_alarm::is_supported()
{
	return true;
}

void ctx::device_status_alarm::submit_trigger_item()
{
	context_manager::register_trigger_item(DEVICE_ST_SUBJ_ALARM, OPS_SUBSCRIBE,
			"{"
				"\"TimeOfDay\":{\"type\":\"integer\",\"min\":0,\"max\":1439},"
				"\"DayOfWeek\":{\"type\":\"string\",\"values\":[\"Mon\",\"Tue\",\"Wed\",\"Thu\",\"Fri\",\"Sat\",\"Sun\",\"Weekday\",\"Weekend\"]}"
			"}",
			NULL);
}

int ctx::device_status_alarm::subscribe(const char *subject, ctx::json option, ctx::json *request_result)
{
	int ret = subscribe(option);
	destroy_if_unused();
	return ret;
}

int ctx::device_status_alarm::unsubscribe(const char *subject, ctx::json option)
{
	int ret = unsubscribe(option);
	destroy_if_unused();
	return ret;
}

int ctx::device_status_alarm::read(const char *subject, ctx::json option, ctx::json *request_result)
{
	destroy_if_unused();
	return ERR_NOT_SUPPORTED;
}

int ctx::device_status_alarm::write(const char *subject, ctx::json data, ctx::json *request_result)
{
	destroy_if_unused();
	return ERR_NOT_SUPPORTED;
}

int ctx::device_status_alarm::subscribe(ctx::json option)
{
	int dow = get_arranged_day_of_week(option);

	int time;
	for (int i = 0; option.get_array_elem(NULL, DEVICE_ST_TIME_OF_DAY, i, &time); i++) {
		add(time, dow);
	}

	ctx::json* elem = new(std::nothrow) ctx::json(option);
	if (elem) {
		option_set.insert(elem);
	} else {
		unsubscribe(option);
		_E("Memory allocation failed");
		return ERR_OUT_OF_MEMORY;
	}

	return ERR_NONE;
}

int ctx::device_status_alarm::unsubscribe(ctx::json option)
{
	int dow = get_arranged_day_of_week(option);

	int time;
	for (int i = 0; option.get_array_elem(NULL, DEVICE_ST_TIME_OF_DAY, i, &time); i++) {
		remove(time, dow);
	}

	option_t::iterator target = find_option(option);
	if (target != option_set.end()) {
		delete (*target);
		option_set.erase(target);
	}

	return ERR_NONE;
}

int ctx::device_status_alarm::get_arranged_day_of_week(ctx::json& option)
{
	int dow = 0;

	std::string tmp_d;
	for (int i = 0; option.get_array_elem(NULL, DEVICE_ST_DAY_OF_WEEK, i, &tmp_d); i++) {
		dow |= ctx::timer_util::convert_day_of_week_string_to_int(tmp_d);
	}
	_D("Requested day of week (%#x)", dow);

	return dow;
}

ctx::device_status_alarm::ref_count_array_s::ref_count_array_s()
{
	memset(count, 0, sizeof(int) * MAX_DAY);
}

int ctx::device_status_alarm::merge_day_of_week(int* ref_cnt)
{
	int day_of_week = 0;

	for (int d = 0; d < MAX_DAY; ++d) {
		if (ref_cnt[d] > 0) {
			day_of_week |= (0x01 << d);
		}
	}

	return day_of_week;
}

bool ctx::device_status_alarm::add(int minute, int day_of_week)
{
	IF_FAIL_RETURN_TAG(minute >=0 && minute < 1440 &&
			day_of_week > 0 && day_of_week <= timer_types::EVERYDAY,
			false, _E, "Invalid parameter");

	ref_count_array_s &ref = ref_count_map[minute];

	for (int d = 0; d < MAX_DAY; ++d) {
		if ((day_of_week & (0x01 << d)) != 0) {
			ref.count[d] += 1;
		}
	}

	return reset_timer(minute);
}

bool ctx::device_status_alarm::remove(int minute, int day_of_week)
{
	IF_FAIL_RETURN_TAG(minute >=0 && minute < 1440 &&
			day_of_week > 0 && day_of_week <= timer_types::EVERYDAY,
			false, _E, "Invalid parameter");

	ref_count_array_s &ref = ref_count_map[minute];

	for (int d = 0; d < MAX_DAY; ++d) {
		if ((day_of_week & (0x01 << d)) != 0 && ref.count[d] > 0) {
			ref.count[d] -= 1;
		}
	}

	return reset_timer(minute);
}

bool ctx::device_status_alarm::reset_timer(int minute)
{
	int day_of_week = merge_day_of_week(ref_count_map[minute].count);
	timer_state_s &timer = timer_state_map[minute];

	if (day_of_week == timer.day_of_week) {
		/* Necessary timers are already running... */
		return true;
	}

	if (day_of_week == 0 && timer.timer_id > 0) {
		/* Turn off the timer at hour, if it is not necessray anymore. */
		timer_manager::remove(timer.timer_id);
		timer_state_map.erase(minute);
		ref_count_map.erase(minute);
		return true;
	}

	if (timer.timer_id > 0) {
		/* Turn off the current timer, to set a new one. */
		timer_manager::remove(timer.timer_id);
		timer.timer_id = -1;
		timer.day_of_week = 0;
	}

	/* Create a new timer, w.r.t. the new day_of_week value. */
	int h = minute / 60;
	int m = minute - h * 60;
	int tid = timer_manager::set_at(h, m, day_of_week, this);
	IF_FAIL_RETURN_TAG(tid > 0, false, _E, "Timer setting failed");

	timer.timer_id = tid;
	timer.day_of_week = day_of_week;

	return true;
}

void ctx::device_status_alarm::clear()
{
	for (timer_state_map_t::iterator it = timer_state_map.begin(); it != timer_state_map.end(); ++it) {
		if (it->second.timer_id > 0) {
			timer_manager::remove(it->second.timer_id);
		}
	}

	timer_state_map.clear();
	ref_count_map.clear();
}

bool ctx::device_status_alarm::on_timer_expired(int timer_id, void* user_data)
{
	time_t rawtime;
	struct tm timeinfo;

	time(&rawtime);
	tzset();
	localtime_r(&rawtime, &timeinfo);

	int hour = timeinfo.tm_hour;
	int min = timeinfo.tm_min;
	int day_of_week = (0x01 << timeinfo.tm_wday);

	on_timer_expired(hour, min, day_of_week);

	return true;
}

void ctx::device_status_alarm::on_timer_expired(int hour, int min, int day_of_week)
{
	_I("Time: %02d:%02d, Day of Week: %#x", hour, min, day_of_week);

	ctx::json data_read;
	int result_time = hour * 60 + min;
	std::string result_day = ctx::timer_util::convert_day_of_week_int_to_string(day_of_week);
	data_read.set(NULL, DEVICE_ST_TIME_OF_DAY, result_time);
	data_read.set(NULL, DEVICE_ST_DAY_OF_WEEK, result_day);

	for (option_t::iterator it = option_set.begin(); it != option_set.end(); ++it) {
		ctx::json option = (**it);
		if (is_matched(option, result_time, result_day)) {
			context_manager::publish(DEVICE_ST_SUBJ_ALARM, option, ERR_NONE, data_read);
		}
	}
}

bool ctx::device_status_alarm::is_matched(ctx::json& option, int time, std::string day)
{
	bool ret = false;
	int opt_time;
	for (int i = 0; option.get_array_elem(NULL, DEVICE_ST_TIME_OF_DAY, i, &opt_time); i++){
		if (time == opt_time) {
			ret = true;
			break;
		}
	}
	IF_FAIL_RETURN(ret, false);

	std::string opt_day;
	for (int i = 0; option.get_array_elem(NULL, DEVICE_ST_DAY_OF_WEEK, i, &opt_day); i++){
		if (day == opt_day) {
			return true;
		}
	}

	return false;
}

ctx::device_status_alarm::option_t::iterator ctx::device_status_alarm::find_option(ctx::json& option)
{
	for (ctx::device_status_alarm::option_t::iterator it = option_set.begin(); it != option_set.end(); ++it) {
		if (option == (**it))
			return it;
	}
	return option_set.end();
}

void ctx::device_status_alarm::destroy_if_unused()
{
	IF_FAIL_VOID(option_set.empty());
	destroy(NULL);
}
