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

#ifndef _CONTEXT_WIFI_WRAPPER_H_
#define _CONTEXT_WIFI_WRAPPER_H_

#include <set>
#include <wifi.h>

namespace ctx {

	/* Wifi API does not support multiple sessions in one process.
	   This is a wrapper class to walkaround the linitation of Wifi API. */
	class WifiWrapper {
	public:
		WifiWrapper();
		~WifiWrapper();

		int isActivated(bool *activated);

		int getConnectionState(wifi_connection_state_e *state);
		int getConnectedAP(wifi_ap_h *ap);

		int getEssidFromAP(wifi_ap_h ap, char **essid);
		int getBssidFromAP(wifi_ap_h ap, char **bssid);
		int destroyAP(wifi_ap_h ap);

		int setDeviceStateChangedCb(wifi_device_state_changed_cb callback, void *userData);
		int unsetDeviceStateChangedCb();

		int setConnectionStateChangedCb(wifi_connection_state_changed_cb callback, void *userData);
		int unsetConnectionStateChangedCb();

		int setBackgroundScanCb(wifi_scan_finished_cb callback, void *userData);
		int unsetBackgroundScanCb();

		int scan(wifi_scan_finished_cb callback, void *userData);
		int foreachFoundAP(wifi_found_ap_cb callback, void *userData);

	private:
		void __init();
		void __release();

		static void __cbFuncDeviceState(wifi_device_state_e state, void *userData);
		static void __cbFuncConnectionState(wifi_connection_state_e state, wifi_ap_h ap, void *userData);
		static void __cbFuncBackgroundScan(wifi_error_e error_code, void *userData);

		wifi_device_state_changed_cb __cbDeviceState;
		wifi_connection_state_changed_cb __cbConnectionState;
		wifi_scan_finished_cb __cbBackgroundScan;

		void *__cbDataDeviceState;
		void *__cbDataConnectionState;
		void *__cbDataBackgroundScan;

		static std::set<WifiWrapper*> __instances;
		static bool __enabledDeviceState;
		static bool __enabledConnectionState;
		static bool __enabledBackgroundScan;
	};
}

#endif	/* _CONTEXT_WIFI_WRAPPER_H_ */
