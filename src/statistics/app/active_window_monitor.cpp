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

#include <db_mgr.h>
#include <Json.h>
#include <types_internal.h>
#include "../shared/system_info.h"
#include "app_stats_types.h"
#include "active_window_monitor.h"

/* Active window changes frequently.
 * We thus consider the apps being foregrounded at least 3 secs */
#define ONE_DAY_IN_SEC 86400

ctx::app_use_monitor::app_use_monitor()
	: signal_id(-1)
	, last_cleanup_time(0)
	, __dbusWatcher(DBusType::SYSTEM)
{
	start_logging();
}

ctx::app_use_monitor::~app_use_monitor()
{
	stop_logging();
}

bool ctx::app_use_monitor::start_logging()
{
	signal_id = __dbusWatcher.watch(NULL, "/Org/Tizen/Aul/AppStatus", "org.tizen.aul.AppStatus", "AppStatusChange", this);
	_D("Active window monitoring started (%lld)", signal_id);
	return (signal_id > 0);
}

void ctx::app_use_monitor::stop_logging()
{
	if (signal_id > 0) {
		__dbusWatcher.unwatch(signal_id);
		_D("Active window monitoring stopped");
	}
}

void ctx::app_use_monitor::onSignal(const char* sender, const char* path, const char* iface, const char* name, GVariant* param)
{
	gint pid = 0;
	const gchar *appid = NULL;
	const gchar *pkgid = NULL;
	const gchar *status = NULL;
	const gchar *type = NULL;

	g_variant_get(param, "(i&s&s&s&s)", &pid, &appid, &pkgid, &status, &type);
	_D("AppEvent: %s, %s, %s", appid, status, type);

	IF_FAIL_VOID(appid && status && type);
	IF_FAIL_VOID(STR_EQ(type, "uiapp") && !is_skippable(appid));

	if (STR_EQ(status, "fg")) {
		create_record(appid);
	} else if (STR_EQ(status, "bg")) {
		finish_record(appid);
		remove_expired();
	}
}

void ctx::app_use_monitor::create_record(std::string app_id)
{
	int audiojack;
	int system_volume;
	int media_volume;
	std::string bssid;
	Json data;
	data.set(NULL, STATS_APP_ID, app_id);

	if (ctx::system_info::get_audio_jack_state(&audiojack))
		data.set(NULL, STATS_AUDIO_JACK, audiojack);

	if (ctx::system_info::get_volume(&system_volume, &media_volume)) {
		data.set(NULL, STATS_SYSTEM_VOLUME, system_volume);
		data.set(NULL, STATS_MEDIA_VOLUME, media_volume);
	}

	if (ctx::system_info::get_wifi_bssid(bssid))
		data.set(NULL, STATS_BSSID, bssid);

	db_manager::insert(0, APP_TABLE_USAGE_LOG, data, NULL);
}

void ctx::app_use_monitor::finish_record(std::string app_id)
{
	/* TODO: It might be necessary to update system status here */
	std::stringstream query;
	query <<
		"UPDATE " APP_TABLE_USAGE_LOG \
		" SET " STATS_DURATION " = strftime('%s', 'now') - " STATS_UNIV_TIME \
		" WHERE " STATS_COL_ROW_ID " = (" \
			"SELECT MAX(" STATS_COL_ROW_ID ") FROM " APP_TABLE_USAGE_LOG \
			" WHERE " STATS_APP_ID " = '" << app_id << "'" \
			" AND " STATS_DURATION " = 0)";
	db_manager::execute(0, query.str().c_str(), NULL);
}

bool ctx::app_use_monitor::is_skippable(std::string app_id)
{
	/* TODO: circular cache */
	app_info_h app_info = NULL;
	int err = app_manager_get_app_info(app_id.c_str(), &app_info);
	IF_FAIL_RETURN_TAG(err == APP_MANAGER_ERROR_NONE && app_info, true, _E, "app_manager_get_app_info() failed");

	bool nodisp = false;
	err = app_info_is_nodisplay(app_info, &nodisp);
	if (err != APP_MANAGER_ERROR_NONE) {
		app_info_destroy(app_info);
		_E("app_info_is_nodisplay() failed");
		return true;
	}

	app_info_destroy(app_info);
	return nodisp;
}

void ctx::app_use_monitor::remove_expired()
{
	int timestamp = static_cast<int>(time(NULL));
	IF_FAIL_VOID(timestamp - last_cleanup_time >= ONE_DAY_IN_SEC);

	last_cleanup_time = timestamp;

	std::stringstream query;
	query << "DELETE FROM " APP_TABLE_USAGE_LOG " WHERE " \
		STATS_UNIV_TIME " < strftime('%s', 'now') - " << LOG_RETENTION_PERIOD;
	db_manager::execute(0, query.str().c_str(), NULL);
}
