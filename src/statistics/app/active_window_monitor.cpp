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
#include <json.h>
#include <types_internal.h>
#include <system_info.h>
#include <dbus_server.h>
#include "app_stats_types.h"
#include "active_window_monitor.h"

/* Active window changes frequently.
 * We thus consider the apps being foregrounded at least 3 secs */
#define MIN_VALID_USE_TIME 2
#define ONE_DAY_IN_SEC 86400

ctx::app_use_monitor::app_use_monitor()
	: signal_id(-1)
	, last_cleanup_time(0)
	, last_timestamp(0)
	, last_pid(-1)
{
	start_logging();
}

ctx::app_use_monitor::~app_use_monitor()
{
	stop_logging();
}

bool ctx::app_use_monitor::start_logging()
{
	signal_id = dbus_server::subscribe_system_signal(NULL,
			"/Org/Tizen/Aul/AppStatus", "org.tizen.aul.AppStatus", "AppStatusChange", this);
	_D("Active window monitoring started (%lld)", signal_id);
	return (signal_id > 0);
}

void ctx::app_use_monitor::stop_logging()
{
	if (signal_id > 0) {
		dbus_server::unsubscribe_system_signal(signal_id);
		_D("Active window monitoring stopped");
	}
}

void ctx::app_use_monitor::on_signal_received(const char* sender, const char* path, const char* iface, const char* name, GVariant* param)
{
	gint pid = 0;
	const gchar *appid = NULL;
	const gchar *pkgid = NULL;
	const gchar *status = NULL;
	const gchar *type = NULL;

	g_variant_get(param, "(i&s&s&s&s)", &pid, &appid, &pkgid, &status, &type);
	IF_FAIL_VOID(appid && status && type);
	IF_FAIL_VOID(STR_EQ(status, "fg") && STR_EQ(type, "uiapp"));

	on_active_window_changed(appid);
}

void ctx::app_use_monitor::on_active_window_changed(std::string app_id)
{
	IF_FAIL_VOID(last_app_id != app_id);
	_D("New fourground app '%s'", app_id.c_str());

	int timestamp = static_cast<int>(time(NULL));
	int duration = timestamp - last_timestamp;

	if (!last_app_id.empty() && duration >= MIN_VALID_USE_TIME)
		verify_used_app(last_app_id.c_str(), duration);

	last_timestamp = timestamp;
	last_app_id = app_id;
}

void ctx::app_use_monitor::verify_used_app(const char *app_id, int duration)
{
	app_info_h app_info = NULL;
	int err = app_manager_get_app_info(app_id, &app_info);
	IF_FAIL_VOID_TAG(err == APP_MANAGER_ERROR_NONE && app_info, _E, "app_manager_get_app_info() failed");

	bool nodisp = false;
	err = app_info_is_nodisplay(app_info, &nodisp);
	IF_FAIL_CATCH_TAG(err == APP_MANAGER_ERROR_NONE, _E, "app_info_is_nodisplay() failed");
	IF_FAIL_CATCH(!nodisp);

	insert_log(app_id, duration);

CATCH:
	if (app_info)
		app_info_destroy(app_info);
}

void ctx::app_use_monitor::insert_log(const char *app_id, int duration)
{
	int audiojack;
	int system_volume;
	int media_volume;
	std::string bssid;

	std::stringstream cols;
	std::stringstream vals;

	/* App ID */
	cols << STATS_APP_ID << ",";
	vals << "'" << app_id << "',";

	/* Audio Jack */
	if (ctx::system_info::get_audio_jack_state(&audiojack)) {
		cols << STATS_AUDIO_JACK << ",";
		vals << audiojack << ",";
	}

	/* Volume */
	if (ctx::system_info::get_volume(&system_volume, &media_volume)) {
		cols << STATS_SYSTEM_VOLUME << "," << STATS_MEDIA_VOLUME << ",";
		vals << system_volume << "," << media_volume << ",";
	}

	/* BSSID */
	if (ctx::system_info::get_wifi_bssid(bssid)) {
		cols << STATS_BSSID << ",";
		vals << "'" << bssid << "',";
	}

	/* Time */
	cols << STATS_UNIV_TIME << ",";
	vals << "(strftime('%s', 'now')) - " << duration << ",";

	cols << STATS_LOCAL_TIME << ",";
	vals << "(strftime('%s', 'now', 'localtime')) - " << duration << ",";

	/* Duration */
	cols << STATS_DURATION;
	vals << duration;

	std::stringstream query;
	append_cleanup_query(query);
	query << "INSERT INTO " << APP_TABLE_USAGE_LOG << " ("
		<< cols.str() << ") VALUES (" << vals.str() << ")";

	db_manager::execute(0, query.str().c_str(), NULL);
}

void ctx::app_use_monitor::append_cleanup_query(std::stringstream &query)
{
	IF_FAIL_VOID(last_timestamp - last_cleanup_time >= ONE_DAY_IN_SEC);

	last_cleanup_time = last_timestamp;

	query << "DELETE FROM " APP_TABLE_USAGE_LOG " WHERE " \
		STATS_UNIV_TIME " < strftime('%s', 'now') - " << LOG_RETENTION_PERIOD << ";";
}
