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

#ifndef __CONTEXT_PLACE_MYPLACE_HANDLE_H__
#define __CONTEXT_PLACE_MYPLACE_HANDLE_H__

#include <set>
#include <string>
#include <map>
#include <geofence_manager.h>

namespace ctx {

	class myplace_handle {

		typedef std::map<int, geofence_state_e> geo_state_map_t;
		typedef std::set<std::string> string_set_t;

		public:
			myplace_handle();
			~myplace_handle();

			bool start_monitor(int place_id);
			int get_place_id();

		private:
			int _place_id;
			geofence_state_e prev_state;
			geofence_manager_h geo_handle;
			geo_state_map_t geo_state_map;

			void emit_state_change();
			void stop_monitor();
			bool start_fence(int fence_id);
			void remove_fence(int fence_id);
			void update_fence(int fence_id, geofence_manage_e manage);
			void update_state(int fence_id, geofence_state_e state);

			static bool fence_list_cb(int geofence_id, geofence_h fence, int fence_index, int fence_cnt, void* user_data);
			static void fence_event_cb(int place_id, int geofence_id, geofence_manager_error_e error, geofence_manage_e manage, void* user_data);
			static void fence_state_cb(int geofence_id, geofence_state_e state, void* user_data);
	};

}	/* namespace ctx */

#endif /* __CONTEXT_PLACE_MYPLACE_HANDLE_H__ */
