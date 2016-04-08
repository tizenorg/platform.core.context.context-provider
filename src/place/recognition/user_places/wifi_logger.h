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

#ifndef _CONTEXT_PLACE_RECOGNITION_WIFI_LOGGER_H_
#define _CONTEXT_PLACE_RECOGNITION_WIFI_LOGGER_H_

#include <wifi.h>
#include <time.h>
#include <vector>
#include <set>
#include <TimerManager.h>
#include "wifi_listener_iface.h"
#include "visit_listener_iface.h"
#include "user_places_params.h"

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
				place_recog_mode_e mode = PLACE_RECOG_HIGH_ACCURACY_MODE,
				bool testMode = false);
		~WifiLogger();

		void startLogging();
		void stopLogging();
		void setMode(place_recog_mode_e energyMode);

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
		void __setInterval(place_recog_mode_e energyMode);
		bool __checkTimerId(int id);
		bool __checkTimerTime(time_t now);
		void __timerStart(time_t minutes);
		void __timerRestart();

		/* DATABASE */
		static int __dbCreateTable();
		int __dbInsertLogs();

		/* SYSTEM CAPI WRAPPERS */
		void __wifiSetBackgroundScanCbRequest();
		void __wifiSetDeviceStateChangedCbRequest();
		void __wifiSetConnectionStateChangedCbRequest();
		static bool __checkWifiIsActivated();
		void __wifiScanRequest();
		static int __wifiForeachFoundApsRequest(void *user_data);
		static wifi_connection_state_e __wifiGetConnectionStateRequest();
		static int __wifiApGetBssidRequest(wifi_ap_h ap, char **bssid);
		void __wifiInitializeRequest();
		void __wifiDeinitializeRequest();

		/* SYSTEM CAPI CALLBACKS */
		static void __wifiDeviceStateChangedCb(wifi_device_state_e state, void *user_data);
		static void __wifiConnectionStateChangedCb(wifi_connection_state_e state, wifi_ap_h ap, void *user_data);
		static bool __wifiFoundApCb(wifi_ap_h ap, void *user_data);
		static void __wifiScanFinishedCb(wifi_error_e error_code, void *user_data);

		bool __testMode;
		IWifiListener * const __listener;
		std::vector<MacEvent> __logs;
		std::set<std::string> __lastScansPool;
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
