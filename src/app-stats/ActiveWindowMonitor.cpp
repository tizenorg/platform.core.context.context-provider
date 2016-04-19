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

#include <sys/types.h>
#include <time.h>
#include <app_manager.h>

#include <Json.h>
#include <Types.h>
#include "../shared/SystemInfo.h"
#include "AppStatisticsTypes.h"
#include "ActiveWindowMonitor.h"

/* Active window changes frequently.
 * We thus consider the apps being foregrounded at least 3 secs */
#define ONE_DAY_IN_SEC 86400

ctx::AppUseMonitor::AppUseMonitor() :
	__signalId(-1),
	__lastCleanupTime(0),
	__dbusWatcher(DBusType::SYSTEM)
{
	__startLogging();
}

ctx::AppUseMonitor::~AppUseMonitor()
{
	__stopLogging();
}

bool ctx::AppUseMonitor::__startLogging()
{
	__signalId = __dbusWatcher.watch(NULL, "/Org/Tizen/Aul/AppStatus", "org.tizen.aul.AppStatus", "AppStatusChange", this);
	_D("Active window monitoring started (%lld)", __signalId);
	return (__signalId > 0);
}

void ctx::AppUseMonitor::__stopLogging()
{
	if (__signalId > 0) {
		__dbusWatcher.unwatch(__signalId);
		_D("Active window monitoring stopped");
	}
}

void ctx::AppUseMonitor::onSignal(const char* sender, const char* path, const char* iface, const char* name, GVariant* param)
{
	gint pid = 0;
	const gchar *appId = NULL;
	const gchar *pkgId = NULL;
	const gchar *status = NULL;
	const gchar *type = NULL;

	g_variant_get(param, "(i&s&s&s&s)", &pid, &appId, &pkgId, &status, &type);
	_D("AppEvent: %s, %s, %s", appId, status, type);

	IF_FAIL_VOID(appId && status && type);
	IF_FAIL_VOID(STR_EQ(type, "uiapp") && !__isSkippable(appId));

	if (STR_EQ(status, "fg")) {
		__createRecord(appId);
	} else if (STR_EQ(status, "bg")) {
		__finishRecord(appId);
		__removeExpired();
	}
}

void ctx::AppUseMonitor::__createRecord(std::string appId)
{
	int audioJack;
	int systemVolume;
	int mediaVolume;
	std::string bssid;
	Json data;
	data.set(NULL, STATS_APP_ID, appId);

	if (ctx::system_info::getAudioJackState(&audioJack))
		data.set(NULL, STATS_AUDIO_JACK, audioJack);

	if (ctx::system_info::getVolume(&systemVolume, &mediaVolume)) {
		data.set(NULL, STATS_SYSTEM_VOLUME, systemVolume);
		data.set(NULL, STATS_MEDIA_VOLUME, mediaVolume);
	}

	if (ctx::system_info::getWifiBssid(bssid))
		data.set(NULL, STATS_BSSID, bssid);

	__dbManager.insert(0, APP_TABLE_USAGE_LOG, data, NULL);
}

void ctx::AppUseMonitor::__finishRecord(std::string appId)
{
	/* TODO: It might be necessary to update system status here */
	std::stringstream query;
	query <<
		"UPDATE " APP_TABLE_USAGE_LOG \
		" SET " STATS_DURATION " = strftime('%s', 'now') - " STATS_UNIV_TIME \
		" WHERE " STATS_COL_ROW_ID " = (" \
			"SELECT MAX(" STATS_COL_ROW_ID ") FROM " APP_TABLE_USAGE_LOG \
			" WHERE " STATS_APP_ID " = '" << appId << "'" \
			" AND " STATS_DURATION " = 0)";
	__dbManager.execute(0, query.str().c_str(), NULL);
}

bool ctx::AppUseMonitor::__isSkippable(std::string appId)
{
	/* TODO: circular cache */
	app_info_h appInfo = NULL;
	int err = app_manager_get_app_info(appId.c_str(), &appInfo);
	IF_FAIL_RETURN_TAG(err == APP_MANAGER_ERROR_NONE && appInfo, true, _E, "app_manager_get_app_info() failed");

	bool nodisp = false;
	err = app_info_is_nodisplay(appInfo, &nodisp);
	if (err != APP_MANAGER_ERROR_NONE) {
		app_info_destroy(appInfo);
		_E("app_info_is_nodisplay() failed");
		return true;
	}

	app_info_destroy(appInfo);
	return nodisp;
}

void ctx::AppUseMonitor::__removeExpired()
{
	int timestamp = static_cast<int>(time(NULL));
	IF_FAIL_VOID(timestamp - __lastCleanupTime >= ONE_DAY_IN_SEC);

	__lastCleanupTime = timestamp;

	std::stringstream query;
	query << "DELETE FROM " APP_TABLE_USAGE_LOG " WHERE " \
		STATS_UNIV_TIME " < strftime('%s', 'now') - " << LOG_RETENTION_PERIOD;
	__dbManager.execute(0, query.str().c_str(), NULL);
}
