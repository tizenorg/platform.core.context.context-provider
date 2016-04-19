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
#include <Types.h>
#include <DatabaseManager.h>
#include "AppStatisticsTypes.h"
#include "InstallMonitor.h"

static package_manager_event_type_e __lastEventType;

ctx::AppInstallMonitor::AppInstallMonitor() :
	__pkgmgrHandle(NULL)
{
	__startMonitoring();
}

ctx::AppInstallMonitor::~AppInstallMonitor()
{
	__stopMonitoring();
}

bool ctx::AppInstallMonitor::__startMonitoring()
{
	int err = package_manager_create(&__pkgmgrHandle);
	IF_FAIL_RETURN_TAG(err == PACKAGE_MANAGER_ERROR_NONE, false, _E, "package_manager_create() failed");

	err = package_manager_set_event_cb(__pkgmgrHandle, __packageEventCb, this);
	IF_FAIL_RETURN_TAG(err == PACKAGE_MANAGER_ERROR_NONE, false, _E, "package_manager_set_event_cb() failed");

	return true;
}

void ctx::AppInstallMonitor::__stopMonitoring()
{
	if(__pkgmgrHandle) {
		package_manager_unset_event_cb(__pkgmgrHandle);
		package_manager_destroy(__pkgmgrHandle);
		__pkgmgrHandle = NULL;
	}
}

void ctx::AppInstallMonitor::__packageEventCb(const char *type, const char *package, package_manager_event_type_e eventType, package_manager_event_state_e eventState, int progress, package_manager_error_e error, void *userData)
{
	IF_FAIL_VOID_TAG(error == PACKAGE_MANAGER_ERROR_NONE, _E, "package_manager error: %d", error);

	if (!(eventType == PACKAGE_MANAGER_EVENT_TYPE_INSTALL && eventState == PACKAGE_MANAGER_EVENT_STATE_COMPLETED) &&
		!(eventType == PACKAGE_MANAGER_EVENT_TYPE_UNINSTALL && eventState == PACKAGE_MANAGER_EVENT_STATE_STARTED)) {
		_D("Skipping event type-%d / state-%d", eventType, eventState);
		return;
	}

	package_info_h pkgInfo;
	int err = package_manager_get_package_info(package, &pkgInfo);
	IF_FAIL_VOID_TAG(err == PACKAGE_MANAGER_ERROR_NONE, _E, "package_manager_get_package_info() failed");

	__lastEventType = eventType;

	err = package_info_foreach_app_from_package(pkgInfo, PACKAGE_INFO_UIAPP, __appInfoCb, userData);
	if (err != PACKAGE_MANAGER_ERROR_NONE)
		_E("package_info_foreach_app_from_package() failed");

	package_info_destroy(pkgInfo);
}

bool ctx::AppInstallMonitor::__appInfoCb(package_info_app_component_type_e compType, const char *appId, void *userData)
{
	DatabaseManager dbManager;

	if (__lastEventType == PACKAGE_MANAGER_EVENT_TYPE_INSTALL) {
		Json data;
		data.set(NULL, STATS_APP_ID, appId);
		dbManager.insert(0, APP_TABLE_REMOVABLE_APP, data, NULL);
	} else if (__lastEventType == PACKAGE_MANAGER_EVENT_TYPE_UNINSTALL) {
		dbManager.execute(0, __createDeletionQuery(APP_TABLE_REMOVABLE_APP, appId).c_str(), NULL);
		dbManager.execute(0, __createDeletionQuery(APP_TABLE_USAGE_LOG, appId).c_str(), NULL);
	}

	return true;
}

std::string ctx::AppInstallMonitor::__createDeletionQuery(const char* tableName, const char* appId)
{
	std::stringstream query;
	query << "DELETE FROM " << tableName;
	query << " WHERE " STATS_APP_ID " = '" << appId << "'";
	return query.str();
}
