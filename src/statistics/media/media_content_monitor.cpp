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

#include <time.h>
#include <types_internal.h>
#include <db_mgr.h>
#include <system_info.h>
#include "media_stats_types.h"
#include "db_handle.h"
#include "media_content_monitor.h"

#define PLAYCOUNT_RETENTION_PERIOD 259200	/* 1 month in secs */
#define ONE_DAY_IN_SEC 86400

ctx::media_content_monitor::media_content_monitor()
	: started(false)
	, last_cleanup_time(0)
{
	db_manager::create_table(0, MEDIA_TABLE_NAME, MEDIA_TABLE_COLUMNS, NULL, NULL);
	db_manager::execute(0, MEDIA_PLAYCOUNT_TABLE_SCHEMA, NULL);

	started = start_monitoring();
}

ctx::media_content_monitor::~media_content_monitor()
{
	if (started)
		stop_monitoring();
}

bool ctx::media_content_monitor::start_monitoring()
{
	int err;
	err = media_content_connect();
	IF_FAIL_RETURN_TAG(err == MEDIA_CONTENT_ERROR_NONE, false, _E, "media_content_connect() failed");

	err = media_content_set_db_updated_cb(on_media_content_db_updated, this);
	if (err != MEDIA_CONTENT_ERROR_NONE) {
		media_content_disconnect();
		_E("media_content_set_db_updated_cb() failed");
		return false;
	}

	return true;
}

void ctx::media_content_monitor::stop_monitoring()
{
	media_content_unset_db_updated_cb();
	media_content_disconnect();
}

void ctx::media_content_monitor::on_media_content_db_updated(
		media_content_error_e error, int pid,
		media_content_db_update_item_type_e update_item,
		media_content_db_update_type_e update_type,
		media_content_type_e media_type,
		char *uuid, char *path, char *mime_type, void *user_data)
{
	IF_FAIL_VOID(error == MEDIA_CONTENT_ERROR_NONE && uuid != NULL);
	IF_FAIL_VOID(update_item == MEDIA_ITEM_FILE && update_type == MEDIA_CONTENT_UPDATE);
	IF_FAIL_VOID(media_type == MEDIA_CONTENT_TYPE_MUSIC || media_type == MEDIA_CONTENT_TYPE_VIDEO);

	media_info_h media = NULL;
	media_info_get_media_from_db(uuid, &media);
	IF_FAIL_VOID_TAG(media, _E, "media_info_get_media_from_db() failed");

	int cnt = -1;
	media_info_get_played_count(media, &cnt);
	media_info_destroy(media);
	IF_FAIL_VOID_TAG(cnt >= 0, _E, "Invalid play count");

	media_content_monitor *instance = static_cast<media_content_monitor*>(user_data);
	instance->update_play_count(uuid,
			(media_type == MEDIA_CONTENT_TYPE_MUSIC) ? MEDIA_TYPE_MUSIC : MEDIA_TYPE_VIDEO,
			cnt);
}

void ctx::media_content_monitor::append_cleanup_query(std::stringstream &query)
{
	int timestamp = static_cast<int>(time(NULL));
	IF_FAIL_VOID(timestamp - last_cleanup_time >= ONE_DAY_IN_SEC);

	last_cleanup_time = timestamp;

	query <<
		"DELETE FROM Log_MediaPlayCount WHERE UTC < strftime('%s', 'now') - " << PLAYCOUNT_RETENTION_PERIOD << ";" \
		"DELETE FROM " MEDIA_TABLE_NAME " WHERE UTC < strftime('%s', 'now') - " << LOG_RETENTION_PERIOD << ";";
}

void ctx::media_content_monitor::update_play_count(const char *uuid, int type, int count)
{
	std::stringstream query;
	query <<
		/* Inserting the media record to the play count table, if not exist */
		"INSERT OR IGNORE INTO Log_MediaPlayCount" \
		" (UUID, MediaType) VALUES ('" << uuid << "'," << type <<");" \
		/* Updating the play count and getting the diff from the previous count */
		"UPDATE Log_MediaPlayCount SET Diff = " << count << " - Count," \
		" Count = " << count << ", UTC = strftime('%s', 'now')" \
		" WHERE UUID = '" << uuid << "';";
	append_cleanup_query(query);
	query <<
		/* Checking whether the play count changes */
		"SELECT MediaType FROM Log_MediaPlayCount" \
		" WHERE UUID = '" << uuid << "' AND Diff > 0;";

	db_manager::execute(0, query.str().c_str(), this);
}

void ctx::media_content_monitor::on_query_result_received(unsigned int query_id, int error, std::vector<json>& records)
{
	IF_FAIL_VOID(!records.empty());

	int media_type = 0;
	records[0].get(NULL, CX_MEDIA_TYPE, &media_type);

	insert_log(media_type);
}

void ctx::media_content_monitor::insert_log(int media_type)
{
	int system_volume = -1, media_volume = -1, audiojack = -1;

	json data;
	data.set(NULL, CX_MEDIA_TYPE, media_type);

	if (ctx::system_info::get_audio_jack_state(&audiojack))
		data.set(NULL, STATS_AUDIO_JACK, audiojack);

	if (ctx::system_info::get_volume(&system_volume, &media_volume)) {
		data.set(NULL, STATS_SYSTEM_VOLUME, system_volume);
		data.set(NULL, STATS_MEDIA_VOLUME, media_volume);
	}

	db_manager::insert(0, MEDIA_TABLE_NAME, data, NULL);
}
