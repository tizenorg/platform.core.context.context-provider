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

#include <string>
#include <sstream>
#include <dbus_listener_iface.h>

namespace ctx {

	class app_use_monitor : public dbus_listener_iface {
	private:
		int64_t signal_id;
		int last_cleanup_time;
		std::string last_app_id;

		bool start_logging(void);
		void stop_logging(void);

		bool is_skippable(std::string app_id);
		void create_record(std::string app_id);
		void finish_record(std::string app_id);
		void remove_expired();

		void on_active_window_changed(std::string app_id);
		void on_signal_received(const char* sender, const char* path, const char* iface, const char* name, GVariant* param);

	public:
		app_use_monitor();
		~app_use_monitor();
	};	/* class app_use_monitor */

}	/* namespace ctx */

#endif /* __CONTEXT_APP_USE_MONITOR_H__ */
