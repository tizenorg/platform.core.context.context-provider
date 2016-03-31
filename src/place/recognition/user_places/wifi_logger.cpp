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

#include "wifi_logger.h"
#include <types_internal.h>
#include "../place_recognition_types.h"
#include <db_mgr.h>
#include <sstream>
#include "debug_utils.h"

#define WIFI_CREATE_TABLE_COLUMNS \
	WIFI_COLUMN_TIMESTAMP " timestamp NOT NULL, "\
	WIFI_COLUMN_BSSID " TEXT NOT NULL"

#define _WIFI_ERROR_LOG(error) { \
	if (error != WIFI_ERROR_NONE) { \
		_E("ERROR == %s", wifi_error_str(error)); \
	} else { \
		_D("SUCCESS"); \
	} \
}

int ctx::WifiLogger::create_table()
{
	bool ret = db_manager::create_table(0, WIFI_TABLE_NAME, WIFI_CREATE_TABLE_COLUMNS, NULL, NULL);
	_D("Table Creation Request: %s", ret ? "SUCCESS" : "FAIL");
	return ret;
}

int ctx::WifiLogger::db_insert_logs()
{
	if (logs.size() > 0) {
		std::stringstream query;
		const char* separator = " ";
		query << "BEGIN TRANSACTION; \
				INSERT INTO " WIFI_TABLE_NAME " \
				( " WIFI_COLUMN_BSSID ", " WIFI_COLUMN_TIMESTAMP " ) \
				VALUES";
		for (mac_event_s mac_event : logs) {
			query << separator << "( '" << mac_event.mac << "', '" << mac_event.timestamp << "' )";
			separator = ", ";
		}
		logs.clear();
		query << "; \
				END TRANSACTION;";
		bool ret = ctx::db_manager::execute(0, query.str().c_str(), NULL);
		_D("DB insert request: %s", ret ? "SUCCESS" : "FAIL");
		return ret;
	}
	_D("logs vector empty -> nothing to insert");
	return 0;
}

ctx::WifiLogger::WifiLogger(IWifiListener * listener_, place_recog_mode_e energy_mode, bool test_mode_)
	: test_mode(test_mode_)
	, listener(listener_)
	, last_scan_time(time_t(0))
	, last_timer_callback_time(time_t(0))
	, timer_on(false)
	, interval_minutes(WIFI_LOGGER_INTERVAL_MINUTES_HIGH_ACCURACY)
	, during_visit(false)
	, connected_to_wifi_ap(false)
	, started(false)
	, running(false)
{
	_D("CONSTRUCTOR");
	if (test_mode) {
		return;
	}
	set_interval(energy_mode);

	if (WIFI_LOGGER_DATABASE) {
		create_table();
	}

	logs = std::vector<mac_event_s>();

	wifi_initialize_request();
	wifi_set_device_state_changed_cb_request();
	if (WIFI_LOGGER_LOW_POWER_MODE) {
		wifi_set_connection_state_changed_cb_request();
	}
	wifi_connection_state_e state = wifi_get_connection_state_request();
	connected_to_wifi_ap = (state == WIFI_CONNECTION_STATE_CONNECTED);
	_D("connected_to_wifi_ap = %d, during_visit = %d IN CONSTRUCTOR",
				static_cast<int>(connected_to_wifi_ap),
				static_cast<int>(during_visit));
}

ctx::WifiLogger::~WifiLogger()
{
	_D("DESTRUCTOR");
	stop_logging();
	wifi_deinitialize_request();
}

void ctx::WifiLogger::wifi_device_state_changed_cb(wifi_device_state_e state, void *user_data)
{
	ctx::WifiLogger* wifi_logger_p = (ctx::WifiLogger *)user_data;
	switch (state) {
	case WIFI_DEVICE_STATE_DEACTIVATED:
		_D("WIFI setting OFF");
		if (wifi_logger_p->started) {
			wifi_logger_p->_stop_logging();
		}
		break;
	case WIFI_DEVICE_STATE_ACTIVATED:
		_D("WIFI setting ON");
		if (wifi_logger_p->started) {
			wifi_logger_p->_start_logging();
		}
		break;
	default:
		break;
	}
}

void ctx::WifiLogger::wifi_connection_state_changed_cb(wifi_connection_state_e state, wifi_ap_h ap, void *user_data)
{
	ctx::WifiLogger* wifi_logger_p = (ctx::WifiLogger *)user_data;
	switch (state) {
	case WIFI_CONNECTION_STATE_CONNECTED:
		_D("connected to AP");
		wifi_logger_p->connected_to_wifi_ap = true;
		break;
	default:
		_D("disconnected from AP -> last_scans_pool.clear()");
		wifi_logger_p->connected_to_wifi_ap = false;
		wifi_logger_p->last_scans_pool.clear();
		break;
	}
	// TODO: Check if AP bssid (MAC Address) will be helpful somehow in LOW_POWER mode
}

bool ctx::WifiLogger::wifi_found_ap_cb(wifi_ap_h ap, void *user_data)
{
	ctx::WifiLogger* wifi_logger_p = (ctx::WifiLogger *)user_data;

	char *bssid = NULL;
	int ret = wifi_ap_get_bssid_request(ap, &bssid);
	if (ret != WIFI_ERROR_NONE) {
		return false;
	}

	Mac mac;
	try {
		mac = Mac(bssid);
	} catch (std::runtime_error &e) {
		_E("Cannot create mac_event. Exception: %s", e.what());
		return false;
	}

	mac_event_s log(wifi_logger_p->last_scan_time, mac);
	if (wifi_logger_p->listener) {
		wifi_logger_p->listener->on_wifi_scan(log);
		if (WIFI_LOGGER_LOW_POWER_MODE
				&& (wifi_logger_p->connected_to_wifi_ap || wifi_logger_p->during_visit) ) {
			// Add to last scans AP's set
			wifi_logger_p->last_scans_pool.insert(std::string(bssid));
		}
	}
	if (WIFI_LOGGER_DATABASE) {
		wifi_logger_p->logs.push_back(log);
	}

	return true;
}

const char* ctx::WifiLogger::wifi_error_str(int error)
{
	switch (error) {
	case WIFI_ERROR_INVALID_PARAMETER:
		return "WIFI_ERROR_INVALID_PARAMETER";
	case WIFI_ERROR_OUT_OF_MEMORY:
		return "WIFI_ERROR_OUT_OF_MEMORY";
	case WIFI_ERROR_INVALID_OPERATION:
		return "WIFI_ERROR_INVALID_OPERATION";
	case WIFI_ERROR_ADDRESS_FAMILY_NOT_SUPPORTED:
		return "WIFI_ERROR_ADDRESS_FAMILY_NOT_SUPPORTED";
	case WIFI_ERROR_OPERATION_FAILED:
		return "WIFI_ERROR_OPERATION_FAILED";
	case WIFI_ERROR_NO_CONNECTION:
		return "WIFI_ERROR_NO_CONNECTION";
	case WIFI_ERROR_NOW_IN_PROGRESS:
		return "WIFI_ERROR_NOW_IN_PROGRESS";
	case WIFI_ERROR_ALREADY_EXISTS:
		return "WIFI_ERROR_ALREADY_EXISTS";
	case WIFI_ERROR_OPERATION_ABORTED:
		return "WIFI_ERROR_OPERATION_ABORTED";
	case WIFI_ERROR_DHCP_FAILED:
		return "WIFI_ERROR_DHCP_FAILED";
	case WIFI_ERROR_INVALID_KEY:
		return "WIFI_ERROR_INVALID_KEY";
	case WIFI_ERROR_NO_REPLY:
		return "WIFI_ERROR_NO_REPLY";
	case WIFI_ERROR_SECURITY_RESTRICTED:
		return "WIFI_ERROR_SECURITY_RESTRICTED";
	case WIFI_ERROR_PERMISSION_DENIED:
		return "WIFI_ERROR_PERMISSION_DENIED";
	default:
		return "unknown wifi error code";
	}
}

void ctx::WifiLogger::wifi_scan_finished_cb(wifi_error_e error_code, void *user_data)
{
	ctx::WifiLogger* wifi_logger_p = (ctx::WifiLogger *)user_data;

	time_t now = time(nullptr);
#ifdef TIZEN_ENGINEER_MODE
	double seconds = 0;
	if (wifi_logger_p->last_scan_time > 0) {
		seconds = difftime(now, wifi_logger_p->last_scan_time);
	}
	std::string time_str = DebugUtils::human_readable_date_time(now, "%T", 9);
	_D("connected_to_wifi_ap = %d, during_visit = %d, last_scans_pool.size() = %d -> scan %s (from last : %.1fs)",
			static_cast<int>(wifi_logger_p->connected_to_wifi_ap),
			static_cast<int>(wifi_logger_p->during_visit),
			wifi_logger_p->last_scans_pool.size(),
			time_str.c_str(),
			seconds);
#endif /* TIZEN_ENGINEER_MODE */
	wifi_logger_p->last_scan_time = now;

	int ret = wifi_foreach_found_aps_request(user_data);
	if (ret != WIFI_ERROR_NONE) {
		return;
	}
	if (WIFI_LOGGER_DATABASE) {
		wifi_logger_p->db_insert_logs();
	}
}

bool ctx::WifiLogger::check_wifi_is_activated()
{
	bool wifi_activated = true;
	int ret = wifi_is_activated(&wifi_activated);
	_WIFI_ERROR_LOG(ret);
	_D("Wi-Fi is %s", wifi_activated ? "ON" : "OFF");
	return wifi_activated;
}

void ctx::WifiLogger::wifi_scan_request()
{
	int ret = wifi_scan(wifi_scan_finished_cb, this);
	_WIFI_ERROR_LOG(ret);
}

int ctx::WifiLogger::wifi_foreach_found_aps_request(void *user_data)
{
	int ret = wifi_foreach_found_aps(wifi_found_ap_cb, user_data);
	_WIFI_ERROR_LOG(ret);
	return ret;
}

wifi_connection_state_e ctx::WifiLogger::wifi_get_connection_state_request()
{
	wifi_connection_state_e connection_state;
	int ret = wifi_get_connection_state(&connection_state);
	_WIFI_ERROR_LOG(ret);
	return connection_state;
}

void ctx::WifiLogger::wifi_set_background_scan_cb_request()
{
	int ret = wifi_set_background_scan_cb(wifi_scan_finished_cb, this);
	_WIFI_ERROR_LOG(ret);
}

void ctx::WifiLogger::wifi_set_device_state_changed_cb_request()
{
	int ret = wifi_set_device_state_changed_cb(wifi_device_state_changed_cb, this);
	_WIFI_ERROR_LOG(ret);
}

void ctx::WifiLogger::wifi_set_connection_state_changed_cb_request()
{
	int ret = wifi_set_connection_state_changed_cb(wifi_connection_state_changed_cb, this);
	_WIFI_ERROR_LOG(ret);
}

int ctx::WifiLogger::wifi_ap_get_bssid_request(wifi_ap_h ap, char **bssid)
{
	int ret = wifi_ap_get_bssid(ap, bssid);
	_WIFI_ERROR_LOG(ret);
	return ret;
}

void ctx::WifiLogger::wifi_initialize_request()
{
	int ret = wifi_initialize();
	_WIFI_ERROR_LOG(ret);
}

void ctx::WifiLogger::wifi_deinitialize_request()
{
	int ret = wifi_deinitialize();
	_WIFI_ERROR_LOG(ret);
}

bool ctx::WifiLogger::check_timer_id(int id)
{
	_D("id == %d, timer_id == %d", id, timer_id);
	return id == timer_id;
}

/*
 * Accepted time from last callback is >= than minimum interval
 */
bool ctx::WifiLogger::check_timer_time(time_t now)
{
	double seconds = 0;
	if (last_timer_callback_time > 0) {
		seconds = difftime(now, last_timer_callback_time);
		if (seconds < WIFI_LOGGER_ACTIVE_SCANNING_MIN_INTERVAL) {
			_D("last == %d, now == %d, diff = %.1fs -> Incorrect timer callback", last_timer_callback_time, now, seconds);
			return false;
		} else {
			_D("last == %d, now == %d, diff = %.1fs -> Correct timer callback", last_timer_callback_time, now, seconds);
		}
	} else {
		_D("last == %d, now == %d -> First callback", last_timer_callback_time, now);
	}
	last_timer_callback_time = now;
	return true;
}

bool ctx::WifiLogger::onTimerExpired(int id)
{
	time_t now = time(nullptr);
	_D("");
	if (check_timer_id(id) == false) {
		// Incorrect callback call
		return false;
	}
	if (check_timer_time(now) == false) {
		// Prevention from double callback call bug
		return timer_on;
	}
	_D("connected_to_wifi_ap = %d, during_visit = %d, last_scans_pool.size() = %d",
			static_cast<int>(connected_to_wifi_ap),
			static_cast<int>(during_visit),
			last_scans_pool.size());
	if (WIFI_LOGGER_LOW_POWER_MODE
			&& during_visit
			&& connected_to_wifi_ap
			&& last_scans_pool.size() > 0) {
		_D("trying to send fake scan");
		if (listener) {
			_D("listener != false -> CORRECT");
			for (std::string bssid : last_scans_pool) {
				Mac mac(bssid);
				mac_event_s scan(now, mac);
				_D("send fake scan (%s)", bssid.c_str());
				listener->on_wifi_scan(scan);
			}
		}
	} else {
		wifi_scan_request();
	}
	return timer_on;
}

void ctx::WifiLogger::start_logging()
{
	_D("");
	started = true;
	_start_logging();
}

void ctx::WifiLogger::_start_logging()
{
	_D("");
	if (!check_wifi_is_activated() || running) {
		return;
	}
	running = true;

	if (WIFI_LOGGER_ACTIVE_SCANNING) {
		timer_start(interval_minutes);
		wifi_scan_request();
	}
	if (WIFI_LOGGER_PASSIVE_SCANNING) {
		wifi_set_background_scan_cb_request();
	}
}

void ctx::WifiLogger::stop_logging()
{
	_D("");
	started = false;
	_stop_logging();
}

void ctx::WifiLogger::_stop_logging()
{
	_D("");
	if (!running) {
		return;
	}
	if (WIFI_LOGGER_ACTIVE_SCANNING) {
		// Unset timer
		timer_on = false;
		// Remove timer
		__timerManager.remove(timer_id);
	}
	if (WIFI_LOGGER_PASSIVE_SCANNING) {
		wifi_unset_background_scan_cb();
	}
	running = false;
}

void ctx::WifiLogger::timer_start(time_t minutes)
{
	timer_on = true;
	timer_id = __timerManager.setFor(minutes, this);
	_D("%s (minutes=%d)", timer_id >= 0 ? "SUCCESS" : "ERROR", minutes);
}

void ctx::WifiLogger::on_visit_start()
{
	_D("");
	during_visit = true;
}

void ctx::WifiLogger::on_visit_end()
{
	_D("last_scans_pool.clear()");
	during_visit = false;
	last_scans_pool.clear();
}

void ctx::WifiLogger::set_interval(place_recog_mode_e energy_mode)
{
	switch (energy_mode) {
	case PLACE_RECOG_LOW_POWER_MODE:
		interval_minutes = WIFI_LOGGER_INTERVAL_MINUTES_LOW_POWER;
		break;
	case PLACE_RECOG_HIGH_ACCURACY_MODE:
		interval_minutes = WIFI_LOGGER_INTERVAL_MINUTES_HIGH_ACCURACY;
		break;
	default:
		_E("Incorrect energy mode");
	}
}

void ctx::WifiLogger::timer_restart()
{
	__timerManager.remove(timer_id);
	timer_start(interval_minutes);
}

void ctx::WifiLogger::set_mode(place_recog_mode_e energy_mode)
{
	_D("");
	set_interval(energy_mode);
	if (WIFI_LOGGER_ACTIVE_SCANNING && timer_on) {
		timer_restart();
	}
}
