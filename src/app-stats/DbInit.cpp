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
#include <Json.h>
#include <Types.h>
#include "AppStatisticsTypes.h"
#include "DbInit.h"

#define EMPTY_CHECKER_QID 999

ctx::AppDbInitializer::AppDbInitializer()
{
	__createTable();
	__checkAppList();
}

ctx::AppDbInitializer::~AppDbInitializer()
{
}

void ctx::AppDbInitializer::__createTable()
{
	static bool done = false;
	IF_FAIL_VOID(!done);

	__dbManager.createTable(0, APP_TABLE_USAGE_LOG, APP_TABLE_USAGE_LOG_COLUMNS, NULL, NULL);
	__dbManager.createTable(0, APP_TABLE_REMOVABLE_APP, APP_TABLE_REMOVABLE_APP_COLUMNS, NULL, NULL);
	__dbManager.execute(0, APP_TEMP_USAGE_FREQ_SQL, NULL);

	done = true;
}

void ctx::AppDbInitializer::__checkAppList()
{
	__dbManager.execute(EMPTY_CHECKER_QID, "SELECT * FROM " APP_TABLE_REMOVABLE_APP " LIMIT 1", this);
}

void ctx::AppDbInitializer::__duplicateAppList()
{
	int err;
	package_manager_filter_h filter;

	err = package_manager_filter_create(&filter);
	IF_FAIL_VOID_TAG(err == PACKAGE_MANAGER_ERROR_NONE, _E, "package_manager_filter_create() failed");

	err = package_manager_filter_add_bool(filter, PACKAGE_MANAGER_PKGINFO_PROP_REMOVABLE, true);
	IF_FAIL_CATCH_TAG(err == PACKAGE_MANAGER_ERROR_NONE, _E, "package_manager_filter_add_bool() failed");

	err = package_manager_filter_foreach_package_info(filter, __packageInfoCb, this);
	IF_FAIL_CATCH_TAG(err == PACKAGE_MANAGER_ERROR_NONE, _E, "package_manager_filter_foreach_package_info() failed");

CATCH:
	if (filter)
		package_manager_filter_destroy(filter);
}

bool ctx::AppDbInitializer::__packageInfoCb(package_info_h packageInfo, void *userData)
{
	int err = package_info_foreach_app_from_package(packageInfo, PACKAGE_INFO_UIAPP, __appInfoCb, userData);
	IF_FAIL_RETURN_TAG(err == PACKAGE_MANAGER_ERROR_NONE, false, _E, "package_info_foreach_app_from_package() failed");
	return true;
}

bool ctx::AppDbInitializer::__appInfoCb(package_info_app_component_type_e compType, const char *appId, void *userData)
{
	Json data;
	DatabaseManager dbManager;

	data.set(NULL, STATS_APP_ID, appId);
	return dbManager.insert(0, APP_TABLE_REMOVABLE_APP, data, NULL);
}

void ctx::AppDbInitializer::onTableCreated(unsigned int queryId, int error)
{
}

void ctx::AppDbInitializer::onInserted(unsigned int queryId, int error, int64_t rowId)
{
}

void ctx::AppDbInitializer::onExecuted(unsigned int queryId, int error, std::vector<Json>& records)
{
	if (queryId != EMPTY_CHECKER_QID) {
		_E("Unknown Query ID: %d", queryId);
		delete this;
		return;
	}

	if (records.empty())
		__duplicateAppList();

	delete this;
}
