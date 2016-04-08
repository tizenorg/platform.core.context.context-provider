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

#include <set>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <types_internal.h>
#include <db_mgr.h>
#include <Json.h>
#include "../place_recognition_types.h"
#include "visit_detector.h"
#include "user_places_params.h"
#include "visit_categer.h"
#include "similarity.h"
#include "median.h"
#include "debug_utils.h"

#ifdef TIZEN_ENGINEER_MODE
#define VISIT_TABLE_COLUMNS \
	VISIT_COLUMN_WIFI_APS " TEXT, "\
	VISIT_COLUMN_START_TIME " timestamp, "\
	VISIT_COLUMN_END_TIME " timestamp, "\
	VISIT_COLUMN_START_TIME_HUMAN " TEXT, "\
	VISIT_COLUMN_END_TIME_HUMAN " TEXT, "\
	VISIT_COLUMN_LOCATION_VALID " INTEGER, "\
	VISIT_COLUMN_LOCATION_LATITUDE " REAL, "\
	VISIT_COLUMN_LOCATION_LONGITUDE " REAL, "\
	VISIT_COLUMN_CATEG_HOME " REAL, "\
	VISIT_COLUMN_CATEG_WORK " REAL, "\
	VISIT_COLUMN_CATEG_OTHER " REAL"
#else /* TIZEN_ENGINEER_MODE */
#define VISIT_TABLE_COLUMNS \
	VISIT_COLUMN_WIFI_APS " TEXT, "\
	VISIT_COLUMN_START_TIME " timestamp, "\
	VISIT_COLUMN_END_TIME " timestamp, "\
	VISIT_COLUMN_LOCATION_VALID " INTEGER, "\
	VISIT_COLUMN_LOCATION_LATITUDE " REAL, "\
	VISIT_COLUMN_LOCATION_LONGITUDE " REAL, "\
	VISIT_COLUMN_CATEG_HOME " REAL, "\
	VISIT_COLUMN_CATEG_WORK " REAL, "\
	VISIT_COLUMN_CATEG_OTHER " REAL"
#endif /* TIZEN_ENGINEER_MODE */

ctx::VisitDetector::VisitDetector(time_t t_start_scan, place_recog_mode_e energyMode, bool testMode) :
	__testMode(testMode),
	__locationLogger(this, testMode),
	__wifiLogger(this, energyMode, testMode),
	__currentInterval(t_start_scan, t_start_scan + VISIT_DETECTOR_PERIOD_SECONDS_HIGH_ACCURACY),
	__stableCounter(0),
	__tolerance(VISIT_DETECTOR_TOLERANCE_DEPTH),
	__entranceToPlace(false),
	__periodSeconds(VISIT_DETECTOR_PERIOD_SECONDS_HIGH_ACCURACY),
	__entranceTime(0),
	__departureTime(0)
{
	__setPeriod(energyMode);
	__currentInterval = Interval(t_start_scan, t_start_scan + __periodSeconds);
	__currentLogger = std::make_shared<mac_events>();
	__stayMacs = std::make_shared<mac_set_t>();

	if (__testMode) {
		__detectedVisits = std::make_shared<ctx::visits_t>();
		return;
	}

	__listeners.push_back(&__locationLogger);
	__listeners.push_back(&__wifiLogger);
	__dbCreateTable();
	__wifiLogger.startLogging();
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
	_D("timestamp=%d, curent_interval.end=%d, mac=%s", e.timestamp, __currentInterval.end, std::string(e.mac).c_str());
	if (__isValid(e.mac)) {
		while (e.timestamp > __currentInterval.end) {
			__processCurrentLogger();
			__shiftCurrentInterval();
		}
		__currentLogger->push_back(e);
	}
}

void ctx::VisitDetector::__processCurrentLogger()
{
	_D("");
	std::shared_ptr<ctx::Frame> frame = __makeFrame(__currentLogger, __currentInterval);
	__detectEntranceOrDeparture(frame);
	__currentLogger->clear();
}

std::shared_ptr<ctx::Frame> ctx::VisitDetector::__makeFrame(std::shared_ptr<ctx::mac_events> logger, ctx::Interval interval)
{
	std::set<time_t> timestamps;
	std::shared_ptr<Frame> frame = std::make_shared<Frame>(interval);
	for (auto log : *logger) {
		timestamps.insert(log.timestamp);
		if (frame->macCountsMap.find(log.mac) == frame->macCountsMap.end()) {
			frame->macCountsMap[log.mac] = 1;
		} else {
			frame->macCountsMap[log.mac] += 1;
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
}

bool ctx::VisitDetector::__isDisjoint(const ctx::mac_counts_t &macCountsMap, const ctx::mac_set_t &macSet)
{
	for (auto &mac : macSet) {
		if (macCountsMap.find(mac) != macCountsMap.end()) {
			return false;
		}
	}
	return true;
}

bool ctx::VisitDetector::__protrudesFrom(const ctx::mac_counts_t &macCountsMap, const ctx::mac_set_t &macSet)
{
	for (auto &m : macCountsMap) {
		if (macSet.find(m.first) == macSet.end()) {
			return true;
		}
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
	if (__isDisjoint(frame->macCountsMap, *__representativesMacs)) {
		if (frame->macCountsMap.empty() || __protrudesFrom(frame->macCountsMap, *__stayMacs)) {
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
	}

	// cleaning
	__entranceToPlace = false;
	__representativesMacs.reset();
	__tolerance = VISIT_DETECTOR_TOLERANCE_DEPTH;
}

void ctx::VisitDetector::__putLocationToVisit(ctx::Visit &visit)
{
	// TODO: remove small accuracy locations from vectors?
	std::vector<double> latitudes;
	std::vector<double> longitudes;
	visit.location_valid = false;
	for (LocationEvent location : __locationEvents) {
		if (location.timestamp >= __entranceTime && location.timestamp <= __departureTime) {
			latitudes.push_back(location.coordinates.latitude);
			longitudes.push_back(location.coordinates.longitude);
			visit.location_valid = true;
		}
	}
	if (visit.location_valid) {
		visit.location.latitude = median(latitudes);
		visit.location.longitude = median(longitudes);
		_D("visit location set: lat=%.8f, lon=%.8f", visit.location.latitude, visit.location.longitude);
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
			if (__entranceToPlace) {
				break;
			}
		}
	}
}

void ctx::VisitDetector::__detectEntrance(std::shared_ptr<ctx::Frame> current_frame)
{
	if (current_frame->macCountsMap.empty() || __historyFrames.empty()) {
		__resetHistory(current_frame);
		return;
	}

	if (__stableCounter == 0) {
		std::shared_ptr<Frame> oldest_history_frame = __historyFrames[0];
		__stayMacs = mac_set_from_mac_counts(oldest_history_frame->macCountsMap);
	}

	std::shared_ptr<mac_set_t> current_beacons = mac_set_from_mac_counts(current_frame->macCountsMap);

	if (similarity::overlapBiggerOverSmaller(*current_beacons, *__stayMacs) > VISIT_DETECTOR_OVERLAP) {
		__stableCounter++;
		__historyFrames.push_back(current_frame);

		if (__stableCounter == VISIT_DETECTOR_STABLE_DEPTH) { // entrance detected
			__visitStartDetected();
		}
	} else {
		__resetHistory(current_frame);
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

std::shared_ptr<ctx::mac_set_t> ctx::VisitDetector::__selectRepresentatives(const std::vector<std::shared_ptr<Frame>> &frames)
{
	mac_counts_t repr_counts;
	count_t all_count = 0;

	for (auto frame : frames) {
		all_count += frame->numberOfTimestamps;
		for (auto &c : frame->macCountsMap) {
			repr_counts[c.first] += c.second;
		}
	}

	std::shared_ptr<mac_shares_t> repr_shares = __macSharesFromCounts(repr_counts, all_count);

	share_t max_share = __calcMaxShare(*repr_shares);
	share_t threshold = max_share < VISIT_DETECTOR_REP_THRESHOLD ?
		max_share : VISIT_DETECTOR_REP_THRESHOLD;

	std::shared_ptr<mac_set_t> repr_mac_set = __macSetOfGreaterOrEqualShare(*repr_shares, threshold);

	return repr_mac_set;
}

ctx::share_t ctx::VisitDetector::__calcMaxShare(const ctx::mac_shares_t &mac_shares)
{
	ctx::share_t max_value = 0.0;
	for (auto &ms : mac_shares) {
		if (ms.second > max_value) {
			max_value = ms.second;
		}
	}
	return max_value;
}

std::shared_ptr<ctx::mac_set_t> ctx::VisitDetector::__macSetOfGreaterOrEqualShare(const ctx::mac_shares_t &mac_shares, ctx::share_t threshold)
{
	std::shared_ptr<mac_set_t> macSet = std::make_shared<mac_set_t>();
	for (auto &ms : mac_shares) {
		if (ms.second >= threshold) {
			macSet->insert(ms.first);
		}
	}
	return macSet;
}

std::shared_ptr<ctx::mac_shares_t> ctx::VisitDetector::__macSharesFromCounts(ctx::mac_counts_t const &macCountsMap, ctx::count_t denominator)
{
	std::shared_ptr<mac_shares_t> mac_shares(std::make_shared<mac_shares_t>());
	for (auto mac_count : macCountsMap) {
		(*mac_shares)[mac_count.first] = (share_t) mac_count.second / denominator;
	}
	return mac_shares;
}

std::shared_ptr<ctx::visits_t> ctx::VisitDetector::getVisits()
{
	return __detectedVisits;
}

void ctx::VisitDetector::__dbCreateTable()
{
	bool ret = db_manager::create_table(0, VISIT_TABLE, VISIT_TABLE_COLUMNS);
	_D("db: visit Table Creation Result: %s", ret ? "SUCCESS" : "FAIL");
}

void ctx::VisitDetector::__putVisitCategToJson(const char* key, const categs_t &categs, int categ_type, Json &data)
{
	auto categ_p = categs.find(categ_type);
	if (categ_p == categs.end()) {
		_E("json_put_visit no type %d in categs", categ_type);
	} else {
		data.set(NULL, key, categ_p->second);
	}
}

void ctx::VisitDetector::__putVisitCategsToJson(const categs_t &categs, Json &data)
{
	__putVisitCategToJson(VISIT_COLUMN_CATEG_HOME, categs, PLACE_CATEG_ID_HOME, data);
	__putVisitCategToJson(VISIT_COLUMN_CATEG_WORK, categs, PLACE_CATEG_ID_WORK, data);
	__putVisitCategToJson(VISIT_COLUMN_CATEG_OTHER, categs, PLACE_CATEG_ID_OTHER, data);
}

int ctx::VisitDetector::__dbInsertVisit(Visit visit)
{
	std::stringstream macs_ss;
	macs_ss << *visit.macSet;

	Json data;
	data.set(NULL, VISIT_COLUMN_WIFI_APS, macs_ss.str().c_str());

	data.set(NULL, VISIT_COLUMN_LOCATION_VALID, visit.location_valid);
	data.set(NULL, VISIT_COLUMN_LOCATION_LATITUDE, visit.location.latitude);
	data.set(NULL, VISIT_COLUMN_LOCATION_LONGITUDE, visit.location.longitude);

	data.set(NULL, VISIT_COLUMN_START_TIME, static_cast<int>(visit.interval.start));
	data.set(NULL, VISIT_COLUMN_END_TIME, static_cast<int>(visit.interval.end));

#ifdef TIZEN_ENGINEER_MODE
	std::string start_time_human = DebugUtils::humanReadableDateTime(visit.interval.start, "%F %T", 80);
	std::string end_time_human = DebugUtils::humanReadableDateTime(visit.interval.end, "%F %T", 80);
	data.set(NULL, VISIT_COLUMN_START_TIME_HUMAN, start_time_human.c_str());
	data.set(NULL, VISIT_COLUMN_END_TIME_HUMAN, end_time_human.c_str());
	_D("db: visit table insert interval: (%d, %d): (%s, %s)",
			visit.interval.start, visit.interval.end, start_time_human.c_str(), end_time_human.c_str());
#else
	_D("db: visit table insert interval: (%d, %d)", visit.interval.start, visit.interval.end);
#endif /* TIZEN_ENGINEER_MODE */

	__putVisitCategsToJson(visit.categs, data);

	int64_t row_id;
	bool ret = db_manager::insert_sync(VISIT_TABLE, data, &row_id);
	_D("db: visit table insert result: %s", ret ? "SUCCESS" : "FAIL");
	return ret;
}

void ctx::VisitDetector::onNewLocation(LocationEvent location_event)
{
	_D("");
	location_event.log();
	__locationEvents.push_back(location_event);
};

void ctx::VisitDetector::__setPeriod(place_recog_mode_e energyMode)
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

void ctx::VisitDetector::setMode(place_recog_mode_e energyMode)
{
	_D("");
	__setPeriod(energyMode);
	__wifiLogger.setMode(energyMode);
}
