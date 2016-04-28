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

#include <gmodule.h>
#include <time.h>
#include <media-util-noti-common.h>
#include <Types.h>
#include "../shared/SystemInfo.h"
#include "MediaStatisticsTypes.h"
#include "DbHandle.h"
#include "MediaContentMonitor.h"

#define SO_PATH "/usr/lib/context-service/libctx-prvd-media-stats-util.so"

#define PLAYCOUNT_RETENTION_PERIOD 259200	/* 1 month in secs */
#define ONE_DAY_IN_SEC 86400

typedef bool (*get_play_count_t)(int updateItem, int updateType, int mediaType, char *uuid, int *count);

ctx::MediaContentMonitor::MediaContentMonitor() :
	__started(false),
	__lastCleanupTime(0),
	__dbusSignalId(-1),
	__dbusWatcher(DBusType::SYSTEM)
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
	__dbusSignalId = __dbusWatcher.watch(NULL, MS_MEDIA_DBUS_PATH, MS_MEDIA_DBUS_INTERFACE, "ms_db_updated", this);
	IF_FAIL_RETURN(__dbusSignalId >= 0, false);
	return true;
}

void ctx::MediaContentMonitor::__stopMonitoring()
{
	__dbusWatcher.unwatch(__dbusSignalId);
}

void ctx::MediaContentMonitor::onSignal(const char *sender, const char *path, const char *iface, const char *name, GVariant *param)
{
	if (g_variant_n_children(param) < 7)
		return;

	gint32 item = -1;
	gint32 pid = 0;
	gint32 updateType = MS_MEDIA_UNKNOWN;
	gint32 contentType = -1;
	char *updatePath = NULL;
	char *uuid = NULL;
	char *mimeType = NULL;

	g_variant_get(param, "(iii&s&si&s)", &item, &pid, &updateType, &updatePath, &uuid, &contentType, &mimeType);

	int playCount;
	if (__getPlayCount(item, updateType, contentType, uuid, &playCount))
		__updatePlayCount(uuid, (contentType == MS_MEDIA_MUSIC) ? MEDIA_TYPE_MUSIC : MEDIA_TYPE_VIDEO, playCount);
}

bool ctx::MediaContentMonitor::__getPlayCount(int updateItem, int updateType, int mediaType, char *uuid, int *count)
{
	GModule *soHandle = g_module_open(SO_PATH, G_MODULE_BIND_LAZY);
	IF_FAIL_RETURN_TAG(soHandle, false, _E, "%s", g_module_error());

	gpointer symbol;
	if (!g_module_symbol(soHandle, "getMediaPlayCount", &symbol) || symbol == NULL) {
		_E("%s", g_module_error());
		g_module_close(soHandle);
		return false;
	}

	get_play_count_t getCount = reinterpret_cast<get_play_count_t>(symbol);

	bool ret = getCount(updateItem, updateType, mediaType, uuid, count);
	g_module_close(soHandle);

	return ret;
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
