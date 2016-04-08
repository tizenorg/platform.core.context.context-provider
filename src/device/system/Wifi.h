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

#ifndef _DEVICE_SYSTEM_STATUS_WIFI_H_
#define _DEVICE_SYSTEM_STATUS_WIFI_H_

#include <string>
#include <wifi.h>
#include "../DeviceProviderBase.h"

namespace ctx {

	class DeviceStatusWifi : public DeviceProviderBase {

		GENERATE_PROVIDER_COMMON_DECL(DeviceStatusWifi);

	public:
		int subscribe();
		int unsubscribe();
		int read();
		static bool isSupported();
		static void submitTriggerItem();

	private:
		enum InternalState {
			UNKNOWN = -1,
			DISABLED = 0,
			UNCONNECTED,
			CONNECTED,
		};

		int __lastState;
		bool __isInitialized;
		bool __isActivated;
		wifi_connection_state_e __connState;
		std::string __bssid;

		DeviceStatusWifi();
		~DeviceStatusWifi();

		bool __getCurrentState();
		bool __getBssid();
		void __clearBssid();
		bool __getResponsePacket(Json* data);
		void __handleUpdate();
		bool __startMonitor();
		void __stopMonitor();
		static void __deviceStateChangedCb(wifi_device_state_e state, void *userData);
		static void __connectionStateChangedCb(wifi_connection_state_e state, wifi_ap_h ap, void *userData);
	};
}

#endif	// _CONTEXT_SYSTEM_STATUS_WIFI_H_
