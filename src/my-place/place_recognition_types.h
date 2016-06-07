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
#define VISIT_COLUMN_LOCATION_VALID            "location_valid"
#define VISIT_COLUMN_LOCATION_LATITUDE         "location_latitude"
#define VISIT_COLUMN_LOCATION_LONGITUDE        "location_longitude"
#define VISIT_COLUMN_LOCATION_ACCURACY         "location_accuracy"
#define VISIT_COLUMN_CATEG_HOME                "categ_home"
#define VISIT_COLUMN_CATEG_WORK                "categ_work"
#define VISIT_COLUMN_CATEG_OTHER               "categ_other"

#define WIFI_APS_MAP_TABLE                     "place_status_user_place_wifi_aps_map"
#define WIFI_APS_MAP_COLUMN_MAC                "mac"
#define WIFI_APS_MAP_COLUMN_NETWORK_NAME       "network_name"
#define WIFI_APS_MAP_COLUMN_INSERT_TIME        "insert_time"

#define PLACE_TABLE                            "place_status_user_place"
#define PLACE_COLUMN_CATEG_ID                  "categ_id"
#define PLACE_COLUMN_CATEG_CONFIDENCE          "categ_confidence"
#define PLACE_COLUMN_NAME                      "name"
#define PLACE_COLUMN_LOCATION_VALID            "location_valid"
#define PLACE_COLUMN_LOCATION_LATITUDE         "location_latitude"
#define PLACE_COLUMN_LOCATION_LONGITUDE        "location_longitude"
#define PLACE_COLUMN_LOCATION_ACCURACY         "location_accuracy"
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

enum PlaceRecogMode {
	PLACE_RECOG_HIGH_ACCURACY_MODE = 0,
	PLACE_RECOG_LOW_POWER_MODE = 1
};

#endif /* End of _CONTEXT_PLACE_RECOGNITION_TYPES_ */
