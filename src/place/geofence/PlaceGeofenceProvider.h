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

#ifndef _CONTEXT_PLACE_GEOFENCE_PROVIDER_H_
#define _CONTEXT_PLACE_GEOFENCE_PROVIDER_H_

#include <map>
#include <ContextProviderBase.h>
#include "GeofenceMonitorHandle.h"
#include "PlaceGeofenceTypes.h"

namespace ctx {

	class PlaceGeofenceProvider : public ContextProviderBase {

	public:
		static ContextProviderBase *create(void *data);
		static void destroy(void *data);
		static bool isSupported();
		static void submitTriggerItem();

		int subscribe(const char *subject, ctx::Json option, ctx::Json *requestResult);
		int unsubscribe(const char *subject, ctx::Json option);
		int read(const char *subject, ctx::Json option, ctx::Json *requestResult);
		int write(const char *subject, ctx::Json data, ctx::Json *requestResult);

	private:
		static PlaceGeofenceProvider *__instance;
		std::map<int, ctx::GeofenceMonitorHandle*> __handleMap;

		PlaceGeofenceProvider();
		~PlaceGeofenceProvider();

		int __subscribe(ctx::Json option);
		int __unsubscribe(ctx::Json option);
		void __destroyIfUnused();
	};

}	/* namespace ctx */

#endif /* End of _CONTEXT_PLACE_GEOFENCE_PROVIDER_H_ */
