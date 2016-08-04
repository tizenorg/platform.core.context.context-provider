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

#include <ctime>
#include <memory>
#include <sstream>
#include <Types.h>
#include "UserPlaces.h"
#include <MyPlaceTypes.h>
#include <gmodule.h>
#include <DatabaseManager.h>

#define SO_PATH "/usr/lib/context-service/libctx-prvd-my-place-places-detector.so"

typedef void (*places_detector_t)();

#define __GET_PLACES_QUERY "SELECT "\
	PLACE_COLUMN_CATEG_ID ", "\
	PLACE_COLUMN_CATEG_CONFIDENCE ", "\
	PLACE_COLUMN_NAME ", "\
	PLACE_COLUMN_LOCATION_VALID ", "\
	PLACE_COLUMN_LOCATION_LATITUDE ", "\
	PLACE_COLUMN_LOCATION_LONGITUDE ", "\
	PLACE_COLUMN_LOCATION_ACCURACY ", "\
	PLACE_COLUMN_WIFI_APS ", "\
	PLACE_COLUMN_CREATE_DATE \
	" FROM " PLACE_TABLE

#define __GET_WIFI_APS_MAP_QUERY "SELECT "\
	WIFI_APS_MAP_COLUMN_MAC ", "\
	WIFI_APS_MAP_COLUMN_NETWORK_NAME \
	" FROM " WIFI_APS_MAP_TABLE

ctx::UserPlaces::UserPlaces(PlaceRecogMode energyMode):
	__visitDetector(nullptr),
	__timerId(-1)
{
	time_t now = std::time(nullptr);
	__visitDetector = new(std::nothrow) VisitDetector(now, energyMode);
	if (__visitDetector == nullptr) {
		_E("Cannot initialize __visitDetector");
		return;
	}

	__timerId = __timerManager.setAt( // execute once every night
			PLACES_DETECTOR_TASK_START_HOUR,
			PLACES_DETECTOR_TASK_START_MINUTE,
			DayOfWeek::EVERYDAY,
			this);
	if (__timerId < 0) {
		_E("timer set FAIL");
		return;
	} else {
		_D("timer set SUCCESS");
	}
}

ctx::UserPlaces::~UserPlaces()
{
	if (__timerId >= 0) {
		__timerManager.remove(__timerId);
	}
	if (__visitDetector)
		delete __visitDetector;
};

ctx::Json ctx::UserPlaces::getPlaces()
{
	std::vector<Json> records = __dbGetPlaces();
	std::map<std::string, std::string> wifiAPsMap = __dbGetWifiAPsMap();
	std::vector<std::shared_ptr<Place>> places = __placesFromJsons(records, wifiAPsMap);
	return UserPlaces::__composeJson(places);
}

ctx::Json ctx::UserPlaces::__composeJson(std::vector<std::shared_ptr<Place>> places)
{
	ctx::Json data;
	for (std::shared_ptr<ctx::Place> place : places) {
		ctx::Json placeJson;
		placeJson.set(NULL, PLACE_CATEG_ID, static_cast<int>(place->categId));
		placeJson.set(NULL, PLACE_CATEG_CONFIDENCE, static_cast<double>(place->categConfidence));
		placeJson.set(NULL, PLACE_NAME, place->name);
		if (place->locationValid) {
			ctx::Json locationJson;
			locationJson.set(NULL, PLACE_LOCATION_LATITUDE, static_cast<double>(place->location.latitude));
			locationJson.set(NULL, PLACE_LOCATION_LONGITUDE, static_cast<double>(place->location.longitude));
			locationJson.set(NULL, PLACE_LOCATION_ACCURACY, static_cast<double>(place->location.accuracy));
			placeJson.set(NULL, PLACE_LOCATION, locationJson);
		}
		if (place->wifiAps.size()) {
			ctx::Json wifiApsListJson;
			for (std::pair<std::string, std::string> ap : place->wifiAps) {
				ctx::Json wifiApJson;
				wifiApJson.set(NULL, PLACE_WIFI_AP_MAC, ap.first);
				wifiApJson.set(NULL, PLACE_WIFI_AP_NETWORK_NAME, ap.second);
				wifiApsListJson.append(NULL, PLACE_WIFI_APS, wifiApJson);
			}
			placeJson.set(NULL, PLACE_WIFI_APS, wifiApsListJson);
		}
		placeJson.set(NULL, PLACE_CREATE_DATE, static_cast<int64_t>(place->createDate));
		data.append(NULL, PLACE_DATA_READ, placeJson);
	}
	return data;
}

void ctx::UserPlaces::setMode(PlaceRecogMode energyMode)
{
	if (__visitDetector)
		__visitDetector->setMode(energyMode);
}

bool ctx::UserPlaces::onTimerExpired(int timerId)
{
	_D("mmastern try to detect places from UserPlaces");
	GModule *soHandle = g_module_open(SO_PATH, G_MODULE_BIND_LAZY);
	IF_FAIL_RETURN_TAG(soHandle, true, _E, "%s", g_module_error());

	gpointer symbol;
	if (!g_module_symbol(soHandle, "detectPlaces", &symbol) || symbol == NULL) {
		_E("mmastern %s", g_module_error());
		g_module_close(soHandle);
		return true;
	}

	places_detector_t detectPlaces = reinterpret_cast<places_detector_t>(symbol);

	detectPlaces();
	g_module_close(soHandle);
	return true;
}

std::vector<ctx::Json> ctx::UserPlaces::__dbGetPlaces()
{
	std::vector<Json> records;
	DatabaseManager dbManager;
	bool ret = dbManager.executeSync(__GET_PLACES_QUERY, &records);
	_D("load places execute query result: %s", ret ? "SUCCESS" : "FAIL");
	return records;
}

std::map<std::string, std::string> ctx::UserPlaces::__dbGetWifiAPsMap()
{
	std::vector<Json> records;
	std::map<std::string, std::string> wifiAPsMap;
	DatabaseManager dbManager;
	bool ret = dbManager.executeSync(__GET_WIFI_APS_MAP_QUERY, &records);
	// TODO: add return statements if db fail
	_D("load Wifi APs map (size = %d), result: %s", records.size(), ret ? "SUCCESS" : "FAIL");
	std::string mac = "";
	std::string networkName = "";
	for (Json record : records) {
		record.get(NULL, WIFI_APS_MAP_COLUMN_MAC, &mac);
		record.get(NULL, WIFI_APS_MAP_COLUMN_NETWORK_NAME, &networkName);
		wifiAPsMap.insert(std::pair<std::string, std::string>(mac, networkName));
	}
	return wifiAPsMap;
}

std::shared_ptr<ctx::Place> ctx::UserPlaces::__placeFromJson(Json &row, std::map<std::string, std::string> &wifiAPsMap)
{
	std::shared_ptr<Place> place = std::make_shared<Place>();
	__placeCategoryFromJson(row, *place);
	row.get(NULL, PLACE_COLUMN_NAME, &(place->name));
	__placeLocationFromJson(row, *place);
	__placeWifiAPsFromJson(row, wifiAPsMap, *place);
	__placeCreateDateFromJson(row, *place);
	_D("db_result: categId: %d; place: name: %s; locationValid: %d; latitude: %lf, longitude: %lf, createDate: %d", place->categId, place->name.c_str(), place->locationValid, place->location.latitude, place->location.longitude, place->createDate);
	return place;
}

void ctx::UserPlaces::__placeCategoryFromJson(Json &row, ctx::Place &place)
{
	int categId;
	row.get(NULL, PLACE_COLUMN_CATEG_ID, &categId);
	// This is due to the fact the JSON module API interface doesn't handle enum
	place.categId = static_cast<PlaceCategId>(categId);
}

void ctx::UserPlaces::__placeLocationFromJson(Json &row, ctx::Place &place)
{
	int locationValidInt;
	row.get(NULL, PLACE_COLUMN_LOCATION_VALID, &locationValidInt);
	place.locationValid = (bool) locationValidInt;
	row.get(NULL, PLACE_COLUMN_LOCATION_LATITUDE, &(place.location.latitude));
	row.get(NULL, PLACE_COLUMN_LOCATION_LONGITUDE, &(place.location.longitude));
	row.get(NULL, PLACE_COLUMN_LOCATION_ACCURACY, &(place.location.accuracy));
}

void ctx::UserPlaces::__placeWifiAPsFromJson(Json &row, std::map<std::string, std::string> &wifiAPsMap, ctx::Place &place)
{
	std::string wifiAps;
	row.get(NULL, PLACE_COLUMN_WIFI_APS, &wifiAps);
	std::stringstream ss;
	ss << wifiAps;
	std::shared_ptr<MacSet> macSet = std::make_shared<MacSet>();
	ss >> *macSet;
	for (ctx::Mac mac : *macSet) {
		place.wifiAps.insert(std::pair<std::string, std::string>(mac, wifiAPsMap[mac]));
	}
}

void ctx::UserPlaces::__placeCreateDateFromJson(Json &row, ctx::Place &place)
{
	int createDate;
	row.get(NULL, PLACE_COLUMN_CREATE_DATE, &(createDate));
	// This is due to the fact the JSON module API interface doesn't handle time_t
	place.createDate = static_cast<time_t>(createDate);
}

std::vector<std::shared_ptr<ctx::Place>> ctx::UserPlaces::__placesFromJsons(std::vector<Json>& records, std::map<std::string, std::string> &wifiAPsMap)
{
	std::vector<std::shared_ptr<Place>> places;
	_D("db_result: number of all places: %d", records.size());

	for (Json &row : records) {
		std::shared_ptr<Place> place = __placeFromJson(row, wifiAPsMap);
		places.push_back(place);
	}
	_D("number of all places in vector: %d", places.size());
	return places;
}
