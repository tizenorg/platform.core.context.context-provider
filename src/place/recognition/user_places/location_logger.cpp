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

#include <sstream>
#include <types_internal.h>
#include <Json.h>
#include <DatabaseManager.h>
#include "../place_recognition_types.h"
#include "user_places_params.h"
#include "debug_utils.h"
#include "location_logger.h"

#ifdef TIZEN_ENGINEER_MODE
#define __LOCATION_CREATE_TABLE_COLUMNS \
	LOCATION_COLUMN_LATITUDE " REAL NOT NULL, "\
	LOCATION_COLUMN_LONGITUDE " REAL NOT NULL, "\
	LOCATION_COLUMN_ACCURACY " REAL, "\
	LOCATION_COLUMN_TIMESTAMP " timestamp NOT NULL, "\
	LOCATION_COLUMN_TIMESTAMP_HUMAN " TEXT, "\
	LOCATION_COLUMN_METHOD " INTEGER "
#else /* TIZEN_ENGINEER_MODE */
#define __LOCATION_CREATE_TABLE_COLUMNS \
	LOCATION_COLUMN_LATITUDE " REAL NOT NULL, "\
	LOCATION_COLUMN_LONGITUDE " REAL NOT NULL, "\
	LOCATION_COLUMN_ACCURACY " REAL, "\
	LOCATION_COLUMN_TIMESTAMP " timestamp NOT NULL "
#endif /* TIZEN_ENGINEER_MODE */

#define __LOCATION_ERROR_LOG(error) { \
	if (error != LOCATIONS_ERROR_NONE) { \
		_E("ERROR == %s", __locationError2Str(error)); \
	} else { \
		_D("SUCCESS"); \
	} \
}

void ctx::LocationLogger::__locationServiceStateChangedCb(location_service_state_e state, void *userData)
{
	ctx::LocationLogger* locationLogger = (ctx::LocationLogger *)userData;
	locationLogger->__locationServiceState = state;
	if (state == LOCATIONS_SERVICE_ENABLED) {
		_D("LOCATIONS_SERVICE_ENABLED");
		switch (locationLogger->__timerPurpose) {
		case LOCATION_LOGGER_WAITING_FOR_SERVICE_START:
			_D("Waiting for location service start FINISHED");
			locationLogger->__timerStop();
			locationLogger->__locationRequest();
			break;
		case LOCATION_LOGGER_WAITING_FOR_ACTIVE_REQUEST:
		case LOCATION_LOGGER_WAITING_FOR_LOCATION_METHOD_SETTING_ON:
		case LOCATION_LOGGER_WAITING_FOR_ACTIVE_INTERVAL:
		case LOCATION_LOGGER_WAITING_FOR_PASSIVE_INTERVAL:
		default:
			// Do nothing
			break;
		}
	} else {
		_D("LOCATIONS_SERVICE_DISABLED");
//		locationLogger->__timerStop();
	}
}

void ctx::LocationLogger::__locationSettingChangedCb(location_method_e method, bool enable, void *userData)
{
	ctx::LocationLogger* locationLogger = (ctx::LocationLogger *)userData;
	locationLogger->__locationMethodState = enable;
	if (method == locationLogger->__locationMethod) {
		if (enable) {
			_D("Location method settings ON");
			switch (locationLogger->__timerPurpose) {
			case LOCATION_LOGGER_WAITING_FOR_LOCATION_METHOD_SETTING_ON:
				_D("Waiting for location method settings on FINISHED");
				if (locationLogger->__locationServiceState == LOCATIONS_SERVICE_ENABLED) {
					locationLogger->__timerStop();
					locationLogger->__locationRequest();
				} else {
					locationLogger->__locationManagerStart();
				}
				break;
			case LOCATION_LOGGER_WAITING_FOR_SERVICE_START:
			case LOCATION_LOGGER_WAITING_FOR_ACTIVE_REQUEST:
			case LOCATION_LOGGER_WAITING_FOR_ACTIVE_INTERVAL:
			case LOCATION_LOGGER_WAITING_FOR_PASSIVE_INTERVAL:
			default:
				// Do nothing
				break;
			}
		} else {
			_D("Location method settings OFF");
//			locationLogger->__timerStop();
		}
	}
}

void ctx::LocationLogger::__positionUpdatedCb(double latitude, double longitude,
		double altitude, time_t timestamp, void *userData)
{
	_D("");
	ctx::LocationLogger* locationLogger = (ctx::LocationLogger *)userData;
	double horizontal = locationLogger->__locationManagerGetHorizontalAccuracy();
#ifdef TIZEN_ENGINEER_MODE
	ctx::LocationEvent location(latitude, longitude, horizontal, timestamp, LOCATION_METHOD_REQUEST);
#else /* TIZEN_ENGINEER_MODE */
	ctx::LocationEvent location(latitude, longitude, horizontal, timestamp);
#endif /* TIZEN_ENGINEER_MODE */
	locationLogger->__broadcast(location);
	locationLogger->__onActiveRequestSucceeded();
}

void ctx::LocationLogger::__locationUpdatedCb(location_error_e error, double latitude, double longitude,
		double altitude, time_t timestamp, double speed, double direction, double climb, void *userData)
{
	_D("");
	__positionUpdatedCb(latitude, longitude, altitude, timestamp, userData);
}

const char* ctx::LocationLogger::__locationError2Str(int error)
{
	switch (error) {
	case LOCATIONS_ERROR_NONE:
		return "LOCATIONS_ERROR_NONE";
	case LOCATIONS_ERROR_OUT_OF_MEMORY:
		return "LOCATIONS_ERROR_OUT_OF_MEMORY";
	case LOCATIONS_ERROR_INVALID_PARAMETER:
		return "LOCATIONS_ERROR_INVALID_PARAMETER";
	case LOCATIONS_ERROR_ACCESSIBILITY_NOT_ALLOWED:
		return "LOCATIONS_ERROR_ACCESSIBILITY_NOT_ALLOWED";
	case LOCATIONS_ERROR_NOT_SUPPORTED:
		return "LOCATIONS_ERROR_NOT_SUPPORTED";
	case LOCATIONS_ERROR_INCORRECT_METHOD:
		return "LOCATIONS_ERROR_INCORRECT_METHOD";
	case LOCATIONS_ERROR_NETWORK_FAILED:
		return "LOCATIONS_ERROR_NETWORK_FAILED";
	case LOCATIONS_ERROR_SERVICE_NOT_AVAILABLE:
		return "LOCATIONS_ERROR_SERVICE_NOT_AVAILABLE";
	case LOCATIONS_ERROR_GPS_SETTING_OFF:
		return "LOCATIONS_ERROR_GPS_SETTING_OFF";
	case LOCATIONS_ERROR_SECURITY_RESTRICTED:
		return "LOCATIONS_ERROR_SECURITY_RESTRICTED";
	default:
		return "unknown location error code";
	}
}


void ctx::LocationLogger::__log(location_accessibility_state_e state)
{
	switch (state) {
	case LOCATIONS_ACCESS_STATE_NONE : // Access state is not determined
		_D("LOCATIONS_ACCESS_STATE_NONE ");
		break;
	case LOCATIONS_ACCESS_STATE_DENIED: // Access denied
		_D("LOCATIONS_ACCESS_STATE_DENIED");
		break;
	case LOCATIONS_ACCESS_STATE_ALLOWED: // Access authorized
		_D("LOCATIONS_ACCESS_STATE_ALLOWED");
		break;
	default:
		break;
	}
}

int ctx::LocationLogger::__dbCreateTable()
{
	ctx::DatabaseManager dbManager;
	bool ret = dbManager.createTable(0, LOCATION_TABLE_NAME, __LOCATION_CREATE_TABLE_COLUMNS, NULL, NULL);
	_D("%s -> Table Creation Request", ret ? "SUCCESS" : "FAIL");
	return 0;
}

int ctx::LocationLogger::__dbInsertLog(LocationEvent locationEvent)
{
	Json data;
	data.set(NULL, LOCATION_COLUMN_LATITUDE, locationEvent.coordinates.latitude);
	data.set(NULL, LOCATION_COLUMN_LONGITUDE, locationEvent.coordinates.longitude);
	data.set(NULL, LOCATION_COLUMN_ACCURACY, locationEvent.coordinates.accuracy);
	data.set(NULL, LOCATION_COLUMN_TIMESTAMP, static_cast<int>(locationEvent.timestamp));
#ifdef TIZEN_ENGINEER_MODE
	std::string timeHuman = DebugUtils::humanReadableDateTime(locationEvent.timestamp, "%F %T", 80);
	data.set(NULL, LOCATION_COLUMN_TIMESTAMP_HUMAN, timeHuman);
	data.set(NULL, LOCATION_COLUMN_METHOD, static_cast<int>(locationEvent.method));
#endif /* TIZEN_ENGINEER_MODE */

	ctx::DatabaseManager dbManager;
	int64_t rowId;
	bool ret = dbManager.insertSync(LOCATION_TABLE_NAME, data, &rowId);
	_D("%s -> DB: location table insert result", ret ? "SUCCESS" : "FAIL");
	return ret;
}

ctx::LocationLogger::LocationLogger(ILocationListener *listener, bool testMode) :
	__listener(listener),
	__testMode(testMode),
	__activeRequestAttempts(0),
	__activeAttempts(0),
	__allAttempts(0),
	__locationCount(0),
	__activeRequestSucceeded(false),
	__activeLocationSucceeded(false),
	__timerId(-1),
	__timerTimestamp(0),
	__timerPurpose(LOCATION_LOGGER_WAITING_FOR_PASSIVE_INTERVAL),
	__locationServiceState(LOCATIONS_SERVICE_DISABLED),
	__locationMethod(LOCATION_LOGGER_METHOD),
	__locationMethodState(false)
{
	_D("CONSTRUCTOR");

	__locationManagerCreate();

	if (__testMode) {
		return;
	}
	if (LOCATION_LOGGER_DATABASE) {
		__dbCreateTable();
	}

	__locationManagerSetServiceStateChangedCb();
	__locationManagerSetSettingChangedCb();
	__locationMethodState = __locationManagerIsEnabledMethod(__locationMethod);
}

ctx::LocationLogger::~LocationLogger()
{
	_D("DESTRUCTOR");
	__stopLogging();
	__locationManagerUnsetServiceStateChangedCb();
	__locationManagerUnsetSettingChangedCb();
	__locationManagerDestroy();
}

void ctx::LocationLogger::__locationManagerCreate()
{
	int ret = location_manager_create(__locationMethod, &__locationManager);
	__LOCATION_ERROR_LOG(ret);
}

void ctx::LocationLogger::__locationManagerDestroy()
{
	int ret = location_manager_destroy(__locationManager);
	__LOCATION_ERROR_LOG(ret);
}

void ctx::LocationLogger::__locationManagerSetServiceStateChangedCb()
{
	int ret = location_manager_set_service_state_changed_cb(__locationManager, __locationServiceStateChangedCb, this);
	__LOCATION_ERROR_LOG(ret);
}

void ctx::LocationLogger::__locationManagerUnsetServiceStateChangedCb()
{
	int ret = location_manager_unset_service_state_changed_cb(__locationManager);
	__LOCATION_ERROR_LOG(ret);
}

void ctx::LocationLogger::__locationManagerStart()
{
	int ret = location_manager_start(__locationManager);
	__LOCATION_ERROR_LOG(ret);
	__startServiceTimerStart();
}

void ctx::LocationLogger::__locationManagerStop()
{
	int ret = location_manager_stop(__locationManager);
	__LOCATION_ERROR_LOG(ret);
}

double ctx::LocationLogger::__locationManagerGetHorizontalAccuracy()
{
	location_accuracy_level_e accuracyLevel;
	double horizontal, vertical;
	int ret = location_manager_get_accuracy(__locationManager, &accuracyLevel, &horizontal, &vertical);
	__LOCATION_ERROR_LOG(ret);
	return horizontal;
}

location_accessibility_state_e ctx::LocationLogger::__locationManagerGetAccessibilityState()
{
	location_accessibility_state_e state;
	int ret = location_manager_get_accessibility_state(&state);
	__LOCATION_ERROR_LOG(ret);
	return state;
}

void ctx::LocationLogger::__locationManagerSetSettingChangedCb()
{
	int ret = location_manager_set_setting_changed_cb(__locationMethod, __locationSettingChangedCb, this);
	__LOCATION_ERROR_LOG(ret);
}

void ctx::LocationLogger::__locationManagerUnsetSettingChangedCb()
{
	int ret = location_manager_unset_setting_changed_cb(__locationMethod);
	__LOCATION_ERROR_LOG(ret);
}

bool ctx::LocationLogger::__locationManagerRequestSingleLocation()
{
	int ret = location_manager_request_single_location(__locationManager,
			LOCATION_LOGGER_ACTIVE_REQUEST_TIMEOUT_SECONDS, __locationUpdatedCb, this);
	_D("%s (seconds=%d) ----- ATTEMPTS: REQ[%d/%d], ACT[%d/%d], ALL[%d/%d]; ----- LOCATIONS:[%d/%d]",
			ret == LOCATIONS_ERROR_NONE ? "SUCCESS" : "ERROR",
			LOCATION_LOGGER_ACTIVE_REQUEST_TIMEOUT_SECONDS,
			__activeRequestAttempts,
			LOCATION_LOGGER_MAX_ACTIVE_REQUEST_ATTEMPTS,
			__activeAttempts,
			LOCATION_LOGGER_MAX_ACTIVE_LOCATION_ATTEMPTS,
			__allAttempts,
			LOCATION_LOGGER_MAX_LOCATION_ATTEMPTS,
			__locationCount,
			LOCATION_LOGGER_MAX_LOCATION_COUNT);
	__LOCATION_ERROR_LOG(ret);
	__activeRequestAttempts++;
	__activeAttempts++;
	__allAttempts++;
	if (ret == LOCATIONS_ERROR_NONE) {
		__activeRequestTimerStart();
		return true;
	} else {
		return false;
	}
}

bool ctx::LocationLogger::__locationManagerGetLocation()
{
	double altitude, latitude, longitude, climb, direction, speed, horizontal, vertical;
	location_accuracy_level_e level;
	time_t timestamp;
	int ret = location_manager_get_location(__locationManager, &altitude, &latitude, &longitude,
			&climb, &direction, &speed, &level, &horizontal, &vertical, &timestamp);
	_D("%s ----- ATTEMPTS: REQ[%d/%d], ACT[%d/%d], ALL[%d/%d]; ----- LOCATIONS:[%d/%d]",
			ret == LOCATIONS_ERROR_NONE ? "SUCCESS" : "ERROR",
			__activeRequestAttempts,
			LOCATION_LOGGER_MAX_ACTIVE_REQUEST_ATTEMPTS,
			__activeAttempts,
			LOCATION_LOGGER_MAX_ACTIVE_LOCATION_ATTEMPTS,
			__allAttempts,
			LOCATION_LOGGER_MAX_LOCATION_ATTEMPTS,
			__locationCount,
			LOCATION_LOGGER_MAX_LOCATION_COUNT);
	__LOCATION_ERROR_LOG(ret);
	__activeAttempts++;
	__allAttempts++;
	if (ret == LOCATIONS_ERROR_NONE) {
#ifdef TIZEN_ENGINEER_MODE
		ctx::LocationEvent location(latitude, longitude, horizontal, timestamp, LOCATION_METHOD_GET_LOCATION);
#else /* TIZEN_ENGINEER_MODE */
		ctx::LocationEvent location(latitude, longitude, horizontal, timestamp);
#endif /* TIZEN_ENGINEER_MODE */
		__broadcast(location);
		__onActiveLocationSucceeded();
		return true;
	} else {
		return false;
	}
}

void ctx::LocationLogger::__locationManagerGetLastLocation()
{
	double altitude, latitude, longitude, climb, direction, speed, horizontal, vertical;
	location_accuracy_level_e level;
	time_t timestamp;
	int ret = location_manager_get_last_location(__locationManager, &altitude, &latitude, &longitude,
			&climb, &direction, &speed, &level, &horizontal, &vertical, &timestamp);
	_D("%s ----- ATTEMPTS: REQ[%d/%d], ACT[%d/%d], ALL[%d/%d]; ----- LOCATIONS:[%d/%d]",
			ret == LOCATIONS_ERROR_NONE ? "SUCCESS" : "ERROR",
			__activeRequestAttempts,
			LOCATION_LOGGER_MAX_ACTIVE_REQUEST_ATTEMPTS,
			__activeAttempts,
			LOCATION_LOGGER_MAX_ACTIVE_LOCATION_ATTEMPTS,
			__allAttempts,
			LOCATION_LOGGER_MAX_LOCATION_ATTEMPTS,
			__locationCount,
			LOCATION_LOGGER_MAX_LOCATION_COUNT);
	__LOCATION_ERROR_LOG(ret);
	__allAttempts++;
	if (ret == LOCATIONS_ERROR_NONE) {
#ifdef TIZEN_ENGINEER_MODE
		ctx::LocationEvent location(latitude, longitude, horizontal, timestamp, LOCATION_METHOD_GET_LAST_LOCATION);
#else /* TIZEN_ENGINEER_MODE */
		ctx::LocationEvent location(latitude, longitude, horizontal, timestamp);
#endif /* TIZEN_ENGINEER_MODE */
		__broadcast(location);
	}
}

bool ctx::LocationLogger::__locationManagerIsEnabledMethod(location_method_e method)
{
	bool enable;
	int ret = location_manager_is_enabled_method(method, &enable);
	__LOCATION_ERROR_LOG(ret);
	return enable;
}

bool ctx::LocationLogger::__checkGeneralLimits()
{
	return (__locationCount < LOCATION_LOGGER_MAX_LOCATION_COUNT
			&& __allAttempts < LOCATION_LOGGER_MAX_LOCATION_ATTEMPTS);
}

bool ctx::LocationLogger::__checkActiveLimits()
{
	return (!__activeLocationSucceeded
			&& __activeAttempts < LOCATION_LOGGER_MAX_ACTIVE_LOCATION_ATTEMPTS);
}

bool ctx::LocationLogger::__checkActiveRequestLimits()
{
	return (!__activeRequestSucceeded
			&& __activeRequestAttempts < LOCATION_LOGGER_MAX_ACTIVE_REQUEST_ATTEMPTS);
}

void ctx::LocationLogger::__locationRequest()
{
	_D("");
	bool requestSingleLocationRet = false;
	bool getLocationRet = false;
	if (__checkGeneralLimits() && __checkActiveLimits() && __checkActiveRequestLimits()) {
		requestSingleLocationRet = __locationManagerRequestSingleLocation();
	}
	if (__checkGeneralLimits() && __checkActiveLimits() && !requestSingleLocationRet) {
		getLocationRet = __locationManagerGetLocation();
	}
	if (__checkGeneralLimits() && !requestSingleLocationRet && !getLocationRet
			&& __activeAttempts >= LOCATION_LOGGER_MAX_ACTIVE_LOCATION_ATTEMPTS) {
		__locationManagerGetLastLocation();
	}
	if (!requestSingleLocationRet) {
		__locationManagerStop();
		__setNextTimer();
	}
}

void ctx::LocationLogger::__setNextTimer()
{
	_D("ATTEMPTS: REQ[%d/%d], ACT[%d/%d], ALL[%d/%d]; ----- LOCATIONS:[%d/%d])",
			__activeRequestAttempts,
			LOCATION_LOGGER_MAX_ACTIVE_REQUEST_ATTEMPTS,
			__activeAttempts,
			LOCATION_LOGGER_MAX_ACTIVE_LOCATION_ATTEMPTS,
			__allAttempts,
			LOCATION_LOGGER_MAX_LOCATION_ATTEMPTS,
			__locationCount,
			LOCATION_LOGGER_MAX_LOCATION_COUNT);
	if (__checkGeneralLimits()) {
		if (__checkActiveLimits()) {
			__activeIntervalTimerStart();
		} else {
			__passiveIntervalTimerStart();
		}
	}
}

void ctx::LocationLogger::__onActiveRequestSucceeded()
{
	_D("");
	__locationManagerStop();
	__activeRequestSucceeded = true;
	__onActiveLocationSucceeded();
}

void ctx::LocationLogger::__onActiveLocationSucceeded()
{
	_D("");
	__activeLocationSucceeded = true;
}

void ctx::LocationLogger::__broadcast(ctx::LocationEvent locationEvent)
{
	_D("");
	__locationCount++;
	if (__listener) {
		__listener->onNewLocation(locationEvent);
	}
	if (LOCATION_LOGGER_DATABASE) {
		__dbInsertLog(locationEvent);
	}
}

bool ctx::LocationLogger::onTimerExpired(int id)
{
	time_t now = time(nullptr);
	double seconds = difftime(now, __timerTimestamp);

	switch (__timerPurpose) {
	case LOCATION_LOGGER_WAITING_FOR_ACTIVE_REQUEST:
		_D("Active request FAILED, timerId = %d[%d], from start = %.1fs", id, __timerId, seconds);
		__locationManagerStop();
		__setNextTimer();
		return false;
	case LOCATION_LOGGER_WAITING_FOR_SERVICE_START:
		_D("Service start in timeout time FAILED, timerId = %d[%d], from start = %.1fs", id, __timerId, seconds);
		// Waiting for service start FAILURE is also some kind of active request attempt
		__activeRequestAttempts++;
		__activeAttempts++;
		__allAttempts++;
		__locationManagerStop();
		__setNextTimer();
		return false;
	case LOCATION_LOGGER_WAITING_FOR_LOCATION_METHOD_SETTING_ON:
		_D("Still waiting for Location method settings on, timerId = %d[%d], from start = %.1fs", id, __timerId, seconds);
		// Do nothing
		return false;
	case LOCATION_LOGGER_WAITING_FOR_ACTIVE_INTERVAL:
		_D("Active interval time expired, timerId = %d[%d], from start = %.1fs", id, __timerId, seconds);
		break;
	case LOCATION_LOGGER_WAITING_FOR_PASSIVE_INTERVAL:
		_D("Passive interval time expired, timerId = %d[%d], from start = %.1fs", id, __timerId, seconds);
		break;
	default:
		_D("Do nothing, timerId = %d[%d], from start = %.1fs", id, __timerId, seconds);
		return false;
	}
	if (__locationMethodState) {
		__locationManagerStart();
	} else {
		__timerPurpose = LOCATION_LOGGER_WAITING_FOR_LOCATION_METHOD_SETTING_ON;
		_D("LOCATION_LOGGER_WAITING_FOR_LOCATION_METHOD_SETTING_ON");
	}
	return false;
}

void ctx::LocationLogger::__activeRequestTimerStart()
{
	int minutes = LOCATION_LOGGER_ACTIVE_REQUEST_TIMEOUT_SECONDS / 60;
	if (LOCATION_LOGGER_ACTIVE_REQUEST_TIMEOUT_SECONDS % 60) {
		minutes++;
	}
	__timerPurpose = LOCATION_LOGGER_WAITING_FOR_ACTIVE_REQUEST;
	_D("LOCATION_LOGGER_WAITING_FOR_ACTIVE_REQUEST (minutes=%d)", minutes);
	__timerStart(minutes);
}

void ctx::LocationLogger::__startServiceTimerStart()
{
	__timerPurpose = LOCATION_LOGGER_WAITING_FOR_SERVICE_START;
	_D("LOCATION_LOGGER_WAITING_FOR_SERVICE_START");
	__timerStart(LOCATION_LOGGER_SERVICE_START_TIMEOUT_MINUTES);
}

void ctx::LocationLogger::__activeIntervalTimerStart()
{
	__timerPurpose = LOCATION_LOGGER_WAITING_FOR_ACTIVE_INTERVAL;
	_D("LOCATION_LOGGER_WAITING_FOR_ACTIVE_INTERVAL");
	__timerStart(LOCATION_LOGGER_ACTIVE_INTERVAL_MINUTES);
}

void ctx::LocationLogger::__passiveIntervalTimerStart()
{
	__timerPurpose = LOCATION_LOGGER_WAITING_FOR_PASSIVE_INTERVAL;
	_D("LOCATION_LOGGER_WAITING_FOR_PASSIVE_INTERVAL");
	__timerStart(LOCATION_LOGGER_PASSIVE_INTERVAL_MINUTES);
}

void ctx::LocationLogger::__timerStart(time_t minutes)
{
	__timerTimestamp = time(nullptr);
	__timerId = __timerManager.setFor(minutes, this);
	_D("%s (minutes=%d) timerId = %d", __timerId >= 0 ? "SUCCESS" : "ERROR", minutes, __timerId);
}

void ctx::LocationLogger::__timerStop()
{
	_D("");
	__timerManager.remove(__timerId);
}

void ctx::LocationLogger::__startLogging()
{
	_D("");
	__activeRequestAttempts = 0;
	__activeAttempts = 0;
	__allAttempts = 0;
	__locationCount = 0;
	__activeRequestSucceeded = false;;
	__activeLocationSucceeded = false;
	__locationManagerStart();
}

void ctx::LocationLogger::__stopLogging()
{
	_D("");
	__timerStop();
	__locationManagerStop();
}

void ctx::LocationLogger::onVisitStart()
{
	_D("");
	if (!__testMode) {
		__startLogging();
	}
}

void ctx::LocationLogger::onVisitEnd()
{
	_D("");
	if (!__testMode) {
		__stopLogging();
	}
}

#undef __LOCATION_ERROR_LOG
