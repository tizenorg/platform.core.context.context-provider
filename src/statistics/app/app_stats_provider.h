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

#ifndef __CONTEXT_APP_STATS_PROVIDER_H__
#define __CONTEXT_APP_STATS_PROVIDER_H__

#include <provider_iface.h>
#include "app_stats_types.h"

namespace ctx {

	class app_statistics_provider : public context_provider_iface {
	public:
		static context_provider_iface *create(void *data);
		static void destroy(void *data);
		static bool is_supported(const char *subject);
		static void submit_trigger_item();

		int subscribe(const char *subject, ctx::json option, ctx::json *request_result);
		int unsubscribe(const char *subject, ctx::json option);
		int read(const char *subject, ctx::json option, ctx::json *request_result);
		int write(const char *subject, ctx::json data, ctx::json *request_result);

	private:
		static app_statistics_provider *__instance;

		app_statistics_provider();
		~app_statistics_provider();
		bool init();

	};	/* class app_statistics_provider */

}	/* namespace ctx */

#endif /* __CONTEXT_APP_STATS_PROVIDER_H__ */
