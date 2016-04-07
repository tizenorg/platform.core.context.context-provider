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

#ifndef _CONTEXT_PLACE_GEOFENCE_MONITOR_HANDLE_H_
#define _CONTEXT_PLACE_GEOFENCE_MONITOR_HANDLE_H_

#include <string>
#include <map>
#include <geofence_manager.h>

namespace ctx {

	class GeofenceMonitorHandle {

		public:
			GeofenceMonitorHandle();
			~GeofenceMonitorHandle();

			bool startMonitor(int placeId);
			int getPlaceId();

		private:
			int __placeId;
			geofence_state_e __prevState;
			geofence_manager_h __geoHandle;
			std::map<int, geofence_state_e> __geoStateMap;

			void __emitStateChange();
			void __stopMonitor();
			bool __startFence(int fenceId);
			void __removeFence(int fenceId);
			void __updateFence(int fenceId, geofence_manage_e manage);
			void __updateState(int fenceId, geofence_state_e state);

			static bool __fenceListCb(int geofenceId, geofence_h fence, int fenceIndex, int fenceCount, void* userData);
			static void __fenceEventCb(int placeId, int geofenceId, geofence_manager_error_e error, geofence_manage_e manage, void* userData);
			static void __fenceStateCb(int geofenceId, geofence_state_e state, void* userData);
	};

}	/* namespace ctx */

#endif /* End of _CONTEXT_PLACE_GEOFENCE_MONITOR_HANDLE_H_ */
