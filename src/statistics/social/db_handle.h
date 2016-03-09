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

#ifndef __CONTEXT_SOCIAL_DB_HANDLE_H__
#define __CONTEXT_SOCIAL_DB_HANDLE_H__

#include <string>
#include <Json.h>
#include "../shared/db_handle_base.h"

namespace ctx {
	class social_db_handle : public stats_db_handle_base {
		public:
			social_db_handle();
			~social_db_handle();

			int read(const char* subject, ctx::Json filter);

		private:
			std::string create_where_clause(ctx::Json filter);
			std::string create_sql_freq_address(ctx::Json filter);
			std::string create_sql_frequency(ctx::Json filter);
			void reply_trigger_item(int error, ctx::Json &json_result);
	};
}

#endif /* __CONTEXT_SOCIAL_DB_HANDLE_H__ */
