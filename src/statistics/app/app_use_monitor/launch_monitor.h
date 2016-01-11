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

#ifndef __CONTEXT_APP_USE_MONITOR_H__
#define __CONTEXT_APP_USE_MONITOR_H__

#include <app_manager.h>
#include <db_listener_iface.h>

namespace ctx {

	class app_use_monitor : public db_listener_iface {
	private:
		bool start_logging(void);
		void stop_logging(void);

		void log_launch_event(const char* app_id);
		void log_terminate_event(const char* app_id);

		void on_creation_result_received(unsigned int query_id, int error) {}
		void on_insertion_result_received(unsigned int query_id, int error, int64_t row_id) {}
		void on_query_result_received(unsigned int query_id, int error, std::vector<json>& records) {}

		static void app_context_event_cb(app_context_h app_context, app_context_event_e event, void *user_data);

	public:
		app_use_monitor();
		~app_use_monitor();
	};	/* class app_use_monitor */

}	/* namespace ctx */

#endif /* __CONTEXT_APP_USE_MONITOR_H__ */
