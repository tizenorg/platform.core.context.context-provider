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

#ifndef _CONTEXT_STATS_MEDIA_TYPES_H_
#define _CONTEXT_STATS_MEDIA_TYPES_H_

#include "../shared/CommonTypes.h"

#define MEDIA_HISTORY_PRIV					"mediahistory.read"
#define MEDIA_SUBJ_LOGGER					"stats/media/logger"
#define MEDIA_SUBJ_PEAK_TIME_FOR_MUSIC		"stats/music/peak_time"
#define MEDIA_SUBJ_PEAK_TIME_FOR_VIDEO		"stats/video/peak_time"
#define MEDIA_SUBJ_COMMON_SETTING_FOR_MUSIC	"stats/music/setting"
#define MEDIA_SUBJ_COMMON_SETTING_FOR_VIDEO	"stats/video/setting"
#define MEDIA_SUBJ_MUSIC_FREQUENCY			"stats/music/frequency"
#define MEDIA_SUBJ_VIDEO_FREQUENCY			"stats/video/frequency"

#define MEDIA_TABLE_NAME	"Log_MediaPlayback"
#define MEDIA_TABLE_COLUMNS \
	"MediaType INTEGER NOT NULL, " \
	"SystemVolume INTEGER, MediaVolume INTEGER, AudioJack INTEGER, " \
	"UTC TIMESTAMP DEFAULT (strftime('%s', 'now')), " \
	"LocalTime TIMESTAMP DEFAULT (strftime('%s', 'now', 'localtime'))"

#define MEDIA_PLAYCOUNT_TABLE_SCHEMA \
	"CREATE TABLE IF NOT EXISTS Log_MediaPlayCount" \
	" (UUID TEXT NOT NULL PRIMARY KEY, MediaType INTEGER NOT NULL," \
	" Count INTEGER DEFAULT 0, Diff INTEGER DEFAULT 0, " \
	" UTC TIMESTAMP DEFAULT (strftime('%s', 'now')))"

#define CX_MEDIA_TYPE		"MediaType"

enum mediaType_e {
	MEDIA_TYPE_MUSIC = 1,
	MEDIA_TYPE_VIDEO,
};

#endif	/* End of _CONTEXT_STATS_MEDIA_TYPES_H_ */
