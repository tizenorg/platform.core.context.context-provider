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
#include <Ecore_X.h>
#include <app_manager.h>

#include <db_mgr.h>
#include <json.h>
#include <types_internal.h>
#include <system_info.h>
#include "../app_stats_types.h"
#include "active_window_monitor.h"

/* Active window changes frequently.
 * We thus consider the apps being foregrounded at least 3 secs */
#define MIN_VALID_USE_TIME 2

#define ONE_DAY_IN_SEC 86400

static Ecore_Event_Handler *window_property_event_handler = NULL;

ctx::app_use_monitor::app_use_monitor()
	: last_cleanup_time(0)
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
	/* This ecore_x based approach does not work with virtualization features */
	if(window_property_event_handler == NULL) {
		ecore_x_init(NULL);
		ecore_x_event_mask_set(ecore_x_window_root_first_get(), ECORE_X_EVENT_MASK_WINDOW_PROPERTY);
		window_property_event_handler = ecore_event_handler_add(ECORE_X_EVENT_WINDOW_PROPERTY, on_window_property_changed, this);
		IF_FAIL_RETURN_TAG(window_property_event_handler, false, _E, "ecore_event_handler_add() failed");
		_D("Active window monitoring started");
	}

	return true;
}

void ctx::app_use_monitor::stop_logging()
{
	if (window_property_event_handler) {
		ecore_event_handler_del(window_property_event_handler);
		window_property_event_handler = NULL;
		_D("Active window monitoring stopped");
	}
}

Eina_Bool ctx::app_use_monitor::on_window_property_changed(void* data, int type, void* event)
{
	IF_FAIL_RETURN_TAG(data && event, ECORE_CALLBACK_PASS_ON, _W, "Invalid window event");

	Ecore_X_Event_Window_Property *property = static_cast<Ecore_X_Event_Window_Property*>(event);
	Ecore_X_Atom atom = ecore_x_atom_get("_NET_ACTIVE_WINDOW");

	IF_FAIL_RETURN(property->atom == atom, ECORE_CALLBACK_PASS_ON);

	int pid = 0;
	Ecore_X_Window win = 0;

	ecore_x_window_prop_window_get(property->win, atom, &win, 1);
	ecore_x_netwm_pid_get(win, &pid);

	IF_FAIL_RETURN_TAG(pid > 0, ECORE_CALLBACK_PASS_ON, _W, "Invalid pid");

	app_use_monitor *instance = static_cast<app_use_monitor*>(data);
	instance->on_active_window_changed(pid);

	return ECORE_CALLBACK_PASS_ON;
}

void ctx::app_use_monitor::on_active_window_changed(int pid)
{
	IF_FAIL_VOID(last_pid != pid);
	_D("Active window changed: PID-%d", pid);

	int timestamp = static_cast<int>(time(NULL));
	int duration = timestamp - last_timestamp;

	if (!last_app_id.empty() > 0 && duration >= MIN_VALID_USE_TIME)
		verify_used_app(last_app_id.c_str(), duration);

	last_timestamp = timestamp;
	last_pid = pid;

	char *app_id = NULL;
	app_manager_get_app_id(pid, &app_id);
	last_app_id = (app_id ? app_id : "");
	g_free(app_id);
	_D("Current Active App: %s", last_app_id.c_str());
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
