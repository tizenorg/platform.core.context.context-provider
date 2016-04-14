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

#ifndef __CONTEXT_INACTIVE_DETECTOR_STORAGE_H__
#define __CONTEXT_INACTIVE_DETECTOR_STORAGE_H__

#include <vector>
#include "app_inactive_detector_types.h"
#include <Json.h>
#include <DatabaseManager.h>

namespace ctx {

	class inactive_detector_storage : public IDatabaseListener
	{
		private:
			//int type;  //TODO: enum
			void inject_params(std::string& str,
				const std::string& from,
				const std::string& to);

			void json_to_object(std::vector<Json>& records,
				std::vector<app_t> *apps_with_weights, bool result_mode);

			std::string query_get_apps(const char *subject,
				ctx::Json filter);

			std::string query_update_apps(std::vector<app_t> *apps_with_weights);

			std::string subquery_form_values(std::vector<app_t> *apps_with_weights);

			void onTableCreated(unsigned int query_id, int error) {}
			void onInserted(unsigned int query_id, int error, int64_t row_id) {}
			void onExecuted(unsigned int query_id, int error, std::vector<Json>& records);

			DatabaseManager __dbManager;

		public:
			inactive_detector_storage();
			~inactive_detector_storage();

			int read(const char *subject,
				ctx::Json filter);

			int update_ranks();
			int get_apps_info_w_weights(double timestamp_from);
	};	/* class inactive_detector_storage */

}	/* namespace ctx */

#endif /* __CONTEXT_INACTIVE_DETECTOR_STORAGE_H__ */
