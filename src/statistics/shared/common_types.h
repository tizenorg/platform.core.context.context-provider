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

#ifndef __CONTEXT_STATS_COMMON_TYPES_H__
#define __CONTEXT_STATS_COMMON_TYPES_H__

#define LOG_RETENTION_PERIOD	7776000		/* 90 days in secs */

#define DEFAULT_TIMESPAN		30
#define DEFAULT_LIMIT			10

#define STATS_QUERY_RESULT		"QueryResult"
#define STATS_RESULT_SIZE		"ResultSize"
#define STATS_COL_ROW_ID		"row_id"

#define STATS_TIMESPAN			"TimeSpan"
#define STATS_START_TIME		"StartTime"
#define STATS_END_TIME			"EndTime"
#define STATS_LAST_TIME			"LastTime"
#define STATS_TOTAL_COUNT		"TotalCount"
#define STATS_AVERAGE_COUNT		"AvgCount"
#define STATS_DURATION			"Duration"
#define STATS_TOTAL_DURATION	"TotalDuration"
#define STATS_DAY_OF_WEEK		"DayOfWeek"
#define STATS_HOUR_OF_DAY		"HourOfDay"
#define STATS_TIME_OF_DAY		"TimeOfDay"
#define STATS_TOTAL_COUNT		"TotalCount"
#define STATS_APP_ID			"AppId"
#define STATS_PKG_ID			"PkgId"
#define STATS_AUDIO_JACK		"AudioJack"
#define STATS_SYSTEM_VOLUME		"SystemVolume"
#define STATS_MEDIA_VOLUME		"MediaVolume"
#define STATS_BSSID				"BSSID"
#define STATS_UNIV_TIME			"UTC"
#define STATS_LOCAL_TIME		"LocalTime"
#define STATS_RANK				"Rank"

#define STATS_SUN		"Sun"
#define STATS_MON		"Mon"
#define STATS_TUE		"Tue"
#define STATS_WED		"Wed"
#define STATS_THU		"Thu"
#define STATS_FRI		"Fri"
#define STATS_SAT		"Sat"
#define STATS_WEEKDAY	"Weekday"
#define STATS_WEEKEND	"Weekend"

enum stats_day_of_week_e {
	STATS_DAY_OF_WEEK_WEEKDAY = 1,
	STATS_DAY_OF_WEEK_WEEKEND,
	STATS_DAY_OF_WEEK_ALL,
	STATS_DAY_OF_WEEK_SUN,
	STATS_DAY_OF_WEEK_MON,
	STATS_DAY_OF_WEEK_TUE,
	STATS_DAY_OF_WEEK_WED,
	STATS_DAY_OF_WEEK_THU,
	STATS_DAY_OF_WEEK_FRI,
	STATS_DAY_OF_WEEK_SAT,
};

#define TRIG_DEF_RANK			"\"Rank\":{\"type\":\"integer\",\"min\":1}"
#define TRIG_DEF_TOTAL_COUNT	"\"TotalCount\":{\"type\":\"integer\",\"min\":0}"
#define TRIG_DEF_TIME_OF_DAY	"\"TimeOfDay\":{\"type\":\"string\"}"
#define TRIG_DEF_DAY_OF_WEEK	"\"DayOfWeek\":{\"type\":\"string\",\"values\":[\"Mon\",\"Tue\",\"Wed\",\"Thu\",\"Fri\",\"Sat\",\"Sun\",\"Weekday\",\"Weekend\"]}"

#endif
