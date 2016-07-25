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

#include "BatteryStatsProvider.h"
#include "HeartManager.h"
#include <string.h>
#include <sstream>
#include <vector>
#include <Json.h>

//#define HEART_DB_PATH "/opt/usr/dbspace/.resourced-logging.db"

#ifdef LEGACY_FILE_PATH
#define HEART_DB_PATH "/opt/usr/dbspace/.resourced-logging.db"
#else
#include <tzplatform_config.h>
#define HEART_DB_PATH tzplatform_mkpath(TZ_USER_DB, ".calendar-svc.db")
#endif

//#define HEART_DB_PATH "/opt/usr/dbspace/"
//#define HEART_CPU_DB_NAME "/opt/usr/dbspace/.resourced-logging.db"
#define CPU_TABLE_NAME "event_view"
//#define HEART_BATTERY_DB_NAME ".resourced-logging-level.db"

using namespace ctx;

HeartManager::HeartManager() 
{

	__dbManager = DatabaseManager((char*)HEART_DB_PATH);
	_D("[kunal] HeartManager started with path %s", HEART_DB_PATH);
}

HeartManager::~HeartManager()
{
}


int HeartManager::getAppData(QueryData **app_data, time_t begin_time, time_t end_time)
{
	std::stringstream query;
	std::vector<Json> app_vector;

	query << "SELECT * FROM " CPU_TABLE_NAME;
	
	int error = __dbManager.executeSync( query.str().c_str(), &app_vector);

	_D("[kunal] Error is %d ", error);

	return error;
}
