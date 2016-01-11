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

#ifndef _DEVICE_STATUS_HEADPHONE_H_
#define _DEVICE_STATUS_HEADPNOHE_H_

#include <glib.h>
#include <runtime_info.h>
#include <bluetooth.h>
#include "../provider_base.h"

namespace ctx {

	class device_status_headphone : public device_provider_base {

		GENERATE_PROVIDER_COMMON_DECL(device_status_headphone);

	public:
		int subscribe();
		int unsubscribe();
		int read();
		static bool is_supported();
		static void submit_trigger_item();

	private:
		bool connected;
		int audio_jack_state;
		bool bt_audio_state;
		bool bt_audio_callback_on;
		bool bt_event_handler_added;
		int bt_event_handling_count;

		device_status_headphone();
		~device_status_headphone();

		bool get_current_status();
		void set_bt_audio_callback();
		void unset_bt_audio_callback();
		void set_bt_audio_state(bool state);

		void generate_data_packet(json &data);
		bool handle_event();
		void handle_audio_jack_event();

		static gboolean handle_bt_event(gpointer data);
		static void on_audio_jack_state_changed(runtime_info_key_e runtime_key, void* user_data);
		static void on_bt_connection_changed(bool connected, bt_device_connection_info_s *conn_info, void *user_data);
		static bool on_bt_bond(bt_device_info_s *device_info, void* user_data);
	};
}

#endif // _DEVICE_STATUS_HEADPHONE_H_
