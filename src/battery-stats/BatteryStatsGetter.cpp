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

#include "BatteryStatsGetter.h"
#include "HeartManager.h"
#include <sstream>
#include <list>
#include <Json.h>
#include <glib.h>
#define CPU_TABLE_NAME "cpu"

using namespace ctx;

BatteryStatsGetter::BatteryStatsGetter()
{
}

BatteryStatsGetter::~BatteryStatsGetter()
{
}

vec BatteryStatsGetter::getAppTimes(time_t start_time, time_t end_time)
{
	HeartManager *__heartMgr = new (std::nothrow) HeartManager();
	std::vector<Json> app_vector;

	std::map<std::string,int> app_table;
	const char* query = make_query(start_time, end_time);
	int utime,stime;
	GVariant* app;
	GVariant* temp;
	const char* data = "data";
	const char* appid = "appid";
	const char* path_json = NULL;
	std::list<std::string> *listed;
	__heartMgr->getAppData(query, &app_vector);
	_D("[kunal] Got appvector with size %d", app_vector.size());
	for (int i=0;i<app_vector.size();i++) {
		app_vector[i].get(path_json,appid, &app);
		app_vector[i].get(path_json,data, &temp);
		if (!app_vector[i].getKeys(listed)) {
			_D("[kunal] Something is wrong");
		}
		else {
			_D("[kunal] the list size is %d", listed->size());
		}
		// for (auto iterator = listed->begin(); iterator != listed->end(); ++iterator) {
		// 	_D("[kunal] next key is %s ", iterator->c_str());
		// }
		// _D("[kunal] acid test");
		// _D("[kunal] the list size is %d", listed->size());
		_D("[kunal] The appid is %s and the value is %s, with Json being %s", (char*)(app), (char*)(temp), app_vector[i].str().c_str());
		utime =0;
		stime =0;
		std::string app2((char*)app);
		if(app_table.find(app2) == app_table.end()) {
		    app_table[app2] = utime + stime;
		}
		else {
			int prev_value = app_table[app2];
			app_table[app2] = prev_value + utime + stime;
		}
	}
	vec app_table_vector;
	for (auto iterator = app_table.begin(); iterator != app_table.end(); ++iterator) {
		app_table_vector.push_back(std::make_pair(iterator->first,iterator->second));
	}
	return app_table_vector;
}

const char* BatteryStatsGetter::make_query(time_t start_time, time_t end_time) {
	std::stringstream query;
	query << "SELECT * FROM " CPU_TABLE_NAME;
	return query.str().c_str();
}

vec BatteryStatsGetter::calculatePercentage (time_t start_time, time_t end_time)
{
	vec app_battery_use = getAppTimes(start_time,end_time);
	int temp;
	for (auto iterator = app_battery_use.begin(); iterator != app_battery_use.end(); ++iterator) {
		temp += iterator->second;
	}
	for (auto iterator = app_battery_use.begin(); iterator != app_battery_use.end(); ++iterator) {
		float float_percent = 100*(float)iterator->second/(float)temp;
		int percent = int(0.5 + float_percent);
		app_battery_use.erase(iterator);
		app_battery_use.push_back(std::make_pair(iterator->first,percent));
	}
	return app_battery_use;
}