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

#ifndef __CONTEXT_APP_INACTIVE_DETECTOR_H__
#define __CONTEXT_APP_INACTIVE_DETECTOR_H__

#include <ContextProvider.h>
#include "AppInactiveDetectorTypes.h"
#include "InactiveDetector.h"

namespace ctx {

	class app_inactive_detector_provider : public ContextProvider {

		public:
			static ContextProvider *create(void *data);
			static void destroy(void *data);
			static bool is_supported();

			int subscribe(const char *subject, ctx::Json option, ctx::Json *request_result);
			int unsubscribe(const char *subject, ctx::Json option);
			int read(const char *subject, ctx::Json option, ctx::Json *request_result);
			int write(const char *subject, ctx::Json data, ctx::Json *request_result);

		private:
			static app_inactive_detector_provider *__instance;
			inactive_detector *engine;

			app_inactive_detector_provider();
			~app_inactive_detector_provider();

	};	/* class app_inactive_detector_provider */

}	/* namespace ctx */

#endif /* __CONTEXT_APP_INACTIVE_DETECTOR_H__ */