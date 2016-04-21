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
#include <Types.h>
#include "../shared/SystemInfo.h"
#include "MediaStatisticsTypes.h"
#include "DbHandle.h"
#include "MediaContentMonitor.h"

#define PLAYCOUNT_RETENTION_PERIOD 259200	/* 1 month in secs */
#define ONE_DAY_IN_SEC 86400

ctx::MediaContentMonitor::MediaContentMonitor() :
	__started(false),
	__lastCleanupTime(0)
{
	__dbManager.createTable(0, MEDIA_TABLE_NAME, MEDIA_TABLE_COLUMNS, NULL, NULL);
	__dbManager.execute(0, MEDIA_PLAYCOUNT_TABLE_SCHEMA, NULL);

	__started = __startMonitoring();
}

ctx::MediaContentMonitor::~MediaContentMonitor()
{
	if (__started)
		__stopMonitoring();
}

bool ctx::MediaContentMonitor::__startMonitoring()
{
	int err;
	err = media_content_connect();
	IF_FAIL_RETURN_TAG(err == MEDIA_CONTENT_ERROR_NONE, false, _E, "media_content_connect() failed");

	err = media_content_set_db_updated_cb(__onMediaContentDbUpdated, this);
	if (err != MEDIA_CONTENT_ERROR_NONE) {
		media_content_disconnect();
		_E("media_content_set_db_updated_cb() failed");
		return false;
	}

	return true;
}

void ctx::MediaContentMonitor::__stopMonitoring()
{
	media_content_unset_db_updated_cb();
	media_content_disconnect();
}

void ctx::MediaContentMonitor::__onMediaContentDbUpdated(
		media_content_error_e error, int pid,
		media_content_db_update_item_type_e updateItem,
		media_content_db_update_type_e updateType,
		media_content_type_e mediaType,
		char *uuid, char *path, char *mimeType, void *userData)
{
	IF_FAIL_VOID(error == MEDIA_CONTENT_ERROR_NONE && uuid != NULL);
	IF_FAIL_VOID(updateItem == MEDIA_ITEM_FILE && updateType == MEDIA_CONTENT_UPDATE);
	IF_FAIL_VOID(mediaType == MEDIA_CONTENT_TYPE_MUSIC || mediaType == MEDIA_CONTENT_TYPE_VIDEO);

	media_info_h media = NULL;
	media_info_get_media_from_db(uuid, &media);
	IF_FAIL_VOID_TAG(media, _E, "media_info_get_media_from_db() failed");

	int cnt = -1;
	media_info_get_played_count(media, &cnt);
	media_info_destroy(media);
	IF_FAIL_VOID_TAG(cnt >= 0, _E, "Invalid play count");

	MediaContentMonitor *instance = static_cast<MediaContentMonitor*>(userData);
	instance->__updatePlayCount(uuid,
			(mediaType == MEDIA_CONTENT_TYPE_MUSIC) ? MEDIA_TYPE_MUSIC : MEDIA_TYPE_VIDEO,
			cnt);
}

void ctx::MediaContentMonitor::__appendCleanupQuery(std::stringstream &query)
{
	int timestamp = static_cast<int>(time(NULL));
	IF_FAIL_VOID(timestamp - __lastCleanupTime >= ONE_DAY_IN_SEC);

	__lastCleanupTime = timestamp;

	query <<
		"DELETE FROM Log_MediaPlayCount WHERE UTC < strftime('%s', 'now') - " << PLAYCOUNT_RETENTION_PERIOD << ";" \
		"DELETE FROM " MEDIA_TABLE_NAME " WHERE UTC < strftime('%s', 'now') - " << LOG_RETENTION_PERIOD << ";";
}

void ctx::MediaContentMonitor::__updatePlayCount(const char *uuid, int type, int count)
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
	__appendCleanupQuery(query);
	query <<
		/* Checking whether the play count changes */
		"SELECT MediaType FROM Log_MediaPlayCount" \
		" WHERE UUID = '" << uuid << "' AND Diff > 0;";

	__dbManager.execute(0, query.str().c_str(), this);
}

void ctx::MediaContentMonitor::onExecuted(unsigned int queryId, int error, std::vector<Json>& records)
{
	IF_FAIL_VOID(!records.empty());

	int mediaType = 0;
	records[0].get(NULL, CX_MEDIA_TYPE, &mediaType);

	__insertLog(mediaType);
}

void ctx::MediaContentMonitor::__insertLog(int mediaType)
{
	int systemVolume = -1, mediaVolume = -1, audioJack = -1;

	Json data;
	data.set(NULL, CX_MEDIA_TYPE, mediaType);

	if (ctx::system_info::getAudioJackState(&audioJack))
		data.set(NULL, KEY_AUDIO_JACK, audioJack);

	if (ctx::system_info::getVolume(&systemVolume, &mediaVolume)) {
		data.set(NULL, KEY_SYSTEM_VOLUME, systemVolume);
		data.set(NULL, KEY_MEDIA_VOLUME, mediaVolume);
	}

	__dbManager.insert(0, MEDIA_TABLE_NAME, data, NULL);
}
