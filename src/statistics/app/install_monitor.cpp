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
#include <glib.h>
#include <Json.h>
#include <types_internal.h>
#include <db_mgr.h>
#include "app_stats_types.h"
#include "install_monitor.h"

static package_manager_event_type_e last_event_type;

ctx::app_install_monitor::app_install_monitor()
	: pkgmgr_h(NULL)
{
	start_monitoring();
}

ctx::app_install_monitor::~app_install_monitor()
{
	stop_monitoring();
}

bool ctx::app_install_monitor::start_monitoring()
{
	int err = package_manager_create(&pkgmgr_h);
	IF_FAIL_RETURN_TAG(err == PACKAGE_MANAGER_ERROR_NONE, false, _E, "package_manager_create() failed");

	err = package_manager_set_event_cb(pkgmgr_h, package_event_cb, this);
	IF_FAIL_RETURN_TAG(err == PACKAGE_MANAGER_ERROR_NONE, false, _E, "package_manager_set_event_cb() failed");

	return true;
}

void ctx::app_install_monitor::stop_monitoring()
{
	if(pkgmgr_h) {
		package_manager_unset_event_cb(pkgmgr_h);
		package_manager_destroy(pkgmgr_h);
		pkgmgr_h = NULL;
	}
}

void ctx::app_install_monitor::package_event_cb(const char *type, const char *package, package_manager_event_type_e event_type, package_manager_event_state_e event_state, int progress, package_manager_error_e error, void *user_data)
{
	IF_FAIL_VOID_TAG(error == PACKAGE_MANAGER_ERROR_NONE, _E, "package_manager error: %d", error);

	if (!(event_type == PACKAGE_MANAGER_EVENT_TYPE_INSTALL && event_state == PACKAGE_MANAGER_EVENT_STATE_COMPLETED) &&
		!(event_type == PACKAGE_MANAGER_EVENT_TYPE_UNINSTALL && event_state == PACKAGE_MANAGER_EVENT_STATE_STARTED)) {
		_D("Skipping event type-%d / state-%d", event_type, event_state);
		return;
	}

	package_info_h pkg_info;
	int err = package_manager_get_package_info(package, &pkg_info);
	IF_FAIL_VOID_TAG(err == PACKAGE_MANAGER_ERROR_NONE, _E, "package_manager_get_package_info() failed");

	last_event_type = event_type;

	err = package_info_foreach_app_from_package(pkg_info, PACKAGE_INFO_UIAPP, app_info_cb, user_data);
	if (err != PACKAGE_MANAGER_ERROR_NONE)
		_E("package_info_foreach_app_from_package() failed");

	package_info_destroy(pkg_info);
}

bool ctx::app_install_monitor::app_info_cb(package_info_app_component_type_e comp_type, const char *app_id, void *user_data)
{
	if (last_event_type == PACKAGE_MANAGER_EVENT_TYPE_INSTALL) {
		Json data;
		data.set(NULL, STATS_APP_ID, app_id);
		db_manager::insert(0, APP_TABLE_REMOVABLE_APP, data, NULL);
	} else if (last_event_type == PACKAGE_MANAGER_EVENT_TYPE_UNINSTALL) {
		db_manager::execute(0, create_deletion_query(APP_TABLE_REMOVABLE_APP, app_id).c_str(), NULL);
		db_manager::execute(0, create_deletion_query(APP_TABLE_USAGE_LOG, app_id).c_str(), NULL);
	}

	return true;
}

std::string ctx::app_install_monitor::create_deletion_query(const char* table_name, const char* app_id)
{
	std::stringstream query;
	query << "DELETE FROM " << table_name;
	query << " WHERE " STATS_APP_ID " = '" << app_id << "'";
	return query.str();
}
