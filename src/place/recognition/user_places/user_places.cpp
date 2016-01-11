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

#include <ctime>
#include <memory>
#include <types_internal.h>
#include "user_places.h"
#include "places_detector.h"
#include "timer_mgr.h"
#include "../place_recognition_types.h"
#include "db_mgr.h"

ctx::UserPlaces::UserPlaces(place_recog_mode_e energy_mode)
	: visit_detector(nullptr)
	, places_detector(nullptr)
	, places_detector_timer_id(-1)
{
	time_t now = std::time(nullptr);
	visit_detector = new(std::nothrow) VisitDetector(now, energy_mode);
	if (visit_detector == nullptr) {
		_E("Cannot initialize visit_detector");
		return;
	}

	places_detector = new(std::nothrow) PlacesDetector();
	if (places_detector == nullptr) {
		_E("Cannot initialize places_detector");
		return;
	}

	places_detector_timer_id = timer_manager::set_at( // execute once every night
			PLACES_DETECTOR_TASK_START_HOUR,
			PLACES_DETECTOR_TASK_START_MINUTE,
			timer_types::EVERYDAY,
			places_detector);
	if (places_detector_timer_id < 0) {
		_E("PlacesDetector timer set FAIL");
		return;
	} else {
		_D("PlacesDetector timer set SUCCESS");
	}
}

ctx::UserPlaces::~UserPlaces()
{
	if (places_detector_timer_id >= 0) {
		timer_manager::remove(places_detector_timer_id);
		_D("PlacesDetector timer removed");
	}

	if (visit_detector) {
		delete visit_detector;
	}

	if (places_detector) {
		delete places_detector;
	}
};

std::vector<std::shared_ptr<ctx::Place>> ctx::UserPlaces::get_places()
{
	if (places_detector) {
		return places_detector->get_places();
	} else {
		return std::vector<std::shared_ptr<ctx::Place>>();
	}
}

/*
 * Example JSON output:
 * ------------------------------------------------
 * {
 *	  "PlacesList": [
 *		{
 *		  "TypeId": 2,
 *		  "Name": "Work",
 *		  "GeoLatitude": 10.94433,
 *		  "GeoLongitude": 50.85504,
 *		  "WifiAPs": "00:1f:f3:5b:2b:1f,15:34:56:78:9a:ba,13:34:56:78:9a:ba",
 *		  "CreateDate": 12132567
 *		},
 *		{
 *		  "TypeId": 1,
 *		  "Name": "Home",
 *		  "GeoLatitude": 10.96233,
 *		  "GeoLongitude": 50.84304,
 *		  "WifiAPs": "aa:bb:cc:dd:ee:ff,10:34:56:78:9a:bc",
 *		  "CreateDate": 12132889
 *		},
 *		{
 *		  "TypeId": 3,
 *		  "Name": "Other",
 *		  "GeoLatitude": 10.96553,
 *		  "GeoLongitude": 50.80404,
 *		  "WifiAPs": "12:34:56:78:9a:ba",
 *		  "CreateDate": 12132346
 *		}
 *	  ]
 * }
 */
ctx::json ctx::UserPlaces::compose_json(std::vector<std::shared_ptr<Place>> places)
{
	ctx::json data;
	for (std::shared_ptr<ctx::Place> place : places) {
		ctx::json place_j;
		place_j.set(NULL, PLACE_CATEG_ID, place->categ_id);
		place_j.set(NULL, PLACE_CATEG_CONFIDENCE, place->categ_confidence);
		place_j.set(NULL, PLACE_NAME, place->name);
		if (place->location_valid) {
			place_j.set(NULL, PLACE_GEO_LATITUDE, place->location.latitude, GEO_LOCATION_PRECISION);
			place_j.set(NULL, PLACE_GEO_LONGITUDE, place->location.longitude, GEO_LOCATION_PRECISION);
		}
		place_j.set(NULL, PLACE_WIFI_APS, place->wifi_aps);
		place_j.set(NULL, PLACE_CREATE_DATE, static_cast<int>(place->create_date));
		data.array_append(NULL, DATA_READ, place_j);
	}
	return data;
}

void ctx::UserPlaces::set_mode(place_recog_mode_e energy_mode)
{
	if (visit_detector) {
		visit_detector->set_mode(energy_mode);
	}
}
