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

#ifndef __CONTEXT_PLACE_STATUS_WIFI_LOGGER_H__
#define __CONTEXT_PLACE_STATUS_WIFI_LOGGER_H__

#include <timer_listener_iface.h>
#include <wifi.h>
#include <time.h>
#include "timer_mgr.h"
#include <vector>
#include <set>
#include "wifi_listener_iface.h"
#include "visit_listener_iface.h"
#include "user_places_params.h"

/* Database usage flag */
#define WIFI_LOGGER_DATABASE false

/* Active scanning usage flag */
#define WIFI_LOGGER_ACTIVE_SCANNING true

/* Passive scanning usage flag */
#define WIFI_LOGGER_PASSIVE_SCANNING true

/* Active scanning minimum interval in seconds */
#define WIFI_LOGGER_ACTIVE_SCANNING_MIN_INTERVAL 10

/*
 * Low power scanning usage flag
 * (When phone is connected to some WiFi Access Point
 * last scan data is returned instead of new scan triggering)
 */
#define WIFI_LOGGER_LOW_POWER_MODE false

namespace ctx {

	class WifiLogger : public timer_listener_iface, public IVisitListener {

	public:
		WifiLogger(IWifiListener * listener_ = nullptr,
				place_recog_mode_e mode = PLACE_RECOG_HIGH_ACCURACY_MODE,
				bool test_mode_ = false);
		~WifiLogger();

		void start_logging();
		void stop_logging();
		void set_mode(place_recog_mode_e energy_mode);

		/* INPUT */
		void on_visit_start();
		void on_visit_end();

	private:
		bool test_mode;
		IWifiListener * const listener;
		std::vector<mac_event_s> logs;
		std::set<std::string> last_scans_pool;
		time_t last_scan_time;
		time_t last_timer_callback_time;
		bool timer_on;
		int timer_id;
		int interval_minutes;
		bool during_visit;
		bool connected_to_wifi_ap;
		bool started;
		bool running;

		void _start_logging();
		void _stop_logging();
		void set_interval(place_recog_mode_e energy_mode);

		bool check_timer_id(int id);
		bool check_timer_time(time_t now);
		bool on_timer_expired(int timer_id, void* user_data);
		static int create_table();
		int db_insert_logs();
		static void wifi_device_state_changed_cb(wifi_device_state_e state, void *user_data);
		static void wifi_connection_state_changed_cb(wifi_connection_state_e state, wifi_ap_h ap, void *user_data);
		static bool wifi_found_ap_cb(wifi_ap_h ap, void *user_data);
		static void wifi_scan_finished_cb(wifi_error_e error_code, void *user_data);
		static bool check_wifi_is_activated();
		void wifi_scan_request();
		static int wifi_foreach_found_aps_request(void *user_data);
		static wifi_connection_state_e wifi_get_connection_state_request();
		void wifi_set_background_scan_cb_request();
		void wifi_set_device_state_changed_cb_request();
		void wifi_set_connection_state_changed_cb_request();
		static int wifi_ap_get_bssid_request(wifi_ap_h ap, char **bssid);
		void wifi_initialize_request();
		void wifi_deinitialize_request();
		static const char* wifi_error_str(int error);

		void timer_start(time_t minutes);
		void timer_restart();

	};	/* class WifiLogger */

}	/* namespace ctx */

#endif /* __CONTEXT_PLACE_STATUS_WIFI_LOGGER_H__ */
