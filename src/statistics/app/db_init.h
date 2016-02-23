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

#ifndef __CONTEXT_APP_DB_INITIALIZER_H__
#define __CONTEXT_APP_DB_INITIALIZER_H__

#include <package_manager.h>
#include <db_listener_iface.h>

namespace ctx {

	class app_db_initializer : public db_listener_iface {
		private:
			void create_table();
			void check_app_list();
			void duplicate_app_list();

			void on_creation_result_received(unsigned int query_id, int error);
			void on_insertion_result_received(unsigned int query_id, int error, int64_t row_id);
			void on_query_result_received(unsigned int query_id, int error, std::vector<Json>& records);

			static bool package_info_cb(package_info_h package_info, void *user_data);
			static bool app_info_cb(package_info_app_component_type_e comp_type, const char *app_id, void *user_data);

		public:
			app_db_initializer();
			~app_db_initializer();
	};

}	/* namespace ctx */

#endif
