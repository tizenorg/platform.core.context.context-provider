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
#include <db_mgr.h>
#include <json.h>
#include <types_internal.h>

#include <system_info.h>
#include "../app_stats_types.h"
#include "launch_monitor.h"

ctx::app_use_monitor::app_use_monitor()
{
	start_logging();
}

ctx::app_use_monitor::~app_use_monitor()
{
	stop_logging();
}

bool ctx::app_use_monitor::start_logging()
{
	int err = app_manager_set_app_context_event_cb(app_context_event_cb, this);
	IF_FAIL_RETURN_TAG(err == APP_MANAGER_ERROR_NONE, false, _E, "app_manager_set_app_context_event_cb() failed");
	return true;
}

void ctx::app_use_monitor::stop_logging()
{
	app_manager_unset_app_context_event_cb();
}

void ctx::app_use_monitor::app_context_event_cb(app_context_h app_context, app_context_event_e event, void *user_data)
{
	char *app_id = NULL;
	int err = app_context_get_app_id(app_context, &app_id);
	IF_FAIL_VOID_TAG(err == APP_MANAGER_ERROR_NONE, _E, "app_context_get_app_id() failed");

	app_use_monitor *monitor = static_cast<app_use_monitor*>(user_data);

	if (event == APP_CONTEXT_EVENT_LAUNCHED) {
		monitor->log_launch_event(app_id);
	} else if (event == APP_CONTEXT_EVENT_TERMINATED) {
		monitor->log_terminate_event(app_id);
	}
	g_free(app_id);
}

void ctx::app_use_monitor::log_launch_event(const char* app_id)
{
	int audiojack;
	int system_volume;
	int media_volume;
	std::string bssid;
	json data;
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

void ctx::app_use_monitor::log_terminate_event(const char* app_id)
{
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
