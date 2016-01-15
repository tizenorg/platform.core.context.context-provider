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
#include <json.h>
#include <types_internal.h>
#include <db_mgr.h>
#include "app_stats_types.h"
#include "db_init.h"

#define EMPTY_CHECKER_QID 999

ctx::app_db_initializer::app_db_initializer()
{
	create_table();
	check_app_list();
}

ctx::app_db_initializer::~app_db_initializer()
{
}

void ctx::app_db_initializer::create_table()
{
	static bool done = false;
	IF_FAIL_VOID(!done);

	db_manager::create_table(0, APP_TABLE_USAGE_LOG, APP_TABLE_USAGE_LOG_COLUMNS, NULL, NULL);
	db_manager::create_table(0, APP_TABLE_REMOVABLE_APP, APP_TABLE_REMOVABLE_APP_COLUMNS, NULL, NULL);
	db_manager::execute(0, APP_TEMP_USAGE_FREQ_SQL, NULL);

	done = true;
}

void ctx::app_db_initializer::check_app_list()
{
	db_manager::execute(EMPTY_CHECKER_QID, "SELECT * FROM " APP_TABLE_REMOVABLE_APP " LIMIT 1", this);
}

void ctx::app_db_initializer::duplicate_app_list()
{
	int err;
	package_manager_filter_h filter;

	err = package_manager_filter_create(&filter);
	IF_FAIL_VOID_TAG(err == PACKAGE_MANAGER_ERROR_NONE, _E, "package_manager_filter_create() failed");

	err = package_manager_filter_add_bool(filter, PACKAGE_MANAGER_PKGINFO_PROP_REMOVABLE, true);
	IF_FAIL_CATCH_TAG(err == PACKAGE_MANAGER_ERROR_NONE, _E, "package_manager_filter_add_bool() failed");

	err = package_manager_filter_foreach_package_info(filter, package_info_cb, this);
	IF_FAIL_CATCH_TAG(err == PACKAGE_MANAGER_ERROR_NONE, _E, "package_manager_filter_foreach_package_info() failed");

CATCH:
	if (filter)
		package_manager_filter_destroy(filter);
}

bool ctx::app_db_initializer::package_info_cb(package_info_h package_info, void *user_data)
{
	int err = package_info_foreach_app_from_package(package_info, PACKAGE_INFO_UIAPP, app_info_cb, user_data);
	IF_FAIL_RETURN_TAG(err == PACKAGE_MANAGER_ERROR_NONE, false, _E, "package_info_foreach_app_from_package() failed");
	return true;
}

bool ctx::app_db_initializer::app_info_cb(package_info_app_component_type_e comp_type, const char *app_id, void *user_data)
{
	json data;
	data.set(NULL, STATS_APP_ID, app_id);
	return db_manager::insert(0, APP_TABLE_REMOVABLE_APP, data, NULL);
}

void ctx::app_db_initializer::on_creation_result_received(unsigned int query_id, int error)
{
}

void ctx::app_db_initializer::on_insertion_result_received(unsigned int query_id, int error, int64_t row_id)
{
}

void ctx::app_db_initializer::on_query_result_received(unsigned int query_id, int error, std::vector<json>& records)
{
	if (query_id != EMPTY_CHECKER_QID) {
		_E("Unknown Query ID: %d", query_id);
		delete this;
		return;
	}

	if (records.empty())
		duplicate_app_list();

	delete this;
}
