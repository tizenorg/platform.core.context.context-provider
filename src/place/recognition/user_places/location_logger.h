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

#ifndef __CONTEXT_PLACE_STATUS_LOCATION_LOGGER_H__
#define __CONTEXT_PLACE_STATUS_LOCATION_LOGGER_H__

#include <timer_listener_iface.h>
#include <locations.h>
#include "timer_mgr.h"
#include "visit_listener_iface.h"
#include "location_listener_iface.h"

/* Database usage flag */
#define LOCATION_LOGGER_DATABASE false // TODO: false in final solution

/* Locations measure method */
#define LOCATION_LOGGER_METHOD LOCATIONS_METHOD_HYBRID

/* TIMEOUTS: Location active measure request timeout (in seconds). */
#define LOCATION_LOGGER_ACTIVE_REQUEST_TIMEOUT_SECONDS 100

/* TIMEOUTS: Location service start timeout (in minutes). */
#define LOCATION_LOGGER_SERVICE_START_TIMEOUT_MINUTES 2

/* FREQUENCIES/INTERVALS: "Active" measure attempts frequency (in minutes) */
#define LOCATION_LOGGER_ACTIVE_INTERVAL_MINUTES 5

/* FREQUENCIES/INTERVALS: "Passive" measure attempts frequency (in minutes) */
#define LOCATION_LOGGER_PASSIVE_INTERVAL_MINUTES 30

/* ATTEMTS LIMITS: "Active" request attempts limit (must be <= than active location attempts) */
#define LOCATION_LOGGER_MAX_ACTIVE_REQUEST_ATTEMPTS 0

/* ATTEMTS LIMITS: "Active" measures attempts limit (must be <= than all attempts limit) */
#define LOCATION_LOGGER_MAX_ACTIVE_LOCATION_ATTEMPTS 2

/* ATTEMTS LIMITS: All attempts ("active" + "passive") limit */
#define LOCATION_LOGGER_MAX_LOCATION_ATTEMPTS 3

/* LOCATION LIMIT: Location count limit per visit */
#define LOCATION_LOGGER_MAX_LOCATION_COUNT 3

namespace ctx {

	typedef enum {
		LOCATION_LOGGER_WAITING_FOR_ACTIVE_REQUEST = 0,
		LOCATION_LOGGER_WAITING_FOR_SERVICE_START = 1,
		LOCATION_LOGGER_WAITING_FOR_LOCATION_METHOD_SETTING_ON = 2,
		LOCATION_LOGGER_WAITING_FOR_ACTIVE_INTERVAL = 3,
		LOCATION_LOGGER_WAITING_FOR_PASSIVE_INTERVAL = 4
	} timer_purpose_e;

	class LocationLogger : public timer_listener_iface, public IVisitListener {

	public:
		LocationLogger(ILocationListener *listener_ = nullptr,
				bool test_mode_ = false);
		~LocationLogger();

		/* INPUT */
		void on_visit_start();
		void on_visit_end();

	private:
		/* OUTPUT */
		ILocationListener * const listener;
		void broadcast(location_event_s location_event);

		/* INTERNAL */
		bool test_mode;
		void start_logging();
		void stop_logging();
		void location_request();
		void on_active_request_succeeded();
		void on_active_location_succeeded();

		/* INTERNAL : COUNTERS (LIMITS) */
		int active_request_attempts;
		int active_attempts;
		int all_attempts;
		int location_count;
		bool check_general_limits();
		bool check_active_limits();
		bool check_active_request_limits();

		/* INTERNAL : FLAGS */
		bool active_request_succeeded;
		bool active_location_succeeded;

		/* TIMER */
		int timer_id;
		time_t timer_timestamp;
		timer_purpose_e timer_purpose;
		void set_next_timer();
		void active_request_timer_start();
		void start_service_timer_start();
		void active_interval_timer_start();
		void passive_interval_timer_start();
		void timer_start(time_t minutes);
		void timer_stop();
		bool on_timer_expired(int timer_id, void* user_data);

		/* DATABASE */
		static int create_table();
		int db_insert_log(location_event_s location_event);

		/* DEBUG */
		static const char* location_error_str(int error);
		static void log(location_accessibility_state_e state);

		/* LOCATION MANAGER */
		location_manager_h manager;
		void manager_create();
		void manager_destroy();
		void manager_start();
		void manager_stop();
		location_accessibility_state_e manager_get_accessibility_state();

		/* LOCATION MANAGER : LOCATION SERVICE STATE */
		location_service_state_e location_service_state;
		static void location_service_state_changed_cb(location_service_state_e state, void *user_data);
		void manager_set_service_state_changed_cb();
		void manager_unset_service_state_changed_cb();

		/* LOCATION MANAGER : LOCATION METHOD SETTINGS */
		location_method_e location_method;
		bool location_method_state;
		bool manager_is_enabled_method(location_method_e method);
		static void location_setting_changed_cb(location_method_e method, bool enable, void *user_data);
		void manager_set_setting_changed_cb();
		void manager_unset_setting_changed_cb();

		/* LOCATION MANAGER : LOCATION */

		/* LOCATION MANAGER : LOCATION : SYNCHRONOUS */
		bool manager_get_location();
		void manager_get_last_location();
		double manager_get_horizontal_accuracy();

		/* LOCATION MANAGER : LOCATION : ASYNCHRONOUS */
		static void position_updated_cb(double latitude, double longitude,
				double altitude, time_t timestamp, void *user_data);
		static void location_updated_cb(location_error_e error, double latitude,
				double longitude, double altitude, time_t timestamp, double speed,
				double direction, double climb, void *user_data);
		bool manager_request_single_location();

	};	/* class LocationLogger */

}	/* namespace ctx */

#endif /* __CONTEXT_PLACE_STATUS_LOCATION_LOGGER_H__ */
