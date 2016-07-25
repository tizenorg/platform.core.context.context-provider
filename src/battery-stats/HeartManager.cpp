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
#define HEART_DB_PATH tzplatform_mkpath(TZ_USER_DB, ".resourced-logging.db")
#endif

//#define HEART_DB_PATH "/opt/usr/dbspace/"
//#define HEART_CPU_DB_NAME "/opt/usr/dbspace/.resourced-logging.db"
#define CPU_TABLE_NAME "cpu"
//#define HEART_BATTERY_DB_NAME ".resourced-logging-level.db"

using namespace ctx;

HeartManager::HeartManager() 
{
	
	sqlite3 *db = NULL;
	char *err = NULL;
	int ret;
	_D("[kunal] HeartManager starting with path %s", HEART_DB_PATH);
	ret = sqlite3_open(HEART_DB_PATH, &db);
	if (ret != SQLITE_OK) {
		_D("ERROR: %s",sqlite3_errmsg(db));
	}
	ret = sqlite3_exec(db, "PRAGMA journal_mode = WAL", NULL, NULL, &err);
	if (ret != SQLITE_OK) {
		_E("Setting journal mode failed: %s", err);
		sqlite3_free(err);
	}

	__databaseHandle = db;
}

HeartManager::~HeartManager()
{
}

int HeartManager::__executionCb(void *userData, int dim, char **value, char **column)
{
	IF_FAIL_RETURN(userData, 0);

	std::vector<Json> *records = static_cast<std::vector<Json>*>(userData);
	Json row;
	bool columnNull = false;
	for (int i=0; i<dim; ++i) {
		if (!value[i]) {
			columnNull = true;
			_W(RED("Null columns exist"));
			break;
		}
		row.set(NULL, column[i], value[i]);
	}

	if (!columnNull) {
		records->push_back(row);
	}
	return 0;
}

bool HeartManager::getAppData(const char* query, std::vector<Json>* app_vector)
{
	char *err = NULL;
	int ret;
	ret = sqlite3_exec(__databaseHandle, query, __executionCb, app_vector, &err);
	
	if (ret != SQLITE_OK) {
		_E("DB Error: %s", err);
		sqlite3_free(err);
		return false;
	}
	_D("[kunal] Suceesfully opened DB");
	return true;
}
