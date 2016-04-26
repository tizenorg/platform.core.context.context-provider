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

#ifndef _CONTEXT_STATS_APP_TYPES_H_
#define _CONTEXT_STATS_APP_TYPES_H_

#include <ProviderTypes.h>

#define APP_HISTORY_PRIV			PRIV_APP_HISTORY
#define APP_SUBJ_LOGGER				SUBJ_APP_LOGGER
#define APP_SUBJ_RECENTLY_USED		SUBJ_APP_RECENTLY_USED
#define APP_SUBJ_FREQUENTLY_USED	SUBJ_APP_FREQUENTLY_USED
#define APP_SUBJ_RARELY_USED		SUBJ_APP_RARELY_USED
#define APP_SUBJ_PEAK_TIME			SUBJ_APP_PEAK_TIME
#define APP_SUBJ_COMMON_SETTING		SUBJ_APP_COMMON_SETTING
#define APP_SUBJ_FREQUENCY			SUBJ_APP_FREQUENCY

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

#endif	/* End of _CONTEXT_STATS_APP_TYPES_H_ */
