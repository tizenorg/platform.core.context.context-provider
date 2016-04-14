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
#include <Json.h>
#include <DatabaseManager.h>

namespace ctx {
	class stats_db_handle_base : public IDatabaseListener {
		protected:
			bool is_trigger_item;
			std::string req_subject;
			ctx::Json req_filter;
			DatabaseManager __dbManager;

			stats_db_handle_base();
			~stats_db_handle_base();

			std::string create_where_clause(ctx::Json filter);
			std::string create_sql_peak_time(ctx::Json filter, const char* table_name, std::string where_clause);
			std::string create_sql_common_setting(ctx::Json filter, const char* table_name, std::string where_clause);

			bool execute_query(const char* subject, ctx::Json filter, const char* query);
			virtual void reply_trigger_item(int error, ctx::Json &json_result) = 0;
			static int generate_qid();

		private:
			void json_vector_to_array(std::vector<Json> &vec_json, ctx::Json &json_result);

			void onTableCreated(unsigned int query_id, int error);
			void onInserted(unsigned int query_id, int error, int64_t row_id);
			void onExecuted(unsigned int query_id, int error, std::vector<Json>& records);
	};
}

#endif /* __CONTEXT_STATS_DB_HANDLE_BASE_H__ */
