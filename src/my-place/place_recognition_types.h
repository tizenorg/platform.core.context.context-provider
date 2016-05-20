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

#ifndef _CONTEXT_PLACE_RECOGNITION_TYPES_
#define _CONTEXT_PLACE_RECOGNITION_TYPES_

#include <ProviderTypes.h>

// Context Items
#define PLACE_SUBJ_RECOGNITION                 SUBJ_PLACE_DETECTION

#define PLACE_PRIV_RECOGNITION                 "location"

// Database
#define VISIT_TABLE                            "place_status_user_place_visit"
#define VISIT_COLUMN_START_TIME                "start_time"
#define VISIT_COLUMN_END_TIME                  "end_time"
#define VISIT_COLUMN_WIFI_APS                  "wifi_aps"
#define VISIT_COLUMN_CATEGORY                  "category"
#ifdef TIZEN_ENGINEER_MODE
#define VISIT_COLUMN_START_TIME_HUMAN          "start_time_human" // only for debug: human readable time data:
#define VISIT_COLUMN_END_TIME_HUMAN            "end_time_human" // only for debug: human readable time data:
#endif /* TIZEN_ENGINEER_MODE */
#define VISIT_COLUMN_LOCATION_VALID            "geo_valid"
#define VISIT_COLUMN_LOCATION_LATITUDE         "geo_latitude"
#define VISIT_COLUMN_LOCATION_LONGITUDE        "geo_longitude"
#define VISIT_COLUMN_CATEG_HOME                "categ_home"
#define VISIT_COLUMN_CATEG_WORK                "categ_work"
#define VISIT_COLUMN_CATEG_OTHER               "categ_other"

#define PLACE_TABLE                            "place_status_user_place"
#define PLACE_COLUMN_CATEG_ID                  "type_id" // Name inconsistency: "cated_id" vs "type_id" TODO make it consistent
#define PLACE_COLUMN_CATEG_CONFIDENCE          "type_confidence"
#define PLACE_COLUMN_NAME                      "name"
#define PLACE_COLUMN_LOCATION_VALID            "geo_valid"
#define PLACE_COLUMN_LOCATION_LATITUDE         "geo_latitude"
#define PLACE_COLUMN_LOCATION_LONGITUDE        "geo_longitude"
#define PLACE_COLUMN_WIFI_APS                  "wifi_aps"
#define PLACE_COLUMN_CREATE_DATE               "create_date"

#define WIFI_TABLE_NAME                        "place_status_user_place_wifi"
#define WIFI_COLUMN_TIMESTAMP                  "timestamp"
#define WIFI_COLUMN_BSSID                      "bssid"
#define WIFI_COLUMN_ESSID                      "essid"

#define LOCATION_TABLE_NAME                    "place_status_user_place_location"
#define LOCATION_COLUMN_LATITUDE               "geo_latitude"
#define LOCATION_COLUMN_LONGITUDE              "geo_longitude"
#define LOCATION_COLUMN_ACCURACY               "accuracy"
#define LOCATION_COLUMN_TIMESTAMP              "timestamp"
#ifdef TIZEN_ENGINEER_MODE
#define LOCATION_COLUMN_TIMESTAMP_HUMAN        "time_human" // only for debug: human readable time data:
#define LOCATION_COLUMN_METHOD                 "method"
#endif /* TIZEN_ENGINEER_MODE */

// Data Key
#define DATA_READ                              "PlacesList"
#define PLACE_CATEG_ID                         "TypeId" // Name inconsistency: "cated_id" vs "type_id" TODO make it consistent
#define PLACE_CATEG_CONFIDENCE                 "TypeConfidence"
#define PLACE_NAME                             "Name"
#define PLACE_GEO_LATITUDE                     "GeoLatitude"
#define PLACE_GEO_LONGITUDE                    "GeoLongitude"
#define PLACE_WIFI_APS                         "WifiAPs"
#define PLACE_CREATE_DATE                      "CreateDate"

// Data values
enum PlaceCategId {
	PLACE_CATEG_ID_NONE = 0,
	PLACE_CATEG_ID_HOME = 1,
	PLACE_CATEG_ID_WORK = 2,
	PLACE_CATEG_ID_OTHER = 3
};

enum PlaceRecogMode {
	PLACE_RECOG_HIGH_ACCURACY_MODE = 0,
	PLACE_RECOG_LOW_POWER_MODE = 1
};

#endif /* End of _CONTEXT_PLACE_RECOGNITION_TYPES_ */
