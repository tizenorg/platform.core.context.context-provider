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
#include "similarity.h"
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
	__dbDeletePlaces();
	__dbDeleteOldVisits();
	std::vector<Json> records = __dbGetVisits();
	visits_t visits = __visitsFromJsons(records);
	__processVisits(visits);
	return true;
}

std::vector<ctx::Json> ctx::PlacesDetector::__dbGetVisits()
{
	std::vector<Json> records;
	bool ret = db_manager::execute_sync(GET_VISITS_QUERY, &records);
	_D("load visits execute query result: %s", ret ? "SUCCESS" : "FAIL");
	return records;
}

std::vector<ctx::Json> ctx::PlacesDetector::__dbGetPlaces()
{
	std::vector<Json> records;
	bool ret = db_manager::execute_sync(GET_PLACES_QUERY, &records);
	_D("load places execute query result: %s", ret ? "SUCCESS" : "FAIL");
	return records;
}

double ctx::PlacesDetector::__doubleValueFromJson(Json &row, const char* key)
{
	double value;
	row.get(NULL, key, &value);
	_D("__doubleValueFromJson, key:%s, value: %lf", key, value);
	return value;
}

ctx::categs_t ctx::PlacesDetector::__visitCategsFromJson(Json &row)
{
	categs_t categs;
	categs[PLACE_CATEG_ID_HOME] = __doubleValueFromJson(row, VISIT_COLUMN_CATEG_HOME);
	categs[PLACE_CATEG_ID_WORK] = __doubleValueFromJson(row, VISIT_COLUMN_CATEG_WORK);
	categs[PLACE_CATEG_ID_OTHER] = __doubleValueFromJson(row, VISIT_COLUMN_CATEG_OTHER);
	return categs;
}

ctx::Visit ctx::PlacesDetector::__visitFromJson(Json &row)
{
	int start_time;
	int end_time;
	std::string mac_set_string;
	row.get(NULL, VISIT_COLUMN_START_TIME, &start_time);
	row.get(NULL, VISIT_COLUMN_END_TIME, &end_time);
	row.get(NULL, VISIT_COLUMN_WIFI_APS, &mac_set_string);

	std::stringstream mac_set_ss;
	mac_set_ss << mac_set_string;
	std::shared_ptr<mac_set_t> macSet = std::make_shared<mac_set_t>();
	mac_set_ss >> *macSet;

	Interval interval(start_time, end_time);
	categs_t categs = __visitCategsFromJson(row);

	Visit visit(interval, macSet, categs);

	{ // location
		int location_valid_int;
		row.get(NULL, VISIT_COLUMN_LOCATION_VALID, &location_valid_int);
		visit.location_valid = (bool) location_valid_int;
		row.get(NULL, VISIT_COLUMN_LOCATION_LATITUDE, &(visit.location.latitude));
		row.get(NULL, VISIT_COLUMN_LOCATION_LONGITUDE, &(visit.location.longitude));
	}

	return visit;
}

ctx::visits_t ctx::PlacesDetector::__visitsFromJsons(std::vector<Json>& records)
{
	visits_t visits;
	_D("db_result: number of all visits: %d", records.size());

	for (Json &row : records) {
		Visit visit = __visitFromJson(row);
		visits.push_back(visit);
	}
	_D("number of all visits in vector: %d", visits.size());
	return visits;
}

std::shared_ptr<ctx::Place> ctx::PlacesDetector::__placeFromJson(Json &row)
{
	std::shared_ptr<Place> place = std::make_shared<Place>();
	{ // category
		int categ_id;
		row.get(NULL, PLACE_COLUMN_CATEG_ID, &categ_id);
		// This is due to the fact the JSON module API interface doesn't handle enum
		place->categ_id = static_cast<PlaceCategId>(categ_id);
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

std::vector<std::shared_ptr<ctx::Place>> ctx::PlacesDetector::__placesFromJsons(std::vector<Json>& records)
{
	std::vector<std::shared_ptr<Place>> places;
	_D("db_result: number of all places: %d", records.size());

	for (Json &row : records) {
		std::shared_ptr<Place> place = __placeFromJson(row);
		places.push_back(place);
	}
	_D("number of all places in vector: %d", places.size());
	return places;
}

void ctx::PlacesDetector::reduceOutliers(ctx::visits_t &visits)
{
	int size = visits.size();
	visits.erase(std::remove_if(
					visits.begin(),
					visits.end(),
					[](Visit v) {
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

void ctx::PlacesDetector::__processVisits(ctx::visits_t &visits)
{
	reduceOutliers(visits);

	_D("__testMode = %d", __testMode);
	auto components = __mergeVisits(visits);
	std::vector<std::shared_ptr<Place>> newDetectedPlaces;
#ifdef TIZEN_ENGINEER_MODE
	std::vector<visits_t> places_visits; // TODO: remove from final solution.
#endif /* TIZEN_ENGINEER_MODE */
	for (std::shared_ptr<graph::Component> component : *components) {
		// Small places outliers reduction
		if (!__testMode && component->size() < PLACES_DETECTOR_MIN_VISITS_PER_BIG_PLACE) {
			continue;
		}

		std::shared_ptr<visits_t> merged = std::make_shared<visits_t>();
		for (graph::Node i : *component) {
			merged->push_back(visits[i]);
		}
		std::shared_ptr<Place> place = __placeFromMergedVisits(*merged);
		if (place->categ_id == PLACE_CATEG_ID_NONE) {
			continue;
		}
		newDetectedPlaces.push_back(place);
		if (!__testMode) {
			__dbInsertPlace(*place);
		}

#ifdef TIZEN_ENGINEER_MODE
		{ // TODO: Only for debug -> remove in final solution
			visits_t place_visits;
			for (graph::Node i : *component) {
				place_visits.push_back(visits[i]);
			}
			places_visits.push_back(place_visits);
		}
#endif /* TIZEN_ENGINEER_MODE */
	}

	__detectedPlacesUpdate(newDetectedPlaces);

#ifdef TIZEN_ENGINEER_MODE
	{ // Print to file TODO: Only for debug -> remove in final solution
		std::ofstream out(USER_PLACES_FILE);
		for (size_t i = 0; i < newDetectedPlaces.size(); i++) {
			newDetectedPlaces[i]->print_to_stream(out);
			visits_t place_visits = places_visits[i];
			for (Visit visit : place_visits) {
				visit.print_short_to_stream(out);
			}
		}
		out.close();
		Gmap::writeMap(newDetectedPlaces);
	}
#endif /* TIZEN_ENGINEER_MODE */
}

/*
 * Replace old places by new ones.
 */
void ctx::PlacesDetector::__detectedPlacesUpdate(std::vector<std::shared_ptr<Place>> &new_places)
{
	_D("");
	// XXX: In case of thread safety issues use std::mutex to protect places list.
	__detectedPlaces = new_places;
}

void ctx::PlacesDetector::__mergeLocation(const visits_t &visits, Place &place)
{
	place.location_valid = false;
	std::vector<double> latitudes;
	std::vector<double> longitudes;
	for (const Visit& visit : visits) {
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

std::shared_ptr<ctx::Place> ctx::PlacesDetector::__placeFromMergedVisits(visits_t &merged_visits)
{
	std::shared_ptr<Place> place = std::make_shared<Place>();
	place->create_date = std::time(nullptr);
	std::vector<std::shared_ptr<mac_set_t>> macSets;
	for (const Visit &visit : merged_visits) {
		macSets.push_back(visit.macSet);
	}
	std::shared_ptr<mac_set_t> all_macs = mac_sets_union(macSets);
	std::stringstream all_macs_ss;
	all_macs_ss << *all_macs;
	place->wifi_aps = all_macs_ss.str();

	__mergeLocation(merged_visits, *place);

	PlaceCateger::categorize(merged_visits, *place);

	return place;
}

void ctx::PlacesDetector::reduceOutliers(std::shared_ptr<ctx::graph::Components> &cc)
{
	int size = cc->size();
	cc->erase(std::remove_if(cc->begin(),
							 cc->end(),
							 [](std::shared_ptr<graph::Component> &c) {
								 return c->size() < PLACES_DETECTOR_MIN_VISITS_PER_PLACE;
							 }),
			  cc->end());
	int new_size = cc->size();
	if (size != new_size) {
		_D("Connected components from %d to %d (min visit per place)", size, new_size);
	}
}

std::shared_ptr<ctx::graph::Components> ctx::PlacesDetector::__mergeVisits(const std::vector<Visit> &visits)
{
	auto graph = __graphFromVisits(visits);
	auto cc = graph::connectedComponents(*graph);
	reduceOutliers(cc);
	return cc;
}

std::shared_ptr<ctx::graph::Graph> ctx::PlacesDetector::__graphFromVisits(const std::vector<Visit> &visits)
{
	std::shared_ptr<graph::Graph> graph = std::make_shared<graph::Graph>();
	graph->resize(visits.size());
	for (size_t i = 0; i < visits.size(); i++) {
		(*graph)[i] = std::make_shared<graph::NeighbourNodes>();
		for (size_t j = 0; j < i; j++) {
			if (similarity::isJoint(*visits[i].macSet, *visits[j].macSet)) {
				(*graph)[i]->insert(j);
				(*graph)[j]->insert(i);
			}
		}
	}
	return graph;
}

void ctx::PlacesDetector::__dbDeletePlaces()
{
	std::vector<Json> records;
	bool ret = db_manager::execute_sync(DELETE_PLACES_QUERY, &records);
	_D("delete places execute query result: %s", ret ? "SUCCESS" : "FAIL");
}

void ctx::PlacesDetector::__dbDeleteOldVisits()
{
	time_t current_time;
	time(&current_time);
	time_t threshold_time = current_time - PLACES_DETECTOR_RETENTION_SECONDS;
	__dbDeleteOlderVisitsThan(threshold_time);
}

void ctx::PlacesDetector::__dbDeleteOlderVisitsThan(time_t threshold)
{
	_D("deleting vistits older than: %d", threshold);
	std::stringstream query;
	query << "DELETE FROM " << VISIT_TABLE;
	query << " WHERE " << VISIT_COLUMN_END_TIME << " < " << threshold;
	// query << " AND 0"; // XXX: Always false condition. Uncomment it for not deleting any visit during development.
	std::vector<Json> records;
	bool ret = db_manager::execute_sync(query.str().c_str(), &records);
	_D("delete old visits execute query result: %s", ret ? "SUCCESS" : "FAIL");
}

ctx::PlacesDetector::PlacesDetector(bool testMode) :
	__testMode(testMode)
{
	if (testMode) {
		return;
	}
	__dbCreateTable();
	std::vector<Json> records = __dbGetPlaces();
	std::vector<std::shared_ptr<Place>> db_places = __placesFromJsons(records);
	__detectedPlacesUpdate(db_places);
}

void ctx::PlacesDetector::__dbCreateTable()
{
	bool ret = db_manager::create_table(0, PLACE_TABLE, PLACE_TABLE_COLUMNS);
	_D("db: place Table Creation Result: %s", ret ? "SUCCESS" : "FAIL");
}

void ctx::PlacesDetector::__dbInsertPlace(const Place &place)
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

	int64_t row_id;
	bool ret = db_manager::insert_sync(PLACE_TABLE, data, &row_id);
	_D("insert place execute query result: %s", ret ? "SUCCESS" : "FAIL");
}

std::vector<std::shared_ptr<ctx::Place>> ctx::PlacesDetector::getPlaces()
{
	// XXX: In case of thread safety issues use std::mutex to protect places list.
	return __detectedPlaces;
}
