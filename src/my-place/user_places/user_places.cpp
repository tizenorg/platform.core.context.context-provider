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
#include <Types.h>
#include "user_places.h"
#include "places_detector.h"
#include <MyPlaceTypes.h>

ctx::UserPlaces::UserPlaces(PlaceRecogMode energyMode):
	__visitDetector(nullptr),
	__placesDetector(nullptr),
	__placesDetectorTimerId(-1)
{
	time_t now = std::time(nullptr);
	__visitDetector = new(std::nothrow) VisitDetector(now, energyMode);
	if (__visitDetector == nullptr) {
		_E("Cannot initialize __visitDetector");
		return;
	}

	__placesDetector = new(std::nothrow) PlacesDetector();
	if (__placesDetector == nullptr) {
		_E("Cannot initialize __placesDetector");
		return;
	}

	__placesDetectorTimerId = __timerManager.setAt( // execute once every night
			PLACES_DETECTOR_TASK_START_HOUR,
			PLACES_DETECTOR_TASK_START_MINUTE,
			DayOfWeek::EVERYDAY,
			__placesDetector);
	if (__placesDetectorTimerId < 0) {
		_E("PlacesDetector timer set FAIL");
		return;
	} else {
		_D("PlacesDetector timer set SUCCESS");
	}
}

ctx::UserPlaces::~UserPlaces()
{
	if (__placesDetectorTimerId >= 0) {
		__timerManager.remove(__placesDetectorTimerId);
		_D("PlacesDetector timer removed");
	}

	if (__visitDetector)
		delete __visitDetector;
	if (__placesDetector)
		delete __placesDetector;
};

std::vector<std::shared_ptr<ctx::Place>> ctx::UserPlaces::__getPlaces()
{
	if (__placesDetector) {
		return __placesDetector->getPlaces();
	} else {
		return std::vector<std::shared_ptr<ctx::Place>>();
	}
}

ctx::Json ctx::UserPlaces::getPlaces()
{
	std::vector<std::shared_ptr<ctx::Place>> places = __getPlaces();
	Json dataRead = UserPlaces::__composeJson(places);
	return dataRead;
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
