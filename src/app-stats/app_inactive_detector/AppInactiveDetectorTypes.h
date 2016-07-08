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

#ifndef _CONTEXT_APP_INACTIVE_DETECTOR_TYPES_H_
#define _CONTEXT_APP_INACTIVE_DETECTOR_TYPES_H_

#include <string>

// Context Items
//#define APP_INACTIVE_SUBJ_RECALCULATE		"app/recalculate"
#define APP_INACTIVE_SUBJ_GET_APPS_INACTIVE		"app/inactive"
#define APP_INACTIVE_SUBJ_GET_APPS_ACTIVE		"app/active"
#define APP_INACTIVE_SUBJ_GET_APPS_WEIGHT		"app/weight"
#define APP_INACTIVE_SUBJ_UPDATE_CLUSTERS		"app/update"

enum {
		APP_INACTIVE_QUERY_ID_GET_APPS_WEIGHT = 1,
		APP_INACTIVE_QUERY_ID_GET_APPS_INACTIVE = 2,
		APP_INACTIVE_QUERY_ID_GET_APPS_ACTIVE = 3,
		APP_INACTIVE_QUERY_ID_UPDATE_CLUSTERS = 4
};

// Database
#define APP_INACTIVE_DETECTOR_APPINFO_TABLE						"context_app_info"
#define APP_INACTIVE_DETECTOR_APPINFO_TABLE_ALIAS				"app_info"
#define APP_INACTIVE_DETECTOR_APPINFO_COLUMN_ID					"_id"
#define APP_INACTIVE_DETECTOR_APPINFO_COLUMN_PACKAGE_NAME		"package_name"
#define APP_INACTIVE_DETECTOR_APPINFO_COLUMN_PACKAGE_TYPE		"package_type"
#define APP_INACTIVE_DETECTOR_APPINFO_COLUMN_IS_NODISPLAY		"is_nodisplay"
#define APP_INACTIVE_DETECTOR_APPINFO_COLUMN_IS_ENABLED			"is_enabled"
#define APP_INACTIVE_DETECTOR_APPINFO_COLUMN_IS_ATBOOT			"is_atboot"
#define APP_INACTIVE_DETECTOR_APPINFO_COLUMN_IS_PRELOADED		"is_preloaded"
#define APP_INACTIVE_DETECTOR_APPINFO_COLUMN_TIMESTAMP			"timestamp"

#define APP_INACTIVE_DETECTOR_APPLAUNCH_LOG_TABLE						"context_app_launch_log"
#define APP_INACTIVE_DETECTOR_APPLAUNCH_LOG_TABLE_ALIAS					"app_launch_log"
#define APP_INACTIVE_DETECTOR_APPLAUNCH_LOG_COLUMN_ID					"_id"
#define APP_INACTIVE_DETECTOR_APPLAUNCH_LOG_COLUMN_CONTEXT_APP_INFO_ID	"context_app_info_id"
#define APP_INACTIVE_DETECTOR_APPLAUNCH_LOG_COLUMN_DURATION				"duration"
#define APP_INACTIVE_DETECTOR_APPLAUNCH_LOG_COLUMN_SYSTEM_VOLUME		"system_volume"
#define APP_INACTIVE_DETECTOR_APPLAUNCH_LOG_COLUMN_BSSID				"bssid"
#define APP_INACTIVE_DETECTOR_APPLAUNCH_LOG_COLUMN_TIMESTAMP			"timestamp"
#define APP_INACTIVE_DETECTOR_APPLAUNCH_LOG_COLUMN_TIMESTAMP_UTC		"timestamp_utc"

#define APP_INACTIVE_DETECTOR_ACTIVITYCLASSIFIED_TABLE						"context_app_activity_classified"
#define APP_INACTIVE_DETECTOR_ACTIVITYCLASSIFIED_TABLE_ALIAS				"app_activity_classified"
#define APP_INACTIVE_DETECTOR_ACTIVITYCLASSIFIED_COLUMN_ID					"_id"
#define APP_INACTIVE_DETECTOR_ACTIVITYCLASSIFIED_COLUMN_CONTEXT_APP_INFO_ID	"context_app_info_id"
#define APP_INACTIVE_DETECTOR_ACTIVITYCLASSIFIED_COLUMN_ACTIVITY_RATE		"activity_rate"
#define APP_INACTIVE_DETECTOR_ACTIVITYCLASSIFIED_COLUMN_TIMEFRAME			"timeframe"
#define APP_INACTIVE_DETECTOR_ACTIVITYCLASSIFIED_COLUMN_IS_ACTIVE			"is_active"


#define APP_INACTIVE_DETECTOR_VIRTUAL_TABLE_ALIAS		"virtual_app_history"
#define APP_INACTIVE_DETECTOR_VIRTUAL_COLUMN_WEIGHT		"weight"

// Output Data Key
#define APP_INACTIVE_DETECTOR_DATA_READ							"AppsList"
#define APP_INACTIVE_DETECTOR_DATA_TIMESTAMP_FROM				"timestamp_from"
#define APP_INACTIVE_DETECTOR_ACTIVITY_RATE						"activity_rate"
#define APP_INACTIVE_DETECTOR_DATA_TIMEFRAME					"timeframe"
#define APP_INACTIVE_DETECTOR_DATA_ISACTIVE						"is_active"

//Other
#define APP_INACTIVE_DETECTOR_VALUE_PLACEHOLDER_TIMESTAMP	"$timestamp"
#define APP_INACTIVE_DETECTOR_VALUE_PLACEHOLDER_CLUSTER		"$cluster"
#define APP_INACTIVE_DETECTOR_VALUE_PLACEHOLDER_TIMEFRAME	"$timeframe"
#define APP_INACTIVE_DETECTOR_VALUE_PLACEHOLDER_VALUES		"$values"

// Objects
struct AppInfo {
	int id;
	std::string packageName;
	int isNodisplay;
	int isEnabled;
	int isAtBoot;
	int isPreloaded;
	double timestamp;
	int weight;
	int isActive;
	int timeframe;
};


#define APP_TYPE_INACTIVE_INACTIVE	1
#define APP_TYPE_INACTIVE_ACTIVE	2


#define APP_INACTIVE_TASK_START_HOUR	3
#define APP_INACTIVE_TASK_START_MINUTE	0


#define CONTEXT_HISTORY_FILTER_TIME_INTERVAL_ONEDAY		60*60*24
#define CONTEXT_HISTORY_FILTER_TIME_INTERVAL_THREEDAYS	60*60*24*3
#define CONTEXT_HISTORY_FILTER_TIME_INTERVAL_ONEWEEK	60*60*24*7
#define CONTEXT_HISTORY_FILTER_TIME_INTERVAL_TWOWEEKS	60*60*24*14
#define CONTEXT_HISTORY_FILTER_TIME_INTERVAL_ONEMONTH	60*60*24*31


#endif /* _CONTEXT_APP_INACTIVE_DETECTOR_TYPES_H_ */
