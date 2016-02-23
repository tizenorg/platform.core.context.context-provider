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
#include "similar.h"
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


ctx::VisitDetector::VisitDetector(time_t t_start_scan, place_recog_mode_e energy_mode, bool test_mode_)
	: test_mode(test_mode_)
	, location_logger(this, test_mode_)
	, wifi_logger(this, energy_mode, test_mode_)
	, current_interval(t_start_scan, t_start_scan + VISIT_DETECTOR_PERIOD_SECONDS_HIGH_ACCURACY)
	, stable_counter(0)
	, tolerance(VISIT_DETECTOR_TOLERANCE_DEPTH)
	, entrance_to_place(false)
	, period_seconds(VISIT_DETECTOR_PERIOD_SECONDS_HIGH_ACCURACY)
	, entrance_time(0)
	, departure_time(0)
{
	set_period(energy_mode);
	current_interval = interval_s(t_start_scan, t_start_scan + period_seconds);
	current_logger = std::make_shared<mac_events>();
	stay_macs = std::make_shared<mac_set_t>();

	if (test_mode) {
		detected_visits = std::make_shared<ctx::visits_t>();
		return;
	}

	listeners.push_back(&location_logger);
	listeners.push_back(&wifi_logger);
	db_create_table();
	wifi_logger.start_logging();
}

ctx::VisitDetector::~VisitDetector()
{
}

ctx::interval_s ctx::VisitDetector::get_current_interval()
{
	return current_interval;
}

bool ctx::VisitDetector::is_valid(const ctx::Mac &mac)
{
	return mac != "00:00:00:00:00:00";
}

void ctx::VisitDetector::on_wifi_scan(ctx::mac_event_s e)
{
	_D("timestamp=%d, curent_interval.end=%d, mac=%s", e.timestamp, current_interval.end, std::string(e.mac).c_str());
	if (is_valid(e.mac)) {
		while (e.timestamp > current_interval.end) {
			process_current_logger();
			shift_current_interval();
		}
		current_logger->push_back(e);
	}
}

void ctx::VisitDetector::process_current_logger()
{
	_D("");
	std::shared_ptr<ctx::frame_s> frame = make_frame(this->current_logger, this->current_interval);
	detect_entrance_or_departure(frame);
	current_logger->clear();
}

std::shared_ptr<ctx::frame_s> ctx::VisitDetector::make_frame(std::shared_ptr<ctx::mac_events> logger, ctx::interval_s interval)
{
	std::set<time_t> timestamps;
	std::shared_ptr<frame_s> frame = std::make_shared<frame_s>(interval);
	for (auto log : *logger) {
		timestamps.insert(log.timestamp);
		if (frame->mac_counts.find(log.mac) == frame->mac_counts.end()) {
			frame->mac_counts[log.mac] = 1;
		} else {
			frame->mac_counts[log.mac] += 1;
		}
	}
	frame->no_timestamps = timestamps.size();
	return frame;
}

void ctx::VisitDetector::shift_current_interval()
{
	current_interval.end += period_seconds;
	current_interval.start += period_seconds;
}

void ctx::VisitDetector::detect_entrance_or_departure(std::shared_ptr<ctx::frame_s> frame)
{
	entrance_to_place ? detect_departure(frame) : detect_entrance(frame);
}

bool ctx::VisitDetector::is_disjoint(const ctx::mac_counts_t &mac_counts, const ctx::mac_set_t &mac_set)
{
	for (auto &mac : mac_set) {
		if (mac_counts.find(mac) != mac_counts.end()) {
			return false;
		}
	}
	return true;
}

bool ctx::VisitDetector::protrudes_from(const ctx::mac_counts_t &mac_counts, const ctx::mac_set_t &mac_set)
{
	for (auto &m : mac_counts) {
		if (mac_set.find(m.first) == mac_set.end()) {
			return true;
		}
	}
	return false;
}

void ctx::VisitDetector::detect_departure(std::shared_ptr<ctx::frame_s> frame)
{
	if (tolerance == VISIT_DETECTOR_TOLERANCE_DEPTH) {
		departure_time = frame->interval.start;
		buffered_frames.clear();
	} else { // tolerance < VISIT_DETECTOR_TOLERANCE_DEPTH
		buffered_frames.push_back(frame);
	}
	if (is_disjoint(frame->mac_counts, *rep_macs)) {
		if (frame->mac_counts.empty() || protrudes_from(frame->mac_counts, *stay_macs)) {
			tolerance--;
		} else { // no new macs
			buffered_frames.clear();
		}
		if (tolerance == 0) { // departure detected
			visit_end_detected();
			buffer_processing(frame);
		}
	} else if (tolerance < VISIT_DETECTOR_TOLERANCE_DEPTH) {
		tolerance++;
	}
}

void ctx::VisitDetector::visit_start_detected()
{
	entrance_to_place = true;

	locations.clear();
	if (!test_mode) {
		for (IVisitListener* listener : listeners) {
			listener->on_visit_start();
		}
	}
	rep_macs = select_representatives(history_frames);
	entrance_time = history_frames[0]->interval.start;
	_I("Entrance detected, timestamp: %d", entrance_time);
	history_reset();
}

void ctx::VisitDetector::visit_end_detected()
{
	if (!test_mode) {
		for (IVisitListener* listener : listeners) {
			listener->on_visit_end();
		}
	}
	_I("Departure detected, timestamp: %d", departure_time);

	interval_s interval(entrance_time, departure_time);
	visit_s visit(interval, rep_macs);
	VisitCateger::categorize(visit);

	put_visit_location(visit);

	if (test_mode) {
		detected_visits->push_back(visit);
	} else {
		db_insert_visit(visit);
	}

	// cleaning
	entrance_to_place = false;
	rep_macs.reset();
	tolerance = VISIT_DETECTOR_TOLERANCE_DEPTH;
}

void ctx::VisitDetector::put_visit_location(ctx::visit_s &visit)
{
	// TODO: remove small accuracy locations from vectors?
	std::vector<double> latitudes;
	std::vector<double> longitudes;
	visit.location_valid = false;
	for (location_event_s location : locations) {
		if (location.timestamp >= entrance_time && location.timestamp <= departure_time) {
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

void ctx::VisitDetector::buffer_processing(std::shared_ptr<ctx::frame_s> frame)
{
	if (buffered_frames.empty()) {
		history_frames.push_back(frame);
	} else {
		history_frames.push_back(buffered_frames[0]);
		for (size_t i = 1; i < buffered_frames.size(); i++) {
			detect_entrance(buffered_frames[i]);
			if (entrance_to_place) {
				break;
			}
		}
	}
}

void ctx::VisitDetector::detect_entrance(std::shared_ptr<ctx::frame_s> current_frame)
{
	if (current_frame->mac_counts.empty() || history_frames.empty()) {
		history_reset(current_frame);
		return;
	}

	if (stable_counter == 0) {
		std::shared_ptr<frame_s> oldest_history_frame = history_frames[0];
		stay_macs = mac_set_from_mac_counts(oldest_history_frame->mac_counts);
	}

	std::shared_ptr<mac_set_t> current_beacons = mac_set_from_mac_counts(current_frame->mac_counts);

	if (overlap_bigger_over_smaller(*current_beacons, *stay_macs) > VISIT_DETECTOR_OVERLAP) {
		stable_counter++;
		history_frames.push_back(current_frame);

		if (stable_counter == VISIT_DETECTOR_STABLE_DEPTH) { // entrance detected
			visit_start_detected();
		}
	} else {
		history_reset(current_frame);
	}
	return;
}

void ctx::VisitDetector::history_reset()
{
	stable_counter = 0;
	history_frames.clear();
}

void ctx::VisitDetector::history_reset(std::shared_ptr<frame_s> frame)
{
	history_reset();
	history_frames.push_back(frame);
}

std::shared_ptr<ctx::mac_set_t> ctx::VisitDetector::select_representatives(const std::vector<std::shared_ptr<frame_s>> &frames)
{
	mac_counts_t repr_counts;
	count_t all_count = 0;

	for (auto frame : frames) {
		all_count += frame->no_timestamps;
		for (auto &c : frame->mac_counts) {
			repr_counts[c.first] += c.second;
		}
	}

	std::shared_ptr<mac_shares_t> repr_shares = mac_shares_from_counts(repr_counts, all_count);

	share_t max_share = calc_max_share(*repr_shares);
	share_t threshold = max_share < VISIT_DETECTOR_REP_THRESHOLD ?
		max_share : VISIT_DETECTOR_REP_THRESHOLD;

	std::shared_ptr<mac_set_t> repr_mac_set = mac_set_of_greater_or_equal_share(*repr_shares, threshold);

	return repr_mac_set;
}

ctx::share_t ctx::VisitDetector::calc_max_share(const ctx::mac_shares_t &mac_shares)
{
	ctx::share_t max_value = 0.0;
	for (auto &ms : mac_shares) {
		if (ms.second > max_value) {
			max_value = ms.second;
		}
	}
	return max_value;
}

std::shared_ptr<ctx::mac_set_t> ctx::VisitDetector::mac_set_of_greater_or_equal_share(const ctx::mac_shares_t &mac_shares, ctx::share_t threshold)
{
	std::shared_ptr<mac_set_t> mac_set = std::make_shared<mac_set_t>();
	for (auto &ms : mac_shares) {
		if (ms.second >= threshold) {
			mac_set->insert(ms.first);
		}
	}
	return mac_set;
}

std::shared_ptr<ctx::mac_shares_t> ctx::VisitDetector::mac_shares_from_counts(ctx::mac_counts_t const &mac_counts, ctx::count_t denominator)
{
	std::shared_ptr<mac_shares_t> mac_shares(std::make_shared<mac_shares_t>());
	for (auto mac_count : mac_counts) {
		(*mac_shares)[mac_count.first] = (share_t) mac_count.second / denominator;
	}
	return mac_shares;
}

std::shared_ptr<ctx::visits_t> ctx::VisitDetector::get_visits()
{
	return detected_visits;
}

void ctx::VisitDetector::db_create_table()
{
	bool ret = db_manager::create_table(0, VISIT_TABLE, VISIT_TABLE_COLUMNS);
	_D("db: visit Table Creation Result: %s", ret ? "SUCCESS" : "FAIL");
}

void ctx::VisitDetector::json_put_visit_categ(Json &data, const char* key, const categs_t &categs, int categ_type)
{
	auto categ_p = categs.find(categ_type);
	if (categ_p == categs.end()) {
		_E("json_put_visit no type %d in categs", categ_type);
	} else {
		data.set(NULL, key, categ_p->second);
	}
}

void ctx::VisitDetector::json_put_visit_categs(Json &data, const categs_t &categs)
{
	json_put_visit_categ(data, VISIT_COLUMN_CATEG_HOME, categs, PLACE_CATEG_ID_HOME);
	json_put_visit_categ(data, VISIT_COLUMN_CATEG_WORK, categs, PLACE_CATEG_ID_WORK);
	json_put_visit_categ(data, VISIT_COLUMN_CATEG_OTHER, categs, PLACE_CATEG_ID_OTHER);
}

int ctx::VisitDetector::db_insert_visit(visit_s visit)
{
	std::stringstream macs_ss;
	macs_ss << *visit.mac_set;

	Json data;
	data.set(NULL, VISIT_COLUMN_WIFI_APS, macs_ss.str().c_str());

	data.set(NULL, VISIT_COLUMN_LOCATION_VALID, visit.location_valid);
	data.set(NULL, VISIT_COLUMN_LOCATION_LATITUDE, visit.location.latitude, GEO_LOCATION_PRECISION);
	data.set(NULL, VISIT_COLUMN_LOCATION_LONGITUDE, visit.location.longitude, GEO_LOCATION_PRECISION);

	data.set(NULL, VISIT_COLUMN_START_TIME, static_cast<int>(visit.interval.start));
	data.set(NULL, VISIT_COLUMN_END_TIME, static_cast<int>(visit.interval.end));

#ifdef TIZEN_ENGINEER_MODE
	std::string start_time_human = DebugUtils::human_readable_date_time(visit.interval.start, "%F %T", 80);
	std::string end_time_human = DebugUtils::human_readable_date_time(visit.interval.end, "%F %T", 80);
	data.set(NULL, VISIT_COLUMN_START_TIME_HUMAN, start_time_human.c_str());
	data.set(NULL, VISIT_COLUMN_END_TIME_HUMAN, end_time_human.c_str());

	json_put_visit_categs(data, visit.categs);

	_D("db: visit table insert interval: (%d, %d): (%s, %s)",
			visit.interval.start, visit.interval.end, start_time_human.c_str(), end_time_human.c_str());
#else
	json_put_visit_categs(data, visit.categs);

	_D("db: visit table insert interval: (%d, %d)", visit.interval.start, visit.interval.end);
#endif /* TIZEN_ENGINEER_MODE */

	bool ret = db_manager::insert(0, VISIT_TABLE, data);
	_D("db: visit table insert result: %s", ret ? "SUCCESS" : "FAIL");
	return ret;
}

void ctx::VisitDetector::on_new_location(location_event_s location_event)
{
	_D("");
	location_event.log();
	locations.push_back(location_event);
};

void ctx::VisitDetector::set_period(place_recog_mode_e energy_mode)
{
	switch (energy_mode) {
	case PLACE_RECOG_LOW_POWER_MODE:
		period_seconds = VISIT_DETECTOR_PERIOD_SECONDS_LOW_POWER;
		break;
	case PLACE_RECOG_HIGH_ACCURACY_MODE:
		period_seconds = VISIT_DETECTOR_PERIOD_SECONDS_HIGH_ACCURACY;
		break;
	default:
		_E("Incorrect energy mode");
	}
}

void ctx::VisitDetector::set_mode(place_recog_mode_e energy_mode)
{
	_D("");
	set_period(energy_mode);
	wifi_logger.set_mode(energy_mode);
}
