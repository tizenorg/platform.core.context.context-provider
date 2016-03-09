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
#include <db_mgr.h>
#include <Json.h>
#include "similar.h"
#include "places_detector.h"
#include "place_categer.h"
#include "graph.h"
#include "median.h"
#ifdef TIZEN_ENGINEER_MODE
#include "gmap.h"
#endif /* TIZEN_ENGINEER_MODE */
#include "../place_recognition_types.h"
#include <fstream>
#include <algorithm>
#include "user_places_params.h"

#define DELETE_PLACES_QUERY "DELETE FROM " PLACE_TABLE

#ifdef TIZEN_ENGINEER_MODE
#define USER_PLACES_FILE "/opt/usr/media/Others/user_places.txt" // TODO: Only for debug purposes -> Remove in final solution
#endif /* TIZEN_ENGINEER_MODE */

#define GET_VISITS_QUERY "SELECT "\
	VISIT_COLUMN_END_TIME ", "\
	VISIT_COLUMN_START_TIME ", "\
	VISIT_COLUMN_WIFI_APS ", "\
	VISIT_COLUMN_LOCATION_VALID ", "\
	VISIT_COLUMN_LOCATION_LATITUDE ", "\
	VISIT_COLUMN_LOCATION_LONGITUDE ", " \
	VISIT_COLUMN_CATEG_HOME ", "\
	VISIT_COLUMN_CATEG_WORK ", "\
	VISIT_COLUMN_CATEG_OTHER \
	" FROM " VISIT_TABLE

#define GET_PLACES_QUERY "SELECT "\
	PLACE_COLUMN_CATEG_ID ", "\
	PLACE_COLUMN_CATEG_CONFIDENCE ", "\
	PLACE_COLUMN_NAME ", "\
	PLACE_COLUMN_LOCATION_VALID ", "\
	PLACE_COLUMN_LOCATION_LATITUDE ", "\
	PLACE_COLUMN_LOCATION_LONGITUDE ", " \
	PLACE_COLUMN_WIFI_APS ", "\
	PLACE_COLUMN_CREATE_DATE \
	" FROM " PLACE_TABLE

#define PLACE_TABLE_COLUMNS \
	PLACE_COLUMN_CATEG_ID " INTEGER, "\
	PLACE_COLUMN_CATEG_CONFIDENCE " REAL, "\
	PLACE_COLUMN_NAME " TEXT, "\
	PLACE_COLUMN_LOCATION_VALID " INTEGER, "\
	PLACE_COLUMN_LOCATION_LATITUDE " REAL, "\
	PLACE_COLUMN_LOCATION_LONGITUDE " REAL, "\
	PLACE_COLUMN_WIFI_APS " STRING, "\
	PLACE_COLUMN_CREATE_DATE " timestamp"

bool ctx::PlacesDetector::onTimerExpired(int timerId)
{
	_D("");
	db_delete_places();
	return true;
}

void ctx::PlacesDetector::on_query_result_received(unsigned int query_id, int error, std::vector<Json>& records)
{
	// TODO:
	// The below "state machine" approach was choosen because it is not possible to use synchronized database queries in the main thread.
	// Probably the more elegant approach would be to use event_driven_thread class where synchronized queries are allowed.
	// Consider refactoring.
	if (error != ERR_NONE) {
		_E("on_query_result_received query_id:%d, error:%d", query_id, error);
	}
	else if (query_id == PLACES_DETECTOR_QUERY_ID_DELETE_PLACES) {
		db_delete_old_visits();
	}
	else if (query_id == PLACES_DETECTOR_QUERY_ID_DELETE_OLD_VISITS) {
		db_get_visits();
	}
	else if (query_id == PLACES_DETECTOR_QUERY_ID_GET_VISITS) {
		visits_t visits = visits_from_jsons(records);
		process_visits(visits);
	}
	else if (query_id == PLACES_DETECTOR_QUERY_ID_INSERT_VISIT) {
		// Don't do anything. It is fine.
	}
	else if (query_id == PLACES_DETECTOR_QUERY_ID_INSERT_PLACE) {
		// Don't do anything. It is fine.
	}
	else if (query_id == PLACES_DETECTOR_QUERY_ID_GET_PLACES) {
		std::vector<std::shared_ptr<Place>> db_places = places_from_jsons(records);
		detected_places_update(db_places);
	}
	else {
		_E("on_query_result_received unknown query_id:%d", query_id);
	}
}

void ctx::PlacesDetector::db_get_visits()
{
	bool ret = db_manager::execute(PLACES_DETECTOR_QUERY_ID_GET_VISITS, GET_VISITS_QUERY, this);
	_D("load visits execute query result: %s", ret ? "SUCCESS" : "FAIL");
}

void ctx::PlacesDetector::db_get_places()
{
	bool ret = db_manager::execute(PLACES_DETECTOR_QUERY_ID_GET_PLACES, GET_PLACES_QUERY, this);
	_D("load places execute query result: %s", ret ? "SUCCESS" : "FAIL");
}

double ctx::PlacesDetector::double_value_from_json(Json &row, const char* key)
{
	double value;
	row.get(NULL, key, &value);
	_D("double_value_from_json, key:%s, value: %lf", key, value);
	return value;
}

ctx::categs_t ctx::PlacesDetector::visit_categs_from_json(Json &row)
{
	categs_t categs;
	categs[PLACE_CATEG_ID_HOME] = double_value_from_json(row, VISIT_COLUMN_CATEG_HOME);
	categs[PLACE_CATEG_ID_WORK] = double_value_from_json(row, VISIT_COLUMN_CATEG_WORK);
	categs[PLACE_CATEG_ID_OTHER] = double_value_from_json(row, VISIT_COLUMN_CATEG_OTHER);
	return categs;
}

ctx::visit_s ctx::PlacesDetector::visit_from_json(Json &row)
{
	int start_time;
	int end_time;
	std::string mac_set_string;
	row.get(NULL, VISIT_COLUMN_START_TIME, &start_time);
	row.get(NULL, VISIT_COLUMN_END_TIME, &end_time);
	row.get(NULL, VISIT_COLUMN_WIFI_APS, &mac_set_string);

	std::stringstream mac_set_ss;
	mac_set_ss << mac_set_string;
	std::shared_ptr<mac_set_t> mac_set = std::make_shared<mac_set_t>();
	mac_set_ss >> *mac_set;

	interval_s interval(start_time, end_time);
	categs_t categs = visit_categs_from_json(row);

	visit_s visit(interval, mac_set, categs);

	{ // location
		int location_valid_int;
		row.get(NULL, VISIT_COLUMN_LOCATION_VALID, &location_valid_int);
		visit.location_valid = (bool) location_valid_int;
		row.get(NULL, VISIT_COLUMN_LOCATION_LATITUDE, &(visit.location.latitude));
		row.get(NULL, VISIT_COLUMN_LOCATION_LONGITUDE, &(visit.location.longitude));
	}

	return visit;
}

ctx::visits_t ctx::PlacesDetector::visits_from_jsons(std::vector<Json>& records)
{
	visits_t visits;
	_D("db_result: number of all visits: %d", records.size());

	for (Json &row : records) {
		visit_s visit = visit_from_json(row);
		visits.push_back(visit);
	}
	_D("number of all visits in vector: %d", visits.size());
	return visits;
}

std::shared_ptr<ctx::Place> ctx::PlacesDetector::place_from_json(Json &row)
{
	std::shared_ptr<Place> place = std::make_shared<Place>();
	{ // category
		int categ_id;
		row.get(NULL, PLACE_COLUMN_CATEG_ID, &categ_id);
		// This is due to the fact the JSON module API interface doesn't handle enum
		place->categ_id = static_cast<place_categ_id_e>(categ_id);
	}
	row.get(NULL, PLACE_COLUMN_NAME, &(place->name));
	row.get(NULL, PLACE_COLUMN_WIFI_APS, &(place->wifi_aps));
	{ // location
		int location_valid_int;
		row.get(NULL, PLACE_COLUMN_LOCATION_VALID, &location_valid_int);
		place->location_valid = (bool) location_valid_int;
		row.get(NULL, PLACE_COLUMN_LOCATION_LATITUDE, &(place->location.latitude));
		row.get(NULL, PLACE_COLUMN_LOCATION_LONGITUDE, &(place->location.longitude));
	}
	{ // create_date
		int create_date;
		row.get(NULL, PLACE_COLUMN_CREATE_DATE, &(create_date));
		// This is due to the fact the JSON module API interface doesn't handle time_t
		place->create_date = static_cast<time_t>(create_date);
	}
	_D("db_result: categ_id: %d; place: name: %s; wifi_aps: %s; location_valid: %d; latitude: %lf, longitude: %lf, create_date: %d", place->categ_id, place->name.c_str(), place->wifi_aps.c_str(), place->location_valid, place->location.latitude, place->location.longitude, place->create_date);
	return place;
}

std::vector<std::shared_ptr<ctx::Place>> ctx::PlacesDetector::places_from_jsons(std::vector<Json>& records)
{
	std::vector<std::shared_ptr<Place>> places;
	_D("db_result: number of all places: %d", records.size());

	for (Json &row : records) {
		std::shared_ptr<Place> place = place_from_json(row);
		places.push_back(place);
	}
	_D("number of all places in vector: %d", places.size());
	return places;
}

void ctx::PlacesDetector::reduce_outliers(ctx::visits_t &visits)
{
	int size = visits.size();
	visits.erase(std::remove_if(
					visits.begin(),
					visits.end(),
					[](visit_s v) {
						int minutes = (v.interval.end - v.interval.start) / 60;
						return (minutes < PLACES_DETECTOR_MIN_VISIT_DURATION_MINUTES)
								|| (minutes > PLACES_DETECTOR_MAX_VISIT_DURATION_MINUTES);
					}),
				visits.end());
	int new_size = visits.size();
	if (size != new_size) {
		_D("Visits number from %d to %d (to short and to long reduction)", size, new_size);
	}
}

void ctx::PlacesDetector::process_visits(ctx::visits_t &visits)
{
	reduce_outliers(visits);

	_D("test_mode = %d", test_mode);
	auto components = merge_visits(visits);
	std::vector<std::shared_ptr<Place>> new_detected_places;
#ifdef TIZEN_ENGINEER_MODE
	std::vector<visits_t> places_visits; // TODO: remove from final solution.
#endif /* TIZEN_ENGINEER_MODE */
	for (std::shared_ptr<component_t> component : *components) {
		// Small places outliers reduction
		if (!test_mode && component->size() < PLACES_DETECTOR_MIN_VISITS_PER_BIG_PLACE) {
			continue;
		}

		std::shared_ptr<visits_t> merged = std::make_shared<visits_t>();
		for (node_t i : *component) {
			merged->push_back(visits[i]);
		}
		std::shared_ptr<Place> place = place_from_merged(*merged);
		if (place->categ_id == PLACE_CATEG_ID_NONE) {
			continue;
		}
		new_detected_places.push_back(place);
		if (!test_mode) {
			db_insert_place(*place);
		}

#ifdef TIZEN_ENGINEER_MODE
		{ // TODO: Only for debug -> remove in final solution
			visits_t place_visits;
			for (node_t i : *component) {
				place_visits.push_back(visits[i]);
			}
			places_visits.push_back(place_visits);
		}
#endif /* TIZEN_ENGINEER_MODE */
	}

	detected_places_update(new_detected_places);

#ifdef TIZEN_ENGINEER_MODE
	{ // Print to file TODO: Only for debug -> remove in final solution
		std::ofstream out(USER_PLACES_FILE);
		for (size_t i = 0; i < new_detected_places.size(); i++) {
			new_detected_places[i]->print_to_stream(out);
			visits_t place_visits = places_visits[i];
			for (visit_s visit : place_visits) {
				visit.print_short_to_stream(out);
			}
		}
		out.close();
		Gmap::write_map(detected_places);
	}
#endif /* TIZEN_ENGINEER_MODE */
}

/*
 * Pseudo-atomic operation of old places replacement by new ones.
 */
void ctx::PlacesDetector::detected_places_update(std::vector<std::shared_ptr<Place>> &new_places)
{
	_D("");
	detected_places_access_mutex.lock();
	detected_places = new_places;
	new_places.clear();
	detected_places_access_mutex.unlock();
}

void ctx::PlacesDetector::merge_location(const visits_t &visits, Place &place)
{
	place.location_valid = false;
	std::vector<double> latitudes;
	std::vector<double> longitudes;
	for (const visit_s& visit : visits) {
		if (visit.location_valid) {
			latitudes.push_back(visit.location.latitude);
			longitudes.push_back(visit.location.longitude);
			place.location_valid = true;
		}
	}
	if (place.location_valid) {
		place.location.latitude = median(latitudes);
		place.location.longitude = median(longitudes);
	}
}

std::shared_ptr<ctx::Place> ctx::PlacesDetector::place_from_merged(visits_t &merged_visits)
{
	std::shared_ptr<Place> place = std::make_shared<Place>();
	place->create_date = std::time(nullptr);
	std::vector<std::shared_ptr<mac_set_t>> mac_sets;
	for (const visit_s &visit : merged_visits) {
		mac_sets.push_back(visit.mac_set);
	}
	std::shared_ptr<mac_set_t> all_macs = mac_sets_union(mac_sets);
	std::stringstream all_macs_ss;
	all_macs_ss << *all_macs;
	place->wifi_aps = all_macs_ss.str();

	merge_location(merged_visits, *place);

	PlaceCateger::categorize(merged_visits, *place);

	return place;
}

void ctx::PlacesDetector::reduce_outliers(std::shared_ptr<ctx::components_t> &cc)
{
	int size = cc->size();
	cc->erase(std::remove_if(cc->begin(),
							 cc->end(),
							 [](std::shared_ptr<component_t> &c) {
								 return c->size() < PLACES_DETECTOR_MIN_VISITS_PER_PLACE;
							 }),
			  cc->end());
	int new_size = cc->size();
	if (size != new_size) {
		_D("Connected components from %d to %d (min visit per place)", size, new_size);
	}
}

std::shared_ptr<ctx::components_t> ctx::PlacesDetector::merge_visits(const std::vector<visit_s> &visits)
{
	auto graph = graph_from_visits(visits);
	auto cc = connected_components(*graph);
	reduce_outliers(cc);
	return cc;
}

std::shared_ptr<ctx::graph_t> ctx::PlacesDetector::graph_from_visits(const std::vector<visit_s> &visits)
{
	std::shared_ptr<graph_t> graph = std::make_shared<graph_t>();
	graph->resize(visits.size());
	for (size_t i = 0; i < visits.size(); i++) {
		(*graph)[i] = std::make_shared<nhood_t>();
		for (size_t j = 0; j < i; j++) {
			if (is_joint(*visits[i].mac_set, *visits[j].mac_set)) {
				(*graph)[i]->insert(j);
				(*graph)[j]->insert(i);
			}
		}
	}
	return graph;
}

void ctx::PlacesDetector::db_delete_places()
{
	bool ret = db_manager::execute(PLACES_DETECTOR_QUERY_ID_DELETE_PLACES, DELETE_PLACES_QUERY, this);
	_D("delete places execute query result: %s", ret ? "SUCCESS" : "FAIL");
}

void ctx::PlacesDetector::db_delete_old_visits()
{
	time_t current_time;
	time(&current_time);
	time_t threshold_time = current_time - PLACES_DETECTOR_RETENTION_SECONDS;
	db_delete_older_visits(threshold_time);
}

void ctx::PlacesDetector::db_delete_older_visits(time_t threshold)
{
	_D("deleting vistits older than: %d", threshold);
	std::stringstream query;
	query << "DELETE FROM " << VISIT_TABLE;
	query << " WHERE " << VISIT_COLUMN_END_TIME << " < " << threshold;
	// query << " AND 0"; // Always false condition. Uncomment it for not deleting any visit during development.
	bool ret = db_manager::execute(PLACES_DETECTOR_QUERY_ID_DELETE_OLD_VISITS, query.str().c_str(), this);
	_D("delete old visits execute query result: %s", ret ? "SUCCESS" : "FAIL");
}

ctx::PlacesDetector::PlacesDetector(bool test_mode_)
	: test_mode(test_mode_)
{
	if (test_mode) {
		return;
	}
	db_create_table();
	db_get_places();
}

void ctx::PlacesDetector::db_create_table()
{
	bool ret = db_manager::create_table(0, PLACE_TABLE, PLACE_TABLE_COLUMNS);
	_D("db: place Table Creation Result: %s", ret ? "SUCCESS" : "FAIL");
}

void ctx::PlacesDetector::db_insert_place(const Place &place)
{
	Json data;
	data.set(NULL, PLACE_COLUMN_CATEG_ID, place.categ_id);
	data.set(NULL, PLACE_COLUMN_CATEG_CONFIDENCE, place.categ_confidence);
	data.set(NULL, PLACE_COLUMN_NAME, place.name);

	data.set(NULL, PLACE_COLUMN_LOCATION_VALID, place.location_valid);
	data.set(NULL, PLACE_COLUMN_LOCATION_LATITUDE, place.location.latitude);
	data.set(NULL, PLACE_COLUMN_LOCATION_LONGITUDE, place.location.longitude);

	data.set(NULL, PLACE_COLUMN_WIFI_APS, place.wifi_aps);
	data.set(NULL, PLACE_COLUMN_CREATE_DATE, static_cast<int>(place.create_date));

	bool ret = db_manager::insert(PLACES_DETECTOR_QUERY_ID_INSERT_PLACE, PLACE_TABLE, data);
	_D("insert place execute query result: %s", ret ? "SUCCESS" : "FAIL");
}

std::vector<std::shared_ptr<ctx::Place>> ctx::PlacesDetector::get_places()
{
	detected_places_access_mutex.lock();
	// indirect ret vector usage due to limit the scope of a mutex to only this single file / class
	std::vector<std::shared_ptr<Place>> ret = detected_places;
	detected_places_access_mutex.unlock();
	return ret;
}
