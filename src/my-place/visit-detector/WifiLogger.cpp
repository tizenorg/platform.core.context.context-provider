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

#include <sstream>
#include <Types.h>
#include <DatabaseManager.h>
#include "../facade/UserPlacesTypes.h"
#include "../utils/DebugUtils.h"
#include "WifiLogger.h"

#define __WIFI_CREATE_TABLE_COLUMNS \
	WIFI_COLUMN_TIMESTAMP " timestamp NOT NULL, "\
	WIFI_COLUMN_BSSID " TEXT NOT NULL, "\
	WIFI_COLUMN_ESSID " TEXT NOT NULL"

#define __WIFI_ERROR_LOG(error) { \
	if (error != WIFI_ERROR_NONE) { \
		_E("ERROR == %s", __wifiError2Str(error)); \
	} else { \
		_D("SUCCESS"); \
	} \
}

int ctx::WifiLogger::__dbCreateTable()
{
	ctx::DatabaseManager dbManager;
	bool ret = dbManager.createTable(0, WIFI_TABLE_NAME, __WIFI_CREATE_TABLE_COLUMNS, NULL, NULL);
	_D("Table Creation Request: %s", ret ? "SUCCESS" : "FAIL");
	return ret;
}

int ctx::WifiLogger::__dbInsertLogs()
{
	if (__logs.size() > 0) {
		ctx::DatabaseManager dbManager;
		std::stringstream query;
		const char* separator = " ";
		query << "BEGIN TRANSACTION; \
				INSERT INTO " WIFI_TABLE_NAME " \
				( " WIFI_COLUMN_BSSID ", " WIFI_COLUMN_ESSID ", " WIFI_COLUMN_TIMESTAMP " ) \
				VALUES";
		for (MacEvent mac_event : __logs) {
			query << separator << "( '" << mac_event.mac << "', '" << mac_event.networkName << "', '" << mac_event.timestamp << "' )";
			separator = ", ";
		}
		__logs.clear();
		query << "; \
				END TRANSACTION;";
		bool ret = dbManager.execute(0, query.str().c_str(), NULL);
		_D("DB insert request: %s", ret ? "SUCCESS" : "FAIL");
		return ret;
	}
	_D("__logs vector empty -> nothing to insert");
	return 0;
}

ctx::WifiLogger::WifiLogger(IWifiListener * listener, PlaceRecogMode energyMode) :
	__timerOn(false),
	__intervalMinutes(WIFI_LOGGER_INTERVAL_MINUTES_HIGH_ACCURACY),
	__listener(listener),
	__lastScanTime(time_t(0)),
	__lasTimerCallbackTime(time_t(0)),
	__duringVisit(false),
	__connectedToWifiAp(false),
	__started(false),
	__running(false)
{
	_D("CONSTRUCTOR");

	__setInterval(energyMode);

	if (WIFI_LOGGER_DATABASE)
		__dbCreateTable();

	__logs = std::vector<MacEvent>();

	__wifiSetDeviceStateChangedCbRequest();

	if (WIFI_LOGGER_LOW_POWER_MODE)
		__wifiSetConnectionStateChangedCbRequest();

	wifi_connection_state_e state = __wifiGetConnectionStateRequest();
	__connectedToWifiAp = (state == WIFI_CONNECTION_STATE_CONNECTED);
	_D("__connectedToWifiAp = %d, __duringVisit = %d IN CONSTRUCTOR",
				static_cast<int>(__connectedToWifiAp),
				static_cast<int>(__duringVisit));
}

ctx::WifiLogger::~WifiLogger()
{
	_D("DESTRUCTOR");
	stopLogging();
}

void ctx::WifiLogger::__wifiDeviceStateChangedCb(wifi_device_state_e state, void *userData)
{
	ctx::WifiLogger* wifiLogger = (ctx::WifiLogger *)userData;
	switch (state) {
	case WIFI_DEVICE_STATE_DEACTIVATED:
		_D("WIFI setting OFF");
		if (wifiLogger->__started)
			wifiLogger->__stopLogging();
		break;
	case WIFI_DEVICE_STATE_ACTIVATED:
		_D("WIFI setting ON");
		if (wifiLogger->__started)
			wifiLogger->__startLogging();
		break;
	default:
		break;
	}
}

void ctx::WifiLogger::__wifiConnectionStateChangedCb(wifi_connection_state_e state, wifi_ap_h ap, void *userData)
{
	ctx::WifiLogger* wifiLogger = (ctx::WifiLogger *)userData;
	switch (state) {
	case WIFI_CONNECTION_STATE_CONNECTED:
		_D("connected to AP");
		wifiLogger->__connectedToWifiAp = true;
		break;
	default:
		_D("disconnected from AP -> __lastScansPool.clear()");
		wifiLogger->__connectedToWifiAp = false;
		wifiLogger->__lastScansPool.clear();
		break;
	}
	// TODO: Check if AP bssid (MAC Address) will be helpful somehow in LOW_POWER mode
}

bool ctx::WifiLogger::__wifiFoundApCb(wifi_ap_h ap, void *userData)
{
	ctx::WifiLogger* wifiLogger = (ctx::WifiLogger *)userData;

	char *bssid = NULL;
	int ret = wifiLogger->__wifiApGetBssidRequest(ap, &bssid);
	if (ret != WIFI_ERROR_NONE)
		return false;

	char *essid = NULL;
	ret = wifiLogger->__wifiApGetEssidRequest(ap, &essid);
	if (ret != WIFI_ERROR_NONE)
		return false;

	Mac mac;
	try {
		mac = Mac(bssid);
	} catch (std::runtime_error &e) {
		_E("Cannot create mac_event. Exception: %s", e.what());
		return false;
	}

	MacEvent log(wifiLogger->__lastScanTime, mac, std::string(essid));
	if (wifiLogger->__listener) {
		wifiLogger->__listener->onWifiScan(log);
		if (WIFI_LOGGER_LOW_POWER_MODE
				&& (wifiLogger->__connectedToWifiAp || wifiLogger->__duringVisit) ) {
			// Add to last scans AP's set
			wifiLogger->__lastScansPool.insert(std::pair<std::string, std::string>(std::string(bssid), std::string(essid)));
		}
	}
	if (WIFI_LOGGER_DATABASE)
		wifiLogger->__logs.push_back(log);

	return true;
}

const char* ctx::WifiLogger::__wifiError2Str(int error)
{
	switch (error) {
	case WIFI_ERROR_INVALID_PARAMETER:
		return "WIFI_ERROR_INVALID_PARAMETER";
	case WIFI_ERROR_OUT_OF_MEMORY:
		return "WIFI_ERROR_OUT_OF_MEMORY";
	case WIFI_ERROR_INVALID_OPERATION:
		return "WIFI_ERROR_INVALID_OPERATION";
	case WIFI_ERROR_ADDRESS_FAMILY_NOT_SUPPORTED:
		return "WIFI_ERROR_ADDRESS_FAMILY_NOT_SUPPORTED";
	case WIFI_ERROR_OPERATION_FAILED:
		return "WIFI_ERROR_OPERATION_FAILED";
	case WIFI_ERROR_NO_CONNECTION:
		return "WIFI_ERROR_NO_CONNECTION";
	case WIFI_ERROR_NOW_IN_PROGRESS:
		return "WIFI_ERROR_NOW_IN_PROGRESS";
	case WIFI_ERROR_ALREADY_EXISTS:
		return "WIFI_ERROR_ALREADY_EXISTS";
	case WIFI_ERROR_OPERATION_ABORTED:
		return "WIFI_ERROR_OPERATION_ABORTED";
	case WIFI_ERROR_DHCP_FAILED:
		return "WIFI_ERROR_DHCP_FAILED";
	case WIFI_ERROR_INVALID_KEY:
		return "WIFI_ERROR_INVALID_KEY";
	case WIFI_ERROR_NO_REPLY:
		return "WIFI_ERROR_NO_REPLY";
	case WIFI_ERROR_SECURITY_RESTRICTED:
		return "WIFI_ERROR_SECURITY_RESTRICTED";
	case WIFI_ERROR_PERMISSION_DENIED:
		return "WIFI_ERROR_PERMISSION_DENIED";
	default:
		return "unknown wifi error code";
	}
}

void ctx::WifiLogger::__wifiScanFinishedCb(wifi_error_e errorCode, void *userData)
{
	ctx::WifiLogger* wifiLogger = (ctx::WifiLogger *)userData;

	time_t now = time(nullptr);
#ifdef TIZEN_ENGINEER_MODE
	double seconds = 0;
	if (wifiLogger->__lastScanTime > 0) {
		seconds = difftime(now, wifiLogger->__lastScanTime);
	}
	std::string timeStr = DebugUtils::humanReadableDateTime(now, "%T", 9);
	_D("__connectedToWifiAp = %d, __duringVisit = %d, __lastScansPool.size() = %d -> scan %s (from last : %.1fs)",
			static_cast<int>(wifiLogger->__connectedToWifiAp),
			static_cast<int>(wifiLogger->__duringVisit),
			wifiLogger->__lastScansPool.size(),
			timeStr.c_str(),
			seconds);
#endif /* TIZEN_ENGINEER_MODE */
	wifiLogger->__lastScanTime = now;

	int ret = wifiLogger->__wifiForeachFoundApsRequest(userData);
	if (ret != WIFI_ERROR_NONE)
		return;
	if (WIFI_LOGGER_DATABASE)
		wifiLogger->__dbInsertLogs();
}

bool ctx::WifiLogger::__checkWifiIsActivated()
{
	bool wifiActivated = true;
	int ret = __wifiWrapper.isActivated(&wifiActivated);
	__WIFI_ERROR_LOG(ret);
	_D("Wi-Fi is %s", wifiActivated ? "ON" : "OFF");
	return wifiActivated;
}

void ctx::WifiLogger::__wifiScanRequest()
{
	int ret = __wifiWrapper.scan(__wifiScanFinishedCb, this);
	__WIFI_ERROR_LOG(ret);
}

int ctx::WifiLogger::__wifiForeachFoundApsRequest(void *userData)
{
	int ret = __wifiWrapper.foreachFoundAP(__wifiFoundApCb, userData);
	__WIFI_ERROR_LOG(ret);
	return ret;
}

wifi_connection_state_e ctx::WifiLogger::__wifiGetConnectionStateRequest()
{
	wifi_connection_state_e connectionState;
	int ret = __wifiWrapper.getConnectionState(&connectionState);
	__WIFI_ERROR_LOG(ret);
	return connectionState;
}

void ctx::WifiLogger::__wifiSetBackgroundScanCbRequest()
{
	int ret = __wifiWrapper.setBackgroundScanCb(__wifiScanFinishedCb, this);
	__WIFI_ERROR_LOG(ret);
}

void ctx::WifiLogger::__wifiSetDeviceStateChangedCbRequest()
{
	int ret = __wifiWrapper.setDeviceStateChangedCb(__wifiDeviceStateChangedCb, this);
	__WIFI_ERROR_LOG(ret);
}

void ctx::WifiLogger::__wifiSetConnectionStateChangedCbRequest()
{
	int ret = __wifiWrapper.setConnectionStateChangedCb(__wifiConnectionStateChangedCb, this);
	__WIFI_ERROR_LOG(ret);
}

int ctx::WifiLogger::__wifiApGetEssidRequest(wifi_ap_h ap, char **essid)
{
	int ret = __wifiWrapper.getEssidFromAP(ap, essid);
	__WIFI_ERROR_LOG(ret);
	return ret;
}

int ctx::WifiLogger::__wifiApGetBssidRequest(wifi_ap_h ap, char **bssid)
{
	int ret = __wifiWrapper.getBssidFromAP(ap, bssid);
	__WIFI_ERROR_LOG(ret);
	return ret;
}

bool ctx::WifiLogger::__checkTimerId(int id)
{
	_D("id == %d, __timerId == %d", id, __timerId);
	return id == __timerId;
}

/*
 * Accepted time from last callback is >= than minimum interval
 */
bool ctx::WifiLogger::__checkTimerTime(time_t now)
{
	double seconds = 0;
	if (__lasTimerCallbackTime > 0) {
		seconds = difftime(now, __lasTimerCallbackTime);
		if (seconds < WIFI_LOGGER_ACTIVE_SCANNING_MIN_INTERVAL) {
			_D("last == %d, now == %d, diff = %.1fs -> Incorrect timer callback", __lasTimerCallbackTime, now, seconds);
			return false;
		} else {
			_D("last == %d, now == %d, diff = %.1fs -> Correct timer callback", __lasTimerCallbackTime, now, seconds);
		}
	} else {
		_D("last == %d, now == %d -> First callback", __lasTimerCallbackTime, now);
	}
	__lasTimerCallbackTime = now;
	return true;
}

bool ctx::WifiLogger::onTimerExpired(int id)
{
	time_t now = time(nullptr);
	_D("");
	if (__checkTimerId(id) == false) // Incorrect callback call
		return false;
	if (__checkTimerTime(now) == false) // Prevention from double callback call bug
		return __timerOn;
	_D("__connectedToWifiAp = %d, __duringVisit = %d, __lastScansPool.size() = %d",
			static_cast<int>(__connectedToWifiAp),
			static_cast<int>(__duringVisit),
			__lastScansPool.size());
	if (WIFI_LOGGER_LOW_POWER_MODE
			&& __duringVisit
			&& __connectedToWifiAp
			&& __lastScansPool.size() > 0) {
		_D("trying to send fake scan");
		if (__listener) {
			_D("__listener != false -> CORRECT");
			for (std::pair<std::string, std::string> ap : __lastScansPool) {
				Mac mac(ap.first);
				MacEvent scan(now, mac, ap.second);
				_D("send fake scan (%s, %s)", ap.first.c_str(), ap.second.c_str());
				__listener->onWifiScan(scan);
			}
		}
	} else {
		__wifiScanRequest();
	}
	return __timerOn;
}

void ctx::WifiLogger::startLogging()
{
	_D("");
	__started = true;
	__startLogging();
}

void ctx::WifiLogger::__startLogging()
{
	_D("");
	if (!__checkWifiIsActivated() || __running)
		return;
	__running = true;

	if (WIFI_LOGGER_ACTIVE_SCANNING) {
		__timerStart(__intervalMinutes);
		__wifiScanRequest();
	}
	if (WIFI_LOGGER_PASSIVE_SCANNING)
		__wifiSetBackgroundScanCbRequest();
}

void ctx::WifiLogger::stopLogging()
{
	_D("");
	__started = false;
	__stopLogging();
}

void ctx::WifiLogger::__stopLogging()
{
	_D("");
	if (!__running)
		return;
	if (WIFI_LOGGER_ACTIVE_SCANNING) {
		// Unset timer
		__timerOn = false;
		// Remove timer
		__timerManager.remove(__timerId);
	}
	if (WIFI_LOGGER_PASSIVE_SCANNING)
		__wifiWrapper.unsetBackgroundScanCb();
	__running = false;
}

void ctx::WifiLogger::__timerStart(time_t minutes)
{
	__timerOn = true;
	__timerId = __timerManager.setFor(minutes, this);
	_D("%s (minutes=%d)", __timerId >= 0 ? "SUCCESS" : "ERROR", minutes);
}

void ctx::WifiLogger::onVisitStart()
{
	_D("");
	__duringVisit = true;
}

void ctx::WifiLogger::onVisitEnd()
{
	_D("__lastScansPool.clear()");
	__duringVisit = false;
	__lastScansPool.clear();
}

void ctx::WifiLogger::__setInterval(PlaceRecogMode energyMode)
{
	switch (energyMode) {
	case PLACE_RECOG_LOW_POWER_MODE:
		__intervalMinutes = WIFI_LOGGER_INTERVAL_MINUTES_LOW_POWER;
		break;
	case PLACE_RECOG_HIGH_ACCURACY_MODE:
		__intervalMinutes = WIFI_LOGGER_INTERVAL_MINUTES_HIGH_ACCURACY;
		break;
	default:
		_E("Incorrect energy mode");
	}
}

void ctx::WifiLogger::__timerRestart()
{
	__timerManager.remove(__timerId);
	__timerStart(__intervalMinutes);
}

void ctx::WifiLogger::setMode(PlaceRecogMode energyMode)
{
	_D("");
	__setInterval(energyMode);
	if (WIFI_LOGGER_ACTIVE_SCANNING && __timerOn)
		__timerRestart();
}
