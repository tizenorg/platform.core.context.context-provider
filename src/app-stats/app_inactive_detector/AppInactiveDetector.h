/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd.
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

#ifndef _CONTEXT_APP_INACTIVE_DETECTOR_H_
#define _CONTEXT_APP_INACTIVE_DETECTOR_H_

#include <ContextProvider.h>
#include "AppInactiveDetectorTypes.h"
#include "InactiveDetector.h"

namespace ctx {

	class AppInactiveDetectorProvider : public ContextProvider {

		public:
			AppInactiveDetectorProvider();
			~AppInactiveDetectorProvider();

			static ContextProvider *create(void *data);
			static void destroy(void *data);

			int subscribe(ctx::Json option, ctx::Json *requestResult);
			int unsubscribe(ctx::Json option);
			int read(ctx::Json option, ctx::Json *requestResult);
			int write(ctx::Json data, ctx::Json *requestResult);

			bool isSupported();

		private:
			static AppInactiveDetectorProvider *__instance;
			InactiveDetector *__engine;

	};	/* class AppInactiveDetectorProvider */

}	/* namespace ctx */

#endif /* _CONTEXT_APP_INACTIVE_DETECTOR_H_ */
