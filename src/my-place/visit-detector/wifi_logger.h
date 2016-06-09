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

#ifndef _CONTEXT_PLACE_RECOGNITION_WIFI_LOGGER_H_
#define _CONTEXT_PLACE_RECOGNITION_WIFI_LOGGER_H_

#include <WifiWrapper.h>
#include <time.h>
#include <vector>
#include <map>
#include <TimerManager.h>
#include "wifi_listener_iface.h"
#include "visit_listener_iface.h"
#include "../facade/user_places_params.h"

/* Database usage flag */
#define WIFI_LOGGER_DATABASE false

/* Active scanning usage flag */
#define WIFI_LOGGER_ACTIVE_SCANNING true

/* Passive scanning usage flag */
#define WIFI_LOGGER_PASSIVE_SCANNING true

/* Active scanning minimum interval in seconds */
#define WIFI_LOGGER_ACTIVE_SCANNING_MIN_INTERVAL 10

/*
 * Low power scanning usage flag
 * (When phone is connected to some WiFi Access Point
 * last scan data is returned instead of new scan triggering)
 */
#define WIFI_LOGGER_LOW_POWER_MODE false

namespace ctx {

	class WifiLogger : public ITimerListener, public IVisitListener {

	public:
		WifiLogger(IWifiListener * listener = nullptr,
				PlaceRecogMode energyMode = PLACE_RECOG_HIGH_ACCURACY_MODE);
		~WifiLogger();

		void startLogging();
		void stopLogging();
		void setMode(PlaceRecogMode energyMode);

	private:
		/* INPUT */
		void onVisitStart();
		void onVisitEnd();

		bool onTimerExpired(int timerId);

		/* TIMER */
		bool __timerOn;
		int __timerId;
		int __intervalMinutes;
		TimerManager __timerManager;
		void __setInterval(PlaceRecogMode energyMode);
		bool __checkTimerId(int id);
		bool __checkTimerTime(time_t now);
		void __timerStart(time_t minutes);
		void __timerRestart();

		/* DATABASE */
		static int __dbCreateTable();
		int __dbInsertLogs();

		/* SYSTEM CAPI WRAPPERS */
		WifiWrapper __wifiWrapper;
		void __wifiSetBackgroundScanCbRequest();
		void __wifiSetDeviceStateChangedCbRequest();
		void __wifiSetConnectionStateChangedCbRequest();
		bool __checkWifiIsActivated();
		void __wifiScanRequest();
		int __wifiForeachFoundApsRequest(void *userData);
		wifi_connection_state_e __wifiGetConnectionStateRequest();
		int __wifiApGetEssidRequest(wifi_ap_h ap, char **essid);
		int __wifiApGetBssidRequest(wifi_ap_h ap, char **bssid);

		/* SYSTEM CAPI CALLBACKS */
		static void __wifiDeviceStateChangedCb(wifi_device_state_e state, void *userData);
		static void __wifiConnectionStateChangedCb(wifi_connection_state_e state, wifi_ap_h ap, void *userData);
		static bool __wifiFoundApCb(wifi_ap_h ap, void *userData);
		static void __wifiScanFinishedCb(wifi_error_e errorCode, void *userData);

		IWifiListener * const __listener;
		std::vector<MacEvent> __logs;
		std::map<std::string, std::string> __lastScansPool; // Mac address to network name map
		time_t __lastScanTime;
		time_t __lasTimerCallbackTime;
		bool __duringVisit;
		bool __connectedToWifiAp;
		bool __started;
		bool __running;

		void __startLogging();
		void __stopLogging();
		static const char* __wifiError2Str(int error);

	};	/* class WifiLogger */

}	/* namespace ctx */

#endif /* End of _CONTEXT_PLACE_RECOGNITION_WIFI_LOGGER_H_ */
