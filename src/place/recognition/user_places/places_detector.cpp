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
#include <Types.h>
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

#define __DELETE_PLACES_QUERY "DELETE FROM " PLACE_TABLE

#ifdef TIZEN_ENGINEER_MODE
#define __USER_PLACES_FILE "/opt/usr/media/Others/user_places.txt" // TODO: Only for debug purposes -> Remove in final solution
#endif /* TIZEN_ENGINEER_MODE */

#define __GET_VISITS_QUERY "SELECT "\
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

#define __GET_PLACES_QUERY "SELECT "\
	PLACE_COLUMN_CATEG_ID ", "\
	PLACE_COLUMN_CATEG_CONFIDENCE ", "\
	PLACE_COLUMN_NAME ", "\
	PLACE_COLUMN_LOCATION_VALID ", "\
	PLACE_COLUMN_LOCATION_LATITUDE ", "\
	PLACE_COLUMN_LOCATION_LONGITUDE ", " \
	PLACE_COLUMN_WIFI_APS ", "\
	PLACE_COLUMN_CREATE_DATE \
	" FROM " PLACE_TABLE

#define __PLACE_TABLE_COLUMNS \
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
	Visits visits = __visitsFromJsons(records);
	__processVisits(visits);
	return true;
}

std::vector<ctx::Json> ctx::PlacesDetector::__dbGetVisits()
{
	std::vector<Json> records;
	bool ret = __dbManager.executeSync(__GET_VISITS_QUERY, &records);
	_D("load visits execute query result: %s", ret ? "SUCCESS" : "FAIL");
	return records;
}

std::vector<ctx::Json> ctx::PlacesDetector::__dbGetPlaces()
{
	std::vector<Json> records;
	bool ret = __dbManager.executeSync(__GET_PLACES_QUERY, &records);
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

ctx::Categs ctx::PlacesDetector::__visitCategsFromJson(Json &row)
{
	Categs categs;
	categs[PLACE_CATEG_ID_HOME] = __doubleValueFromJson(row, VISIT_COLUMN_CATEG_HOME);
	categs[PLACE_CATEG_ID_WORK] = __doubleValueFromJson(row, VISIT_COLUMN_CATEG_WORK);
	categs[PLACE_CATEG_ID_OTHER] = __doubleValueFromJson(row, VISIT_COLUMN_CATEG_OTHER);
	return categs;
}

ctx::Visit ctx::PlacesDetector::__visitFromJson(Json &row)
{
	int startTime;
	int endTime;
	std::string str;
	row.get(NULL, VISIT_COLUMN_START_TIME, &startTime);
	row.get(NULL, VISIT_COLUMN_END_TIME, &endTime);
	row.get(NULL, VISIT_COLUMN_WIFI_APS, &str);

	std::stringstream ss;
	ss << str;
	std::shared_ptr<MacSet> macSet = std::make_shared<MacSet>();
	ss >> *macSet;

	Interval interval(startTime, endTime);
	Categs categs = __visitCategsFromJson(row);

	Visit visit(interval, macSet, categs);

	{ // location
		int locationValidInt;
		row.get(NULL, VISIT_COLUMN_LOCATION_VALID, &locationValidInt);
		visit.locationValid = (bool) locationValidInt;
		row.get(NULL, VISIT_COLUMN_LOCATION_LATITUDE, &(visit.location.latitude));
		row.get(NULL, VISIT_COLUMN_LOCATION_LONGITUDE, &(visit.location.longitude));
	}

	return visit;
}

ctx::Visits ctx::PlacesDetector::__visitsFromJsons(std::vector<Json>& records)
{
	Visits visits;
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
		int categId;
		row.get(NULL, PLACE_COLUMN_CATEG_ID, &categId);
		// This is due to the fact the JSON module API interface doesn't handle enum
		place->categId = static_cast<PlaceCategId>(categId);
	}
	row.get(NULL, PLACE_COLUMN_NAME, &(place->name));
	row.get(NULL, PLACE_COLUMN_WIFI_APS, &(place->wifiAps));
	{ // location
		int locationValidInt;
		row.get(NULL, PLACE_COLUMN_LOCATION_VALID, &locationValidInt);
		place->locationValid = (bool) locationValidInt;
		row.get(NULL, PLACE_COLUMN_LOCATION_LATITUDE, &(place->location.latitude));
		row.get(NULL, PLACE_COLUMN_LOCATION_LONGITUDE, &(place->location.longitude));
	}
	{ // createDate
		int createDate;
		row.get(NULL, PLACE_COLUMN_CREATE_DATE, &(createDate));
		// This is due to the fact the JSON module API interface doesn't handle time_t
		place->createDate = static_cast<time_t>(createDate);
	}
	_D("db_result: categId: %d; place: name: %s; wifiAps: %s; locationValid: %d; latitude: %lf, longitude: %lf, createDate: %d", place->categId, place->name.c_str(), place->wifiAps.c_str(), place->locationValid, place->location.latitude, place->location.longitude, place->createDate);
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

void ctx::PlacesDetector::reduceOutliers(ctx::Visits &visits)
{
	int size = visits.size();
	visits.erase(std::remove_if(
					visits.begin(),
					visits.end(),
					[](Visit v)->bool {
						int minutes = (v.interval.end - v.interval.start) / 60;
						return (minutes < PLACES_DETECTOR_MIN_VISIT_DURATION_MINUTES)
								|| (minutes > PLACES_DETECTOR_MAX_VISIT_DURATION_MINUTES);
					}),
				visits.end());
	int newSize = visits.size();
	if (size != newSize) {
		_D("Visits number from %d to %d (to short and to long reduction)", size, newSize);
	}
}

void ctx::PlacesDetector::__processVisits(ctx::Visits &visits)
{
	reduceOutliers(visits);

	_D("__testMode = %d", __testMode);
	auto components = __mergeVisits(visits);
	std::vector<std::shared_ptr<Place>> newDetectedPlaces;
#ifdef TIZEN_ENGINEER_MODE
	std::vector<Visits> placesVisits; // TODO: remove from final solution.
#endif /* TIZEN_ENGINEER_MODE */
	for (std::shared_ptr<graph::Component> component : *components) {
		// Small places outliers reduction
		if (!__testMode && component->size() < PLACES_DETECTOR_MIN_VISITS_PER_BIG_PLACE) {
			continue;
		}

		std::shared_ptr<Visits> merged = std::make_shared<Visits>();
		for (graph::Node i : *component) {
			merged->push_back(visits[i]);
		}
		std::shared_ptr<Place> place = __placeFromMergedVisits(*merged);
		if (place->categId == PLACE_CATEG_ID_NONE) {
			continue;
		}
		newDetectedPlaces.push_back(place);
		if (!__testMode) {
			__dbInsertPlace(*place);
		}

#ifdef TIZEN_ENGINEER_MODE
		{ // TODO: Only for debug -> remove in final solution
			Visits placeVisits;
			for (graph::Node i : *component) {
				placeVisits.push_back(visits[i]);
			}
			placesVisits.push_back(placeVisits);
		}
#endif /* TIZEN_ENGINEER_MODE */
	}

	__detectedPlacesUpdate(newDetectedPlaces);

#ifdef TIZEN_ENGINEER_MODE
	{ // Print to file TODO: Only for debug -> remove in final solution
		std::ofstream out(__USER_PLACES_FILE);
		for (size_t i = 0; i < newDetectedPlaces.size(); i++) {
			newDetectedPlaces[i]->print2Stream(out);
			Visits placeVisits = placesVisits[i];
			for (Visit visit : placeVisits) {
				visit.printShort2Stream(out);
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
void ctx::PlacesDetector::__detectedPlacesUpdate(std::vector<std::shared_ptr<Place>> &newPlaces)
{
	_D("");
	// XXX: In case of thread safety issues use std::mutex to protect places list.
	__detectedPlaces = newPlaces;
}

void ctx::PlacesDetector::__mergeLocation(const Visits &visits, Place &place)
{
	place.locationValid = false;
	std::vector<double> latitudes;
	std::vector<double> longitudes;
	for (const Visit& visit : visits) {
		if (visit.locationValid) {
			latitudes.push_back(visit.location.latitude);
			longitudes.push_back(visit.location.longitude);
			place.locationValid = true;
		}
	}
	if (place.locationValid) {
		place.location.latitude = median(latitudes);
		place.location.longitude = median(longitudes);
	}
}

std::shared_ptr<ctx::Place> ctx::PlacesDetector::__placeFromMergedVisits(Visits &mergedVisits)
{
	std::shared_ptr<Place> place = std::make_shared<Place>();
	place->createDate = std::time(nullptr);
	std::vector<std::shared_ptr<MacSet>> macSets;
	for (const Visit &visit : mergedVisits) {
		macSets.push_back(visit.macSet);
	}
	std::shared_ptr<MacSet> allMacs = macSetsUnion(macSets);
	std::stringstream ss;
	ss << *allMacs;
	place->wifiAps = ss.str();

	__mergeLocation(mergedVisits, *place);

	PlaceCateger::categorize(mergedVisits, *place);

	return place;
}

void ctx::PlacesDetector::reduceOutliers(std::shared_ptr<ctx::graph::Components> &cc)
{
	int size = cc->size();
	cc->erase(std::remove_if(cc->begin(),
							 cc->end(),
							 [](std::shared_ptr<graph::Component> &c)->bool {
								 return c->size() < PLACES_DETECTOR_MIN_VISITS_PER_PLACE;
							 }),
			  cc->end());
	int newSize = cc->size();
	if (size != newSize) {
		_D("Connected components from %d to %d (min visit per place)", size, newSize);
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
	bool ret = __dbManager.executeSync(__DELETE_PLACES_QUERY, &records);
	_D("delete places execute query result: %s", ret ? "SUCCESS" : "FAIL");
}

void ctx::PlacesDetector::__dbDeleteOldVisits()
{
	time_t currentTime;
	time(&currentTime);
	time_t thresholdTime = currentTime - PLACES_DETECTOR_RETENTION_SECONDS;
	__dbDeleteOlderVisitsThan(thresholdTime);
}

void ctx::PlacesDetector::__dbDeleteOlderVisitsThan(time_t threshold)
{
	_D("deleting vistits older than: %d", threshold);
	std::stringstream query;
	query << "DELETE FROM " << VISIT_TABLE;
	query << " WHERE " << VISIT_COLUMN_END_TIME << " < " << threshold;
	// query << " AND 0"; // XXX: Always false condition. Uncomment it for not deleting any visit during development.
	std::vector<Json> records;
	bool ret = __dbManager.executeSync(query.str().c_str(), &records);
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
	std::vector<std::shared_ptr<Place>> dbPlaces = __placesFromJsons(records);
	__detectedPlacesUpdate(dbPlaces);
}

void ctx::PlacesDetector::__dbCreateTable()
{
	bool ret = __dbManager.createTable(0, PLACE_TABLE, __PLACE_TABLE_COLUMNS);
	_D("db: place Table Creation Result: %s", ret ? "SUCCESS" : "FAIL");
}

void ctx::PlacesDetector::__dbInsertPlace(const Place &place)
{
	Json data;
	data.set(NULL, PLACE_COLUMN_CATEG_ID, place.categId);
	data.set(NULL, PLACE_COLUMN_CATEG_CONFIDENCE, place.categConfidence);
	data.set(NULL, PLACE_COLUMN_NAME, place.name);

	data.set(NULL, PLACE_COLUMN_LOCATION_VALID, place.locationValid);
	data.set(NULL, PLACE_COLUMN_LOCATION_LATITUDE, place.location.latitude);
	data.set(NULL, PLACE_COLUMN_LOCATION_LONGITUDE, place.location.longitude);

	data.set(NULL, PLACE_COLUMN_WIFI_APS, place.wifiAps);
	data.set(NULL, PLACE_COLUMN_CREATE_DATE, static_cast<int>(place.createDate));

	int64_t rowId;
	bool ret = __dbManager.insertSync(PLACE_TABLE, data, &rowId);
	_D("insert place execute query result: %s", ret ? "SUCCESS" : "FAIL");
}

std::vector<std::shared_ptr<ctx::Place>> ctx::PlacesDetector::getPlaces()
{
	// XXX: In case of thread safety issues use std::mutex to protect places list.
	return __detectedPlaces;
}
