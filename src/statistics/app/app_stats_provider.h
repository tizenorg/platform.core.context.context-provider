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

#include <ContextProviderBase.h>
#include "app_stats_types.h"

namespace ctx {

	class app_statistics_provider : public ContextProviderBase {
	public:
		static ContextProviderBase *create(void *data);
		static void destroy(void *data);
		static bool is_supported(const char *subject);
		static void submit_trigger_item();

		int subscribe(const char *subject, ctx::Json option, ctx::Json *requestResult);
		int unsubscribe(const char *subject, ctx::Json option);
		int read(const char *subject, ctx::Json option, ctx::Json *requestResult);
		int write(const char *subject, ctx::Json data, ctx::Json *requestResult);

	private:
		static app_statistics_provider *__instance;

		app_statistics_provider();
		~app_statistics_provider();
		bool init();

	};	/* class app_statistics_provider */

}	/* namespace ctx */

#endif /* __CONTEXT_APP_STATS_PROVIDER_H__ */
