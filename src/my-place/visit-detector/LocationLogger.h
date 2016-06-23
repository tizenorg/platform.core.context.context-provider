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

#ifndef _CONTEXT_PLACE_RECOGNITION_LOCATION_LOGGER_H_
#define _CONTEXT_PLACE_RECOGNITION_LOCATION_LOGGER_H_

#include <locations.h>
#include <TimerManager.h>
#include "VisitListenerIface.h"
#include "LocationListenerIface.h"

/* Database usage flag */
#define LOCATION_LOGGER_DATABASE false // TODO: false in final solution

/* Locations measure method */
#define LOCATION_LOGGER_METHOD LOCATIONS_METHOD_HYBRID

/* TIMEOUTS: Location active measure request timeout (in seconds). */
#define LOCATION_LOGGER_ACTIVE_REQUEST_TIMEOUT_SECONDS 100

/* TIMEOUTS: Location service start timeout (in minutes). */
#define LOCATION_LOGGER_SERVICE_START_TIMEOUT_MINUTES 2

/* FREQUENCIES/INTERVALS: "Active" measure attempts frequency (in minutes) */
#define LOCATION_LOGGER_ACTIVE_INTERVAL_MINUTES 5

/* FREQUENCIES/INTERVALS: "Passive" measure attempts frequency (in minutes) */
#define LOCATION_LOGGER_PASSIVE_INTERVAL_MINUTES 30

/* ATTEMTS LIMITS: "Active" request attempts limit (must be <= than active location attempts) */
#define LOCATION_LOGGER_MAX_ACTIVE_REQUEST_ATTEMPTS 0

/* ATTEMTS LIMITS: "Active" measures attempts limit (must be <= than all attempts limit) */
#define LOCATION_LOGGER_MAX_ACTIVE_LOCATION_ATTEMPTS 2

/* ATTEMTS LIMITS: All attempts ("active" + "passive") limit */
#define LOCATION_LOGGER_MAX_LOCATION_ATTEMPTS 3

/* LOCATION LIMIT: Location count limit per visit */
#define LOCATION_LOGGER_MAX_LOCATION_COUNT 3

namespace ctx {

	enum TimerPurpose {
		LOCATION_LOGGER_WAITING_FOR_ACTIVE_REQUEST = 0,
		LOCATION_LOGGER_WAITING_FOR_SERVICE_START = 1,
		LOCATION_LOGGER_WAITING_FOR_LOCATION_METHOD_SETTING_ON = 2,
		LOCATION_LOGGER_WAITING_FOR_ACTIVE_INTERVAL = 3,
		LOCATION_LOGGER_WAITING_FOR_PASSIVE_INTERVAL = 4
	};

	class LocationLogger : public ITimerListener, public IVisitListener {

	public:
		LocationLogger(ILocationListener *listener = nullptr);
		~LocationLogger();

	private:
		/* INPUT */
		void onVisitStart();
		void onVisitEnd();

		/* OUTPUT */
		ILocationListener * const __listener;
		void __broadcast(LocationEvent locationEvent);

		/* INTERNAL */
		void __startLogging();
		void __stopLogging();
		void __locationRequest();
		void __onActiveRequestSucceeded();
		void __onActiveLocationSucceeded();

		/* INTERNAL : COUNTERS (LIMITS) */
		int __activeRequestAttempts;
		int __activeAttempts;
		int __allAttempts;
		int __locationCount;
		bool __checkGeneralLimits();
		bool __checkActiveLimits();
		bool __checkActiveRequestLimits();

		/* INTERNAL : FLAGS */
		bool __activeRequestSucceeded;
		bool __activeLocationSucceeded;

		/* TIMER */
		int __timerId;
		time_t __timerTimestamp;
		TimerManager __timerManager;
		TimerPurpose __timerPurpose;
		void __setNextTimer();
		void __activeRequestTimerStart();
		void __startServiceTimerStart();
		void __activeIntervalTimerStart();
		void __passiveIntervalTimerStart();
		void __timerStart(time_t minutes);
		void __timerStop();
		bool onTimerExpired(int timerId);

		/* DATABASE */
		static int __dbCreateTable();
		int __dbInsertLog(LocationEvent locationEvent);

		/* DEBUG */
		static const char* __locationError2Str(int error);
		static void __log(location_accessibility_state_e state);

		/* LOCATION MANAGER */
		location_manager_h __locationManager;
		void __locationManagerCreate();
		void __locationManagerDestroy();
		void __locationManagerStart();
		void __locationManagerStop();
		location_accessibility_state_e __locationManagerGetAccessibilityState();

		/* LOCATION MANAGER : LOCATION SERVICE STATE */
		location_service_state_e __locationServiceState;
		static void __locationServiceStateChangedCb(location_service_state_e state, void *userData);
		void __locationManagerSetServiceStateChangedCb();
		void __locationManagerUnsetServiceStateChangedCb();

		/* LOCATION MANAGER : LOCATION METHOD SETTINGS */
		location_method_e __locationMethod;
		bool __locationMethodState;
		bool __locationManagerIsEnabledMethod(location_method_e method);
		static void __locationSettingChangedCb(location_method_e method, bool enable, void *userData);
		void __locationManagerSetSettingChangedCb();
		void __locationManagerUnsetSettingChangedCb();

		/* LOCATION MANAGER : LOCATION */
		double __locationManagerGetHorizontalAccuracy();

		/* LOCATION MANAGER : LOCATION : SYNCHRONOUS */
		bool __locationManagerGetLocation();
		void __locationManagerGetLastLocation();

		/* LOCATION MANAGER : LOCATION : ASYNCHRONOUS */
		static void __positionUpdatedCb(double latitude, double longitude,
				double altitude, time_t timestamp, void *userData);
		static void __locationUpdatedCb(location_error_e error, double latitude,
				double longitude, double altitude, time_t timestamp, double speed,
				double direction, double climb, void *userData);
		bool __locationManagerRequestSingleLocation();

	};	/* class LocationLogger */

}	/* namespace ctx */

#endif /* End of _CONTEXT_PLACE_RECOGNITION_LOCATION_LOGGER_H_ */
