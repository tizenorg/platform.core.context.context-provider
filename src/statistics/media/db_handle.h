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

#ifndef __CONTEXT_MEDIA_DB_HANDLE_H__
#define __CONTEXT_MEDIA_DB_HANDLE_H__

#include <string>
#include <json.h>
#include <db_handle_base.h>

namespace ctx {
	class media_db_handle : public stats_db_handle_base {
		public:
			media_db_handle();
			~media_db_handle();

			int read(const char* subject, ctx::json filter);

		private:
			std::string create_where_clause(int media_type, ctx::json filter);
			std::string create_sql_peak_time(int media_type, ctx::json filter);
			std::string create_sql_common_setting(int media_type, ctx::json filter);
			std::string create_sql_frequency(int media_type, ctx::json filter);
			void reply_trigger_item(int error, ctx::json &json_result);
	};
}

#endif /* __CONTEXT_MEDIA_DB_HANDLE_H__ */
