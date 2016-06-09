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

#include <set>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <Types.h>
#include <Json.h>
#include "../facade/UserPlacesTypes.h"
#include "VisitDetector.h"
#include "../facade/UserPlacesParams.h"
#include "../visit-categer/VisitCateger.h"
#include "../utils/Similarity.h"
#include "../utils/Median.h"
#include "../utils/DebugUtils.h"

#ifdef TIZEN_ENGINEER_MODE
#define __VISIT_TABLE_COLUMNS \
	VISIT_COLUMN_WIFI_APS " TEXT, "\
	VISIT_COLUMN_START_TIME " timestamp, "\
	VISIT_COLUMN_END_TIME " timestamp, "\
	VISIT_COLUMN_START_TIME_HUMAN " TEXT, "\
	VISIT_COLUMN_END_TIME_HUMAN " TEXT, "\
	VISIT_COLUMN_LOCATION_VALID " INTEGER, "\
	VISIT_COLUMN_LOCATION_LATITUDE " REAL, "\
	VISIT_COLUMN_LOCATION_LONGITUDE " REAL, "\
	VISIT_COLUMN_LOCATION_ACCURACY " REAL, "\
	VISIT_COLUMN_CATEG_HOME " REAL, "\
	VISIT_COLUMN_CATEG_WORK " REAL, "\
	VISIT_COLUMN_CATEG_OTHER " REAL"
#else /* TIZEN_ENGINEER_MODE */
#define __VISIT_TABLE_COLUMNS \
	VISIT_COLUMN_WIFI_APS " TEXT, "\
	VISIT_COLUMN_START_TIME " timestamp, "\
	VISIT_COLUMN_END_TIME " timestamp, "\
	VISIT_COLUMN_LOCATION_VALID " INTEGER, "\
	VISIT_COLUMN_LOCATION_LATITUDE " REAL, "\
	VISIT_COLUMN_LOCATION_LONGITUDE " REAL, "\
	VISIT_COLUMN_LOCATION_ACCURACY " REAL, "\
	VISIT_COLUMN_CATEG_HOME " REAL, "\
	VISIT_COLUMN_CATEG_WORK " REAL, "\
	VISIT_COLUMN_CATEG_OTHER " REAL"
#endif /* TIZEN_ENGINEER_MODE */

#define __WIFI_APS_MAP_TABLE_COLUMNS \
	WIFI_APS_MAP_COLUMN_MAC " TEXT NOT NULL UNIQUE, "\
	WIFI_APS_MAP_COLUMN_NETWORK_NAME " TEXT NOT NULL, "\
	WIFI_APS_MAP_COLUMN_INSERT_TIME " timestamp"

ctx::VisitDetector::VisitDetector(time_t startScan, PlaceRecogMode energyMode, bool testMode) :
	__testMode(testMode),
	__locationLogger(testMode ? nullptr : new LocationLogger(this)),
	__wifiLogger(testMode ? nullptr : new WifiLogger(this, energyMode)),
	__currentInterval(startScan, startScan + VISIT_DETECTOR_PERIOD_SECONDS_HIGH_ACCURACY),
	__stableCounter(0),
	__tolerance(VISIT_DETECTOR_TOLERANCE_DEPTH),
	__entranceToPlace(false),
	__periodSeconds(VISIT_DETECTOR_PERIOD_SECONDS_HIGH_ACCURACY),
	__dbManager(testMode ? nullptr : new DatabaseManager()),
	__entranceTime(0),
	__departureTime(0)
{
	__setPeriod(energyMode);
	__currentInterval = Interval(startScan, startScan + __periodSeconds);
	__currentMacEvents = std::make_shared<MacEvents>();
	__stayMacs = std::make_shared<MacSet>();

	if (__testMode) {
		__detectedVisits = std::make_shared<ctx::Visits>();
		return;
	}

	__listeners.push_back(__locationLogger);
	__listeners.push_back(__wifiLogger);

	__dbCreateTables();
	__wifiLogger->startLogging();
}

ctx::VisitDetector::~VisitDetector()
{
}

bool ctx::VisitDetector::__isValid(const ctx::Mac &mac)
{
	return mac != "00:00:00:00:00:00";
}

void ctx::VisitDetector::onWifiScan(ctx::MacEvent e)
{
	_D("timestamp=%d, current_interval.end=%d, mac=%s, network=%s",
			e.timestamp,
			__currentInterval.end,
			std::string(e.mac).c_str(),
			e.networkName.c_str());
	if (__isValid(e.mac)) {
		while (e.timestamp > __currentInterval.end) {
			__processCurrentLogger();
			__shiftCurrentInterval();
		}
		__currentMacEvents->push_back(e);
	}
}

void ctx::VisitDetector::__processCurrentLogger()
{
	_D("");
	std::shared_ptr<ctx::Frame> frame = __makeFrame(__currentMacEvents, __currentInterval);
	__detectEntranceOrDeparture(frame);
	__currentMacEvents->clear();
}

std::shared_ptr<ctx::Frame> ctx::VisitDetector::__makeFrame(std::shared_ptr<ctx::MacEvents> logger, ctx::Interval interval)
{
	std::set<time_t> timestamps;
	std::shared_ptr<Frame> frame = std::make_shared<Frame>(interval);
	for (auto log : *logger) {
		timestamps.insert(log.timestamp);
		if (frame->macs2Counts.find(log.mac) == frame->macs2Counts.end()) {
			frame->macs2Counts[log.mac] = 1;
		} else {
			frame->macs2Counts[log.mac] += 1;
		}
	}
	frame->numberOfTimestamps = timestamps.size();
	return frame;
}

void ctx::VisitDetector::__shiftCurrentInterval()
{
	__currentInterval.end += __periodSeconds;
	__currentInterval.start += __periodSeconds;
}

void ctx::VisitDetector::__detectEntranceOrDeparture(std::shared_ptr<ctx::Frame> frame)
{
	__entranceToPlace ? __detectDeparture(frame) : __detectEntrance(frame);
	if (__entranceToPlace) {
		for (MacEvent e : *__currentMacEvents) {
			__wifiAPsMap.insert(std::pair<std::string, std::string>(e.mac, e.networkName));
		}
	}
}

bool ctx::VisitDetector::__isDisjoint(const ctx::Macs2Counts &macs2Counts, const ctx::MacSet &macSet)
{
	for (auto &mac : macSet) {
		if (macs2Counts.find(mac) != macs2Counts.end())
			return false;
	}
	return true;
}

bool ctx::VisitDetector::__protrudesFrom(const ctx::Macs2Counts &macs2Counts, const ctx::MacSet &macSet)
{
	for (auto &macCount : macs2Counts) {
		if (macSet.find(macCount.first) == macSet.end())
			return true;
	}
	return false;
}

void ctx::VisitDetector::__detectDeparture(std::shared_ptr<ctx::Frame> frame)
{
	if (__tolerance == VISIT_DETECTOR_TOLERANCE_DEPTH) {
		__departureTime = frame->interval.start;
		__bufferedFrames.clear();
	} else { // __tolerance < VISIT_DETECTOR_TOLERANCE_DEPTH
		__bufferedFrames.push_back(frame);
	}
	if (__isDisjoint(frame->macs2Counts, *__representativesMacs)) {
		if (frame->macs2Counts.empty() || __protrudesFrom(frame->macs2Counts, *__stayMacs)) {
			__tolerance--;
		} else { // no new macs
			__bufferedFrames.clear();
		}
		if (__tolerance == 0) { // departure detected
			__visitEndDetected();
			__processBuffer(frame);
		}
	} else if (__tolerance < VISIT_DETECTOR_TOLERANCE_DEPTH) {
		__tolerance++;
	}
}

void ctx::VisitDetector::__visitStartDetected()
{
	__entranceToPlace = true;

	__locationEvents.clear();
	if (!__testMode) {
		for (IVisitListener* listener : __listeners) {
			listener->onVisitStart();
		}
	}
	__representativesMacs = __selectRepresentatives(__historyFrames);
	__entranceTime = __historyFrames[0]->interval.start;
	_I("Entrance detected, timestamp: %d", __entranceTime);
	__resetHistory();
}

void ctx::VisitDetector::__visitEndDetected()
{
	if (!__testMode) {
		for (IVisitListener* listener : __listeners) {
			listener->onVisitEnd();
		}
	}
	_I("Departure detected, timestamp: %d", __departureTime);

	Interval interval(__entranceTime, __departureTime);
	Visit visit(interval, __representativesMacs);
	VisitCateger::categorize(visit);

	__putLocationToVisit(visit);

	if (__testMode) {
		__detectedVisits->push_back(visit);
	} else {
		__dbInsertVisit(visit);
		__dbInsertWifiAPsMap(visit);
	}

	// cleaning
	__entranceToPlace = false;
	__representativesMacs.reset();
	__tolerance = VISIT_DETECTOR_TOLERANCE_DEPTH;
}

void ctx::VisitDetector::__putLocationToVisit(ctx::Visit &visit)
{
	// TODO: filter out small accuracy locations?
	std::vector<double> latitudes;
	std::vector<double> longitudes;
	std::vector<double> accuracy;
	visit.locationValid = false;
	for (LocationEvent &location : __locationEvents) {
		if (location.timestamp >= __entranceTime && location.timestamp <= __departureTime) {
			latitudes.push_back(location.coordinates.latitude);
			longitudes.push_back(location.coordinates.longitude);
			accuracy.push_back(location.coordinates.accuracy);
			visit.locationValid = true;
		}
	}
	if (visit.locationValid) {
		visit.location = medianLocation(latitudes, longitudes, accuracy);
		_D("visit location set: lat=%.8f, lon=%.8f, acc=%.8f",
				visit.location.latitude,
				visit.location.longitude,
				visit.location.accuracy);
	} else {
		_D("visit location not set");
	}
}

void ctx::VisitDetector::__processBuffer(std::shared_ptr<ctx::Frame> frame)
{
	if (__bufferedFrames.empty()) {
		__historyFrames.push_back(frame);
	} else {
		__historyFrames.push_back(__bufferedFrames[0]);
		for (size_t i = 1; i < __bufferedFrames.size(); i++) {
			__detectEntrance(__bufferedFrames[i]);
			if (__entranceToPlace)
				break;
		}
	}
}

void ctx::VisitDetector::__detectEntrance(std::shared_ptr<ctx::Frame> currentFrame)
{
	if (currentFrame->macs2Counts.empty() || __historyFrames.empty()) {
		__resetHistory(currentFrame);
		return;
	}

	if (__stableCounter == 0) {
		std::shared_ptr<Frame> oldestHistoryFrame = __historyFrames[0];
		__stayMacs = macSetFromMacs2Counts(oldestHistoryFrame->macs2Counts);
	}

	std::shared_ptr<MacSet> currentBeacons = macSetFromMacs2Counts(currentFrame->macs2Counts);

	if (similarity::overlapBiggerOverSmaller(*currentBeacons, *__stayMacs) > VISIT_DETECTOR_OVERLAP) {
		__stableCounter++;
		__historyFrames.push_back(currentFrame);
		if (__stableCounter == VISIT_DETECTOR_STABLE_DEPTH) // entrance detected
			__visitStartDetected();
	} else {
		__resetHistory(currentFrame);
	}
	return;
}

void ctx::VisitDetector::__resetHistory()
{
	__stableCounter = 0;
	__historyFrames.clear();
}

void ctx::VisitDetector::__resetHistory(std::shared_ptr<Frame> frame)
{
	__resetHistory();
	__historyFrames.push_back(frame);
}

std::shared_ptr<ctx::MacSet> ctx::VisitDetector::__selectRepresentatives(const std::vector<std::shared_ptr<Frame>> &frames)
{
	Macs2Counts reprs2Counts;
	count_t allCount = 0;

	for (auto frame : frames) {
		allCount += frame->numberOfTimestamps;
		for (auto &c : frame->macs2Counts) {
			reprs2Counts[c.first] += c.second;
		}
	}

	std::shared_ptr<Macs2Shares> reprs2Shares = __macSharesFromCounts(reprs2Counts, allCount);

	share_t maxShare = __calcMaxShare(*reprs2Shares);
	share_t threshold = maxShare < VISIT_DETECTOR_REP_THRESHOLD ? maxShare : VISIT_DETECTOR_REP_THRESHOLD;

	std::shared_ptr<MacSet> reprsMacSet = __macSetOfGreaterOrEqualShare(*reprs2Shares, threshold);

	return reprsMacSet;
}

ctx::share_t ctx::VisitDetector::__calcMaxShare(const ctx::Macs2Shares &macs2Shares)
{
	ctx::share_t maxShare = 0.0;
	for (auto &macShare : macs2Shares) {
		if (macShare.second > maxShare)
			maxShare = macShare.second;
	}
	return maxShare;
}

std::shared_ptr<ctx::MacSet> ctx::VisitDetector::__macSetOfGreaterOrEqualShare(const ctx::Macs2Shares &macs2Shares, ctx::share_t threshold)
{
	std::shared_ptr<MacSet> macSet = std::make_shared<MacSet>();
	for (auto &macShare : macs2Shares) {
		if (macShare.second >= threshold)
			macSet->insert(macShare.first);
	}
	return macSet;
}

std::shared_ptr<ctx::Macs2Shares> ctx::VisitDetector::__macSharesFromCounts(ctx::Macs2Counts const &macs2Counts, ctx::count_t denominator)
{
	std::shared_ptr<Macs2Shares> macs2Shares(std::make_shared<Macs2Shares>());
	for (auto macCount : macs2Counts) {
		(*macs2Shares)[macCount.first] = (share_t) macCount.second / denominator;
	}
	return macs2Shares;
}

std::shared_ptr<ctx::Visits> ctx::VisitDetector::__getVisits()
{
	return __detectedVisits;
}

void ctx::VisitDetector::__dbCreateTables()
{
	bool ret = __dbManager->createTable(0, VISIT_TABLE, __VISIT_TABLE_COLUMNS);
	_D("db: Visit Table Creation Result: %s", ret ? "SUCCESS" : "FAIL");

	ret = __dbManager->createTable(0, WIFI_APS_MAP_TABLE, __WIFI_APS_MAP_TABLE_COLUMNS);
	_D("db: Wifi AP Map Table Creation Result: %s", ret ? "SUCCESS" : "FAIL");
}

void ctx::VisitDetector::__putVisitCategToJson(const char* key, const Categs &categs, int categType, Json &data)
{
	auto categ = categs.find(categType);
	if (categ == categs.end()) {
		_E("json_put_visit no type %d in categs", categType);
	} else {
		data.set(NULL, key, categ->second);
	}
}

void ctx::VisitDetector::__putVisitCategsToJson(const Categs &categs, Json &data)
{
	__putVisitCategToJson(VISIT_COLUMN_CATEG_HOME, categs, PLACE_CATEG_ID_HOME, data);
	__putVisitCategToJson(VISIT_COLUMN_CATEG_WORK, categs, PLACE_CATEG_ID_WORK, data);
	__putVisitCategToJson(VISIT_COLUMN_CATEG_OTHER, categs, PLACE_CATEG_ID_OTHER, data);
}

int ctx::VisitDetector::__dbInsertVisit(Visit visit)
{
	std::stringstream ss;
	ss << *visit.macSet;

	Json data;
	data.set(NULL, VISIT_COLUMN_WIFI_APS, ss.str().c_str());

	data.set(NULL, VISIT_COLUMN_LOCATION_VALID, visit.locationValid);
	data.set(NULL, VISIT_COLUMN_LOCATION_LATITUDE, visit.location.latitude);
	data.set(NULL, VISIT_COLUMN_LOCATION_LONGITUDE, visit.location.longitude);
	data.set(NULL, VISIT_COLUMN_LOCATION_ACCURACY, visit.location.accuracy);

	data.set(NULL, VISIT_COLUMN_START_TIME, static_cast<int>(visit.interval.start));
	data.set(NULL, VISIT_COLUMN_END_TIME, static_cast<int>(visit.interval.end));

#ifdef TIZEN_ENGINEER_MODE
	std::string startTimeHuman = DebugUtils::humanReadableDateTime(visit.interval.start, "%F %T", 80);
	std::string endTimeHuman = DebugUtils::humanReadableDateTime(visit.interval.end, "%F %T", 80);
	data.set(NULL, VISIT_COLUMN_START_TIME_HUMAN, startTimeHuman.c_str());
	data.set(NULL, VISIT_COLUMN_END_TIME_HUMAN, endTimeHuman.c_str());
	_D("db: visit table insert interval: (%d, %d): (%s, %s)",
			visit.interval.start, visit.interval.end, startTimeHuman.c_str(), endTimeHuman.c_str());
#else
	_D("db: visit table insert interval: (%d, %d)", visit.interval.start, visit.interval.end);
#endif /* TIZEN_ENGINEER_MODE */

	__putVisitCategsToJson(visit.categs, data);

	int64_t rowId;
	bool ret = __dbManager->insertSync(VISIT_TABLE, data, &rowId);
	_D("db: visit table insert result: %s", ret ? "SUCCESS" : "FAIL");
	return ret;
}

int ctx::VisitDetector::__dbInsertWifiAPsMap(Visit visit)
{
	std::stringstream query;
	time_t now = time(nullptr);
	const char* separator = " ";
	query << "BEGIN TRANSACTION; \
			REPLACE INTO " WIFI_APS_MAP_TABLE " \
			( " WIFI_APS_MAP_COLUMN_MAC ", " WIFI_APS_MAP_COLUMN_NETWORK_NAME ", " WIFI_APS_MAP_COLUMN_INSERT_TIME " ) \
			VALUES";
	for (Mac mac : *visit.macSet) {
		// TODO: Add protection from SQL injection in network name!!
		query << separator << "( '" << mac << "', '" << __wifiAPsMap.find(mac)->second << "', '" << now << "' )";
		separator = ", ";
	}
	__wifiAPsMap.clear();
	query << "; \
			END TRANSACTION;";
	bool ret = __dbManager->execute(0, query.str().c_str(), NULL);
	_D("DB Wifi APs map insert request: %s", ret ? "SUCCESS" : "FAIL");
	return ret;
}

void ctx::VisitDetector::onNewLocation(LocationEvent locationEvent)
{
	_D("");
	locationEvent.log();
	__locationEvents.push_back(locationEvent);
};

void ctx::VisitDetector::__setPeriod(PlaceRecogMode energyMode)
{
	switch (energyMode) {
	case PLACE_RECOG_LOW_POWER_MODE:
		__periodSeconds = VISIT_DETECTOR_PERIOD_SECONDS_LOW_POWER;
		break;
	case PLACE_RECOG_HIGH_ACCURACY_MODE:
		__periodSeconds = VISIT_DETECTOR_PERIOD_SECONDS_HIGH_ACCURACY;
		break;
	default:
		_E("Incorrect energy mode");
	}
}

void ctx::VisitDetector::setMode(PlaceRecogMode energyMode)
{
	_D("");
	__setPeriod(energyMode);
	if (__wifiLogger)
		__wifiLogger->setMode(energyMode);
}



