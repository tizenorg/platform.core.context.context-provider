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

#include "location_logger.h"
#include <types_internal.h>
#include "../place_recognition_types.h"
#include <db_mgr.h>
#include <sstream>
#include <Json.h>
#include "user_places_params.h"
#include "debug_utils.h"

#ifdef TIZEN_ENGINEER_MODE
#define LOCATION_CREATE_TABLE_COLUMNS \
	LOCATION_COLUMN_LATITUDE " REAL NOT NULL, "\
	LOCATION_COLUMN_LONGITUDE " REAL NOT NULL, "\
	LOCATION_COLUMN_ACCURACY " REAL, "\
	LOCATION_COLUMN_TIMESTAMP " timestamp NOT NULL, "\
	LOCATION_COLUMN_TIMESTAMP_HUMAN " TEXT, "\
	LOCATION_COLUMN_METHOD " INTEGER "
#else /* TIZEN_ENGINEER_MODE */
#define LOCATION_CREATE_TABLE_COLUMNS \
	LOCATION_COLUMN_LATITUDE " REAL NOT NULL, "\
	LOCATION_COLUMN_LONGITUDE " REAL NOT NULL, "\
	LOCATION_COLUMN_ACCURACY " REAL, "\
	LOCATION_COLUMN_TIMESTAMP " timestamp NOT NULL "
#endif /* TIZEN_ENGINEER_MODE */

#define _LOCATION_ERROR_LOG(error) { \
	if (error != LOCATIONS_ERROR_NONE) { \
		_E("ERROR == %s", location_error_str(error)); \
	} else { \
		_D("SUCCESS"); \
	} \
}

void ctx::LocationLogger::location_service_state_changed_cb(location_service_state_e state, void *user_data)
{
	ctx::LocationLogger* location_logger_p = (ctx::LocationLogger *)user_data;
	location_logger_p->location_service_state = state;
	if (state == LOCATIONS_SERVICE_ENABLED) {
		_D("LOCATIONS_SERVICE_ENABLED");
		switch (location_logger_p->timer_purpose) {
		case LOCATION_LOGGER_WAITING_FOR_SERVICE_START:
			_D("Waiting for location service start FINISHED");
			location_logger_p->timer_stop();
			location_logger_p->location_request();
			break;
		case LOCATION_LOGGER_WAITING_FOR_ACTIVE_REQUEST:
		case LOCATION_LOGGER_WAITING_FOR_LOCATION_METHOD_SETTING_ON:
		case LOCATION_LOGGER_WAITING_FOR_ACTIVE_INTERVAL:
		case LOCATION_LOGGER_WAITING_FOR_PASSIVE_INTERVAL:
		default:
			// Do nothing
			break;
		}
	} else {
		_D("LOCATIONS_SERVICE_DISABLED");
//		location_logger_p->timer_stop();
	}
}

void ctx::LocationLogger::location_setting_changed_cb(location_method_e method, bool enable, void *user_data)
{
	ctx::LocationLogger* location_logger_p = (ctx::LocationLogger *)user_data;
	location_logger_p->location_method_state = enable;
	if (method == location_logger_p->location_method) {
		if (enable) {
			_D("Location method settings ON");
			switch (location_logger_p->timer_purpose) {
			case LOCATION_LOGGER_WAITING_FOR_LOCATION_METHOD_SETTING_ON:
				_D("Waiting for location method settings on FINISHED");
				if (location_logger_p->location_service_state == LOCATIONS_SERVICE_ENABLED) {
					location_logger_p->timer_stop();
					location_logger_p->location_request();
				} else {
					location_logger_p->manager_start();
				}
				break;
			case LOCATION_LOGGER_WAITING_FOR_SERVICE_START:
			case LOCATION_LOGGER_WAITING_FOR_ACTIVE_REQUEST:
			case LOCATION_LOGGER_WAITING_FOR_ACTIVE_INTERVAL:
			case LOCATION_LOGGER_WAITING_FOR_PASSIVE_INTERVAL:
			default:
				// Do nothing
				break;
			}
		} else {
			_D("Location method settings OFF");
//			location_logger_p->timer_stop();
		}
	}
}

void ctx::LocationLogger::position_updated_cb(double latitude, double longitude,
		double altitude, time_t timestamp, void *user_data)
{
	_D("");
	ctx::LocationLogger* location_logger_p = (ctx::LocationLogger *)user_data;
	double horizontal = location_logger_p->manager_get_horizontal_accuracy();
#ifdef TIZEN_ENGINEER_MODE
	ctx::location_event_s location(latitude, longitude, horizontal, timestamp, LOCATION_METHOD_REQUEST);
#else /* TIZEN_ENGINEER_MODE */
	ctx::location_event_s location(latitude, longitude, horizontal, timestamp);
#endif /* TIZEN_ENGINEER_MODE */
	location_logger_p->broadcast(location);
	location_logger_p->on_active_request_succeeded();
}

void ctx::LocationLogger::location_updated_cb(location_error_e error, double latitude, double longitude,
		double altitude, time_t timestamp, double speed, double direction, double climb, void *user_data)
{
	_D("");
	position_updated_cb(latitude, longitude, altitude, timestamp, user_data);
}

const char* ctx::LocationLogger::location_error_str(int error)
{
	switch (error) {
	case LOCATIONS_ERROR_NONE:
		return "LOCATIONS_ERROR_NONE";
	case LOCATIONS_ERROR_OUT_OF_MEMORY:
		return "LOCATIONS_ERROR_OUT_OF_MEMORY";
	case LOCATIONS_ERROR_INVALID_PARAMETER:
		return "LOCATIONS_ERROR_INVALID_PARAMETER";
	case LOCATIONS_ERROR_ACCESSIBILITY_NOT_ALLOWED:
		return "LOCATIONS_ERROR_ACCESSIBILITY_NOT_ALLOWED";
	case LOCATIONS_ERROR_NOT_SUPPORTED:
		return "LOCATIONS_ERROR_NOT_SUPPORTED";
	case LOCATIONS_ERROR_INCORRECT_METHOD:
		return "LOCATIONS_ERROR_INCORRECT_METHOD";
	case LOCATIONS_ERROR_NETWORK_FAILED:
		return "LOCATIONS_ERROR_NETWORK_FAILED";
	case LOCATIONS_ERROR_SERVICE_NOT_AVAILABLE:
		return "LOCATIONS_ERROR_SERVICE_NOT_AVAILABLE";
	case LOCATIONS_ERROR_GPS_SETTING_OFF:
		return "LOCATIONS_ERROR_GPS_SETTING_OFF";
	case LOCATIONS_ERROR_SECURITY_RESTRICTED:
		return "LOCATIONS_ERROR_SECURITY_RESTRICTED";
	default:
		return "unknown location error code";
	}
}


void ctx::LocationLogger::log(location_accessibility_state_e state)
{
	switch (state) {
	case LOCATIONS_ACCESS_STATE_NONE : // Access state is not determined
		_D("LOCATIONS_ACCESS_STATE_NONE ");
		break;
	case LOCATIONS_ACCESS_STATE_DENIED: // Access denied
		_D("LOCATIONS_ACCESS_STATE_DENIED");
		break;
	case LOCATIONS_ACCESS_STATE_ALLOWED: // Access authorized
		_D("LOCATIONS_ACCESS_STATE_ALLOWED");
		break;
	default:
		break;
	}
}

int ctx::LocationLogger::create_table()
{
	bool ret = db_manager::create_table(0, LOCATION_TABLE_NAME, LOCATION_CREATE_TABLE_COLUMNS, NULL, NULL);
	_D("%s -> Table Creation Request", ret ? "SUCCESS" : "FAIL");
	return 0;
}

int ctx::LocationLogger::db_insert_log(location_event_s location_event)
{
	Json data;
	data.set(NULL, LOCATION_COLUMN_LATITUDE, location_event.coordinates.latitude);
	data.set(NULL, LOCATION_COLUMN_LONGITUDE, location_event.coordinates.longitude);
	data.set(NULL, LOCATION_COLUMN_ACCURACY, location_event.coordinates.accuracy);
	data.set(NULL, LOCATION_COLUMN_TIMESTAMP, static_cast<int>(location_event.timestamp));
#ifdef TIZEN_ENGINEER_MODE
	std::string time_human = DebugUtils::human_readable_date_time(location_event.timestamp, "%F %T", 80);
	data.set(NULL, LOCATION_COLUMN_TIMESTAMP_HUMAN, time_human);
	data.set(NULL, LOCATION_COLUMN_METHOD, static_cast<int>(location_event.method));
#endif /* TIZEN_ENGINEER_MODE */

	bool ret = ctx::db_manager::insert(0, LOCATION_TABLE_NAME, data);
	_D("%s -> DB: location table insert result", ret ? "SUCCESS" : "FAIL");
	return ret;
}

ctx::LocationLogger::LocationLogger(ILocationListener *listener_, bool test_mode_)
	: listener(listener_)
	, test_mode(test_mode_)
	, active_request_attempts(0)
	, active_attempts(0)
	, all_attempts(0)
	, location_count(0)
	, active_request_succeeded(false)
	, active_location_succeeded(false)
	, timer_id(-1)
	, timer_timestamp(0)
	, timer_purpose(LOCATION_LOGGER_WAITING_FOR_PASSIVE_INTERVAL)
	, location_service_state(LOCATIONS_SERVICE_DISABLED)
	, location_method(LOCATION_LOGGER_METHOD)
	, location_method_state(false)
{
	_D("CONSTRUCTOR");

	manager_create();

	if (test_mode) {
		return;
	}
	if (LOCATION_LOGGER_DATABASE) {
		create_table();
	}

	manager_set_service_state_changed_cb();
	manager_set_setting_changed_cb();
	location_method_state = manager_is_enabled_method(location_method);
}

ctx::LocationLogger::~LocationLogger()
{
	_D("DESTRUCTOR");
	stop_logging();
	manager_unset_service_state_changed_cb();
	manager_unset_setting_changed_cb();
	manager_destroy();
}

void ctx::LocationLogger::manager_create()
{
	int ret = location_manager_create(location_method, &manager);
	_LOCATION_ERROR_LOG(ret);
}

void ctx::LocationLogger::manager_destroy()
{
	int ret = location_manager_destroy(manager);
	_LOCATION_ERROR_LOG(ret);
}

void ctx::LocationLogger::manager_set_service_state_changed_cb()
{
	int ret = location_manager_set_service_state_changed_cb(manager, location_service_state_changed_cb, this);
	_LOCATION_ERROR_LOG(ret);
}

void ctx::LocationLogger::manager_unset_service_state_changed_cb()
{
	int ret = location_manager_unset_service_state_changed_cb(manager);
	_LOCATION_ERROR_LOG(ret);
}

void ctx::LocationLogger::manager_start()
{
	int ret = location_manager_start(manager);
	_LOCATION_ERROR_LOG(ret);
	start_service_timer_start();
}

void ctx::LocationLogger::manager_stop()
{
	int ret = location_manager_stop(manager);
	_LOCATION_ERROR_LOG(ret);
}

double ctx::LocationLogger::manager_get_horizontal_accuracy()
{
	location_accuracy_level_e accuracy_level;
	double horizontal, vertical;
	int ret = location_manager_get_accuracy(manager, &accuracy_level, &horizontal, &vertical);
	_LOCATION_ERROR_LOG(ret);
	return horizontal;
}

location_accessibility_state_e ctx::LocationLogger::manager_get_accessibility_state()
{
	location_accessibility_state_e state;
	int ret = location_manager_get_accessibility_state(&state);
	_LOCATION_ERROR_LOG(ret);
	return state;
}

void ctx::LocationLogger::manager_set_setting_changed_cb()
{
	int ret = location_manager_set_setting_changed_cb(location_method, location_setting_changed_cb, this);
	_LOCATION_ERROR_LOG(ret);
}

void ctx::LocationLogger::manager_unset_setting_changed_cb()
{
	int ret = location_manager_unset_setting_changed_cb(location_method);
	_LOCATION_ERROR_LOG(ret);
}

bool ctx::LocationLogger::manager_request_single_location()
{
	int ret = location_manager_request_single_location(manager,
			LOCATION_LOGGER_ACTIVE_REQUEST_TIMEOUT_SECONDS, location_updated_cb, this);
	_D("%s (seconds=%d) ----- ATTEMPTS: REQ[%d/%d], ACT[%d/%d], ALL[%d/%d]; ----- LOCATIONS:[%d/%d]",
			ret == LOCATIONS_ERROR_NONE ? "SUCCESS" : "ERROR",
			LOCATION_LOGGER_ACTIVE_REQUEST_TIMEOUT_SECONDS,
			active_request_attempts,
			LOCATION_LOGGER_MAX_ACTIVE_REQUEST_ATTEMPTS,
			active_attempts,
			LOCATION_LOGGER_MAX_ACTIVE_LOCATION_ATTEMPTS,
			all_attempts,
			LOCATION_LOGGER_MAX_LOCATION_ATTEMPTS,
			location_count,
			LOCATION_LOGGER_MAX_LOCATION_COUNT);
	_LOCATION_ERROR_LOG(ret);
	active_request_attempts++;
	active_attempts++;
	all_attempts++;
	if (ret == LOCATIONS_ERROR_NONE) {
		active_request_timer_start();
		return true;
	} else {
		return false;
	}
}

bool ctx::LocationLogger::manager_get_location()
{
	double altitude, latitude, longitude, climb, direction, speed, horizontal, vertical;
	location_accuracy_level_e level;
	time_t timestamp;
	int ret = location_manager_get_location(manager, &altitude, &latitude, &longitude,
			&climb, &direction, &speed, &level, &horizontal, &vertical, &timestamp);
	_D("%s ----- ATTEMPTS: REQ[%d/%d], ACT[%d/%d], ALL[%d/%d]; ----- LOCATIONS:[%d/%d]",
			ret == LOCATIONS_ERROR_NONE ? "SUCCESS" : "ERROR",
			active_request_attempts,
			LOCATION_LOGGER_MAX_ACTIVE_REQUEST_ATTEMPTS,
			active_attempts,
			LOCATION_LOGGER_MAX_ACTIVE_LOCATION_ATTEMPTS,
			all_attempts,
			LOCATION_LOGGER_MAX_LOCATION_ATTEMPTS,
			location_count,
			LOCATION_LOGGER_MAX_LOCATION_COUNT);
	_LOCATION_ERROR_LOG(ret);
	active_attempts++;
	all_attempts++;
	if (ret == LOCATIONS_ERROR_NONE) {
#ifdef TIZEN_ENGINEER_MODE
		ctx::location_event_s location(latitude, longitude, horizontal, timestamp, LOCATION_METHOD_GET_LOCATION);
#else /* TIZEN_ENGINEER_MODE */
		ctx::location_event_s location(latitude, longitude, horizontal, timestamp);
#endif /* TIZEN_ENGINEER_MODE */
		broadcast(location);
		on_active_location_succeeded();
		return true;
	} else {
		return false;
	}
}

void ctx::LocationLogger::manager_get_last_location()
{
	double altitude, latitude, longitude, climb, direction, speed, horizontal, vertical;
	location_accuracy_level_e level;
	time_t timestamp;
	int ret = location_manager_get_last_location(manager, &altitude, &latitude, &longitude,
			&climb, &direction, &speed, &level, &horizontal, &vertical, &timestamp);
	_D("%s ----- ATTEMPTS: REQ[%d/%d], ACT[%d/%d], ALL[%d/%d]; ----- LOCATIONS:[%d/%d]",
			ret == LOCATIONS_ERROR_NONE ? "SUCCESS" : "ERROR",
			active_request_attempts,
			LOCATION_LOGGER_MAX_ACTIVE_REQUEST_ATTEMPTS,
			active_attempts,
			LOCATION_LOGGER_MAX_ACTIVE_LOCATION_ATTEMPTS,
			all_attempts,
			LOCATION_LOGGER_MAX_LOCATION_ATTEMPTS,
			location_count,
			LOCATION_LOGGER_MAX_LOCATION_COUNT);
	_LOCATION_ERROR_LOG(ret);
	all_attempts++;
	if (ret == LOCATIONS_ERROR_NONE) {
#ifdef TIZEN_ENGINEER_MODE
		ctx::location_event_s location(latitude, longitude, horizontal, timestamp, LOCATION_METHOD_GET_LAST_LOCATION);
#else /* TIZEN_ENGINEER_MODE */
		ctx::location_event_s location(latitude, longitude, horizontal, timestamp);
#endif /* TIZEN_ENGINEER_MODE */
		broadcast(location);
	}
}

bool ctx::LocationLogger::manager_is_enabled_method(location_method_e method)
{
	bool enable;
	int ret = location_manager_is_enabled_method(method, &enable);
	_LOCATION_ERROR_LOG(ret);
	return enable;
}

bool ctx::LocationLogger::check_general_limits()
{
	return (location_count < LOCATION_LOGGER_MAX_LOCATION_COUNT
			&& all_attempts < LOCATION_LOGGER_MAX_LOCATION_ATTEMPTS);
}

bool ctx::LocationLogger::check_active_limits()
{
	return (!active_location_succeeded
			&& active_attempts < LOCATION_LOGGER_MAX_ACTIVE_LOCATION_ATTEMPTS);
}

bool ctx::LocationLogger::check_active_request_limits()
{
	return (!active_request_succeeded
			&& active_request_attempts < LOCATION_LOGGER_MAX_ACTIVE_REQUEST_ATTEMPTS);
}

void ctx::LocationLogger::location_request()
{
	_D("");
	bool request_single_location_ret = false;
	bool get_location_ret = false;
	if (check_general_limits() && check_active_limits() && check_active_request_limits()) {
		request_single_location_ret = manager_request_single_location();
	}
	if (check_general_limits() && check_active_limits() && !request_single_location_ret) {
		get_location_ret = manager_get_location();
	}
	if (check_general_limits() && !request_single_location_ret && !get_location_ret
			&& active_attempts >= LOCATION_LOGGER_MAX_ACTIVE_LOCATION_ATTEMPTS) {
		manager_get_last_location();
	}
	if (!request_single_location_ret) {
		manager_stop();
		set_next_timer();
	}
}

void ctx::LocationLogger::set_next_timer()
{
	_D("ATTEMPTS: REQ[%d/%d], ACT[%d/%d], ALL[%d/%d]; ----- LOCATIONS:[%d/%d])",
			active_request_attempts,
			LOCATION_LOGGER_MAX_ACTIVE_REQUEST_ATTEMPTS,
			active_attempts,
			LOCATION_LOGGER_MAX_ACTIVE_LOCATION_ATTEMPTS,
			all_attempts,
			LOCATION_LOGGER_MAX_LOCATION_ATTEMPTS,
			location_count,
			LOCATION_LOGGER_MAX_LOCATION_COUNT);
	if (check_general_limits()) {
		if (check_active_limits()) {
			active_interval_timer_start();
		} else {
			passive_interval_timer_start();
		}
	}
}

void ctx::LocationLogger::on_active_request_succeeded()
{
	_D("");
	manager_stop();
	active_request_succeeded = true;
	on_active_location_succeeded();
}

void ctx::LocationLogger::on_active_location_succeeded()
{
	_D("");
	active_location_succeeded = true;
}

void ctx::LocationLogger::broadcast(ctx::location_event_s location_event)
{
	_D("");
	location_count++;
	if (listener) {
		listener->on_new_location(location_event);
	}
	if (LOCATION_LOGGER_DATABASE) {
		db_insert_log(location_event);
	}
}

bool ctx::LocationLogger::onTimerExpired(int id)
{
	time_t now = time(nullptr);
	double seconds = difftime(now, timer_timestamp);

	switch (timer_purpose) {
	case LOCATION_LOGGER_WAITING_FOR_ACTIVE_REQUEST:
		_D("Active request FAILED, timer_id = %d[%d], from start = %.1fs", id, timer_id, seconds);
		manager_stop();
		set_next_timer();
		return false;
	case LOCATION_LOGGER_WAITING_FOR_SERVICE_START:
		_D("Service start in timeout time FAILED, timer_id = %d[%d], from start = %.1fs", id, timer_id, seconds);
		// Waiting for service start FAILURE is also some kind of active request attempt
		active_request_attempts++;
		active_attempts++;
		all_attempts++;
		manager_stop();
		set_next_timer();
		return false;
	case LOCATION_LOGGER_WAITING_FOR_LOCATION_METHOD_SETTING_ON:
		_D("Still waiting for Location method settings on, timer_id = %d[%d], from start = %.1fs", id, timer_id, seconds);
		// Do nothing
		return false;
	case LOCATION_LOGGER_WAITING_FOR_ACTIVE_INTERVAL:
		_D("Active interval time expired, timer_id = %d[%d], from start = %.1fs", id, timer_id, seconds);
		break;
	case LOCATION_LOGGER_WAITING_FOR_PASSIVE_INTERVAL:
		_D("Passive interval time expired, timer_id = %d[%d], from start = %.1fs", id, timer_id, seconds);
		break;
	default:
		_D("Do nothing, timer_id = %d[%d], from start = %.1fs", id, timer_id, seconds);
		return false;
	}
	if (location_method_state) {
		manager_start();
	} else {
		timer_purpose = LOCATION_LOGGER_WAITING_FOR_LOCATION_METHOD_SETTING_ON;
		_D("LOCATION_LOGGER_WAITING_FOR_LOCATION_METHOD_SETTING_ON");
	}
	return false;
}

void ctx::LocationLogger::active_request_timer_start()
{
	int minutes = LOCATION_LOGGER_ACTIVE_REQUEST_TIMEOUT_SECONDS / 60;
	if (LOCATION_LOGGER_ACTIVE_REQUEST_TIMEOUT_SECONDS % 60) {
		minutes++;
	}
	timer_purpose = LOCATION_LOGGER_WAITING_FOR_ACTIVE_REQUEST;
	_D("LOCATION_LOGGER_WAITING_FOR_ACTIVE_REQUEST (minutes=%d)", minutes);
	timer_start(minutes);
}

void ctx::LocationLogger::start_service_timer_start()
{
	timer_purpose = LOCATION_LOGGER_WAITING_FOR_SERVICE_START;
	_D("LOCATION_LOGGER_WAITING_FOR_SERVICE_START");
	timer_start(LOCATION_LOGGER_SERVICE_START_TIMEOUT_MINUTES);
}

void ctx::LocationLogger::active_interval_timer_start()
{
	timer_purpose = LOCATION_LOGGER_WAITING_FOR_ACTIVE_INTERVAL;
	_D("LOCATION_LOGGER_WAITING_FOR_ACTIVE_INTERVAL");
	timer_start(LOCATION_LOGGER_ACTIVE_INTERVAL_MINUTES);
}

void ctx::LocationLogger::passive_interval_timer_start()
{
	timer_purpose = LOCATION_LOGGER_WAITING_FOR_PASSIVE_INTERVAL;
	_D("LOCATION_LOGGER_WAITING_FOR_PASSIVE_INTERVAL");
	timer_start(LOCATION_LOGGER_PASSIVE_INTERVAL_MINUTES);
}

void ctx::LocationLogger::timer_start(time_t minutes)
{
	timer_timestamp = time(nullptr);
	timer_id = __timerManager.setFor(minutes, this);
	_D("%s (minutes=%d) timer_id = %d", timer_id >= 0 ? "SUCCESS" : "ERROR", minutes, timer_id);
}

void ctx::LocationLogger::timer_stop()
{
	_D("");
	__timerManager.remove(timer_id);
}

void ctx::LocationLogger::start_logging()
{
	_D("");
	active_request_attempts = 0;
	active_attempts = 0;
	all_attempts = 0;
	location_count = 0;
	active_request_succeeded = false;;
	active_location_succeeded = false;
	manager_start();
}

void ctx::LocationLogger::stop_logging()
{
	_D("");
	timer_stop();
	manager_stop();
}

void ctx::LocationLogger::on_visit_start()
{
	_D("");
	if (!test_mode) {
		start_logging();
	}
}

void ctx::LocationLogger::on_visit_end()
{
	_D("");
	if (!test_mode) {
		stop_logging();
	}
}

#undef _LOCATION_ERROR_LOG
