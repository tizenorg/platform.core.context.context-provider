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

#ifndef __CONTEXT_MEDIA_CONTENT_MONITOR_H__
#define __CONTEXT_MEDIA_CONTENT_MONITOR_H__

#include <sstream>
#include <media_content.h>
#include <db_listener_iface.h>

namespace ctx {

	class media_content_monitor : public db_listener_iface {
	private:
		bool started;
		int last_cleanup_time;

		bool start_monitoring();
		void stop_monitoring();

		void append_cleanup_query(std::stringstream &query);
		void update_play_count(const char *uuid, int type, int count);
		void insert_log(int media_type);

		void on_creation_result_received(unsigned int query_id, int error) {}
		void on_insertion_result_received(unsigned int query_id, int error, int64_t row_id) {}
		void on_query_result_received(unsigned int query_id, int error, std::vector<Json>& records);

		static void on_media_content_db_updated(media_content_error_e error, int pid,
				media_content_db_update_item_type_e update_item,
				media_content_db_update_type_e update_type,
				media_content_type_e media_type,
				char *uuid, char *path, char *mime_type, void *user_data);

	public:
		media_content_monitor();
		~media_content_monitor();
	};

}	/* namespace ctx */

#endif
