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

#ifndef __CONTEXT_DEVICE_STATUS_WIFI_H__
#define __CONTEXT_DEVICE_STATUS_WIFI_H__

#include <string>
#include <wifi.h>
#include "../provider_base.h"

namespace ctx {

	class device_status_wifi : public device_provider_base {

		GENERATE_PROVIDER_COMMON_DECL(device_status_wifi);

	public:
		int subscribe();
		int unsubscribe();
		int read();
		static bool is_supported();
		static void submit_trigger_item();

	private:
		enum _state_e {
			_UNKNOWN = -1,
			_DISABLED = 0,
			_UNCONNECTED,
			_CONNECTED,
		};

		int last_state;
		bool is_initialized;
		bool is_activated;
		wifi_connection_state_e conn_state;
		std::string bssid;

		device_status_wifi();
		~device_status_wifi();

		bool get_current_state();
		bool get_bssid();
		void clear_bssid();
		bool get_response_packet(Json &data);
		void aggregate_updated_data();
		bool start_monitor();
		void stop_monitor();
		static void device_state_changed_cb(wifi_device_state_e state, void *user_data);
		static void connection_state_changed_cb(wifi_connection_state_e state, wifi_ap_h ap, void *user_data);
	};
}

#endif
