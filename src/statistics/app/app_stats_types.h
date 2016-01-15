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

#ifndef __CONTEXT_APP_STATS_TYPES_H__
#define __CONTEXT_APP_STATS_TYPES_H__

#include <common_types.h>

#define APP_HISTORY_PRIV			"apphistory.read"
#define APP_SUBJ_RECENTLY_USED		"stats/app/recently"
#define APP_SUBJ_FREQUENTLY_USED	"stats/app/often"
#define APP_SUBJ_RARELY_USED		"stats/app/rarely"
#define APP_SUBJ_PEAK_TIME			"stats/app/peak_time"
#define APP_SUBJ_COMMON_SETTING		"stats/app/setting"
#define APP_SUBJ_FREQUENCY			"stats/app/frequency"

#define APP_TABLE_REMOVABLE_APP	"Log_RemovableApp"
#define APP_TABLE_REMOVABLE_APP_COLUMNS \
	"AppId TEXT NOT NULL UNIQUE"

#define APP_TABLE_USAGE_LOG		"Log_AppLaunch"
#define APP_TABLE_USAGE_LOG_COLUMNS \
	"AppId TEXT NOT NULL, Duration INTEGER NOT NULL DEFAULT 0, " \
	"SystemVolume INTEGER, MediaVolume INTEGER, AudioJack INTEGER, " \
	"BSSID TEXT, " \
	"UTC TIMESTAMP DEFAULT (strftime('%s', 'now')), " \
	"LocalTime TIMESTAMP DEFAULT (strftime('%s', 'now', 'localtime'))"

#define APP_TEMP_USAGE_FREQ		"Temp_AppLaunchFreq"
#define APP_TEMP_USAGE_FREQ_SQL \
	"CREATE TABLE IF NOT EXISTS " APP_TEMP_USAGE_FREQ \
	" (AppId TEXT NOT NULL UNIQUE, TotalCount INTEGER DEFAULT 0);"

#endif
