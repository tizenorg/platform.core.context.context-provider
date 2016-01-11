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

#ifndef __CONTEXT_APP_INSTALL_MONITOR_H__
#define __CONTEXT_APP_INSTALL_MONITOR_H__

#include <package_manager.h>
#include <db_listener_iface.h>

namespace ctx {

	class app_install_monitor : public db_listener_iface {
	private:
		package_manager_h pkgmgr_h;

		bool start_monitoring();
		void stop_monitoring();

		void on_creation_result_received(unsigned int query_id, int error) {}
		void on_insertion_result_received(unsigned int query_id, int error, int64_t row_id) {}
		void on_query_result_received(unsigned int query_id, int error, std::vector<json>& records) {}

		static std::string create_deletion_query(const char* table_name, const char* app_id);
		static void package_event_cb(const char *type, const char *package, package_manager_event_type_e event_type, package_manager_event_state_e event_state, int progress, package_manager_error_e error, void *user_data);
		static bool app_info_cb(package_info_app_component_type_e comp_type, const char *app_id, void *user_data);

	public:
		app_install_monitor();
		~app_install_monitor();
	};

}	/* namespace ctx */

#endif
