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

#ifndef __CONTEXT_STATS_DB_HANDLE_BASE_H__
#define __CONTEXT_STATS_DB_HANDLE_BASE_H__

#include <string>
#include <json.h>
#include <db_listener_iface.h>

namespace ctx {
	class stats_db_handle_base : public db_listener_iface {
		protected:
			bool is_trigger_item;
			std::string req_subject;
			ctx::json req_filter;

			stats_db_handle_base();
			~stats_db_handle_base();

			std::string create_where_clause(ctx::json filter);
			std::string create_sql_peak_time(ctx::json filter, const char* table_name, std::string where_clause);
			std::string create_sql_common_setting(ctx::json filter, const char* table_name, std::string where_clause);

			bool execute_query(const char* subject, ctx::json filter, const char* query);
			virtual void reply_trigger_item(int error, ctx::json &json_result) = 0;
			static int generate_qid();

		private:
			void json_vector_to_array(std::vector<json> &vec_json, ctx::json &json_result);

			void on_creation_result_received(unsigned int query_id, int error);
			void on_insertion_result_received(unsigned int query_id, int error, int64_t row_id);
			void on_query_result_received(unsigned int query_id, int error, std::vector<json>& records);
	};
}

#endif /* __CONTEXT_STATS_DB_HANDLE_BASE_H__ */
