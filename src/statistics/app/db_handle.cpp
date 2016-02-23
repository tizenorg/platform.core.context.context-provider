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
#include <types_internal.h>
#include <context_mgr.h>
#include <db_mgr.h>
#include "app_stats_types.h"
#include "db_handle.h"

ctx::app_db_handle::app_db_handle()
{
}

ctx::app_db_handle::~app_db_handle()
{
}

int ctx::app_db_handle::read(const char* subject, ctx::Json filter)
{
	std::string query;

	if (STR_EQ(subject, APP_SUBJ_RECENTLY_USED)) {
		query = create_sql_recently_used(filter);

	} else if (STR_EQ(subject, APP_SUBJ_FREQUENTLY_USED)) {
		query = create_sql_frequently_used(filter);

	} else if (STR_EQ(subject, APP_SUBJ_RARELY_USED)) {
		query = create_sql_rarely_used(filter);

	} else if (STR_EQ(subject, APP_SUBJ_PEAK_TIME)) {
		query = create_sql_peak_time(filter);

	} else if (STR_EQ(subject, APP_SUBJ_COMMON_SETTING)) {
		query = create_sql_common_setting(filter);

	} else if (STR_EQ(subject, APP_SUBJ_FREQUENCY)) {
		is_trigger_item = true;
		query = create_sql_frequency(filter);
	}

	IF_FAIL_RETURN(!query.empty(), ERR_OPERATION_FAILED);

	bool ret = execute_query(subject, filter, query.c_str());
	IF_FAIL_RETURN(ret, ERR_OPERATION_FAILED);

	return ERR_NONE;
}

std::string ctx::app_db_handle::create_where_clause_with_device_status(ctx::Json filter)
{
	std::stringstream where_clause;
	std::string bssid;
	int audio_jack;

	where_clause << stats_db_handle_base::create_where_clause(filter);

	if (filter.get(NULL, STATS_BSSID, &bssid))
		where_clause << " AND " STATS_BSSID " = '" << bssid << "'";

	if (filter.get(NULL, STATS_AUDIO_JACK, &audio_jack))
		where_clause << " AND " STATS_AUDIO_JACK " = " << audio_jack;

	return where_clause.str();
}

std::string ctx::app_db_handle::create_sql_peak_time(ctx::Json filter)
{
	return stats_db_handle_base::create_sql_peak_time(filter, APP_TABLE_USAGE_LOG, create_where_clause(filter));
}

std::string ctx::app_db_handle::create_sql_common_setting(ctx::Json filter)
{
	return stats_db_handle_base::create_sql_common_setting(filter, APP_TABLE_USAGE_LOG, create_where_clause(filter));
}

std::string ctx::app_db_handle::create_sql_frequency(ctx::Json filter)
{
	ctx::Json filter_cleaned;
	std::string week_str;
	std::string time_of_day;
	std::string app_id;

	if (!filter.get(NULL, STATS_APP_ID, &app_id)) {
		_E("Invalid parameter");
		return "";
	}

	if (filter.get(NULL, STATS_DAY_OF_WEEK, &week_str))
		filter_cleaned.set(NULL, STATS_DAY_OF_WEEK, week_str);

	if (filter.get(NULL, STATS_TIME_OF_DAY, &time_of_day))
		filter_cleaned.set(NULL, STATS_TIME_OF_DAY, time_of_day);

	std::string where_clause = create_where_clause(filter_cleaned);

	std::stringstream query;

	query <<
		"DELETE FROM " APP_TEMP_USAGE_FREQ ";";

	query <<
		"INSERT INTO " APP_TEMP_USAGE_FREQ \
		" SELECT " STATS_APP_ID ", COUNT(*) AS " STATS_TOTAL_COUNT \
		" FROM " APP_TABLE_USAGE_LOG \
		" WHERE " << where_clause <<
		" GROUP BY " STATS_APP_ID ";";

	query <<
		"INSERT OR IGNORE INTO " APP_TEMP_USAGE_FREQ " (" STATS_APP_ID ")" \
		" VALUES ('" << app_id << "');";

	query <<
		"SELECT S." STATS_APP_ID ", S." STATS_TOTAL_COUNT ", 1+COUNT(lesser." STATS_TOTAL_COUNT ") AS " STATS_RANK \
		" FROM " APP_TEMP_USAGE_FREQ " AS S" \
		" LEFT JOIN " APP_TEMP_USAGE_FREQ " AS lesser" \
		" ON S." STATS_TOTAL_COUNT " < lesser." STATS_TOTAL_COUNT \
		" WHERE S." STATS_APP_ID " = '" << app_id << "'";

	return query.str();
}

std::string ctx::app_db_handle::create_sql_recently_used(ctx::Json filter)
{
	std::stringstream query;
	int limit = DEFAULT_LIMIT;

	filter.get(NULL, STATS_RESULT_SIZE, &limit);

	query <<
		"SELECT " STATS_APP_ID ", " \
			"COUNT(*) AS " STATS_TOTAL_COUNT ", " \
			"SUM(" STATS_DURATION ") AS " STATS_TOTAL_DURATION ", " \
			"MAX(" STATS_UNIV_TIME ") AS " STATS_LAST_TIME \
		" FROM " APP_TABLE_USAGE_LOG \
		" WHERE " << create_where_clause_with_device_status(filter) <<
		" GROUP BY " STATS_APP_ID \
		" ORDER BY MAX(" STATS_UNIV_TIME ") DESC" \
		" LIMIT " << limit;

	return query.str();
}

std::string ctx::app_db_handle::create_sql_frequently_used(ctx::Json filter)
{
	std::stringstream query;
	int limit = DEFAULT_LIMIT;

	filter.get(NULL, STATS_RESULT_SIZE, &limit);

	query <<
		"SELECT " STATS_APP_ID ", " \
			"COUNT(*) AS " STATS_TOTAL_COUNT ", " \
			"SUM(" STATS_DURATION ") AS " STATS_TOTAL_DURATION ", " \
			"MAX(" STATS_UNIV_TIME ") AS " STATS_LAST_TIME \
		" FROM " APP_TABLE_USAGE_LOG \
		" WHERE " << create_where_clause_with_device_status(filter) <<
		" GROUP BY " STATS_APP_ID \
		" ORDER BY COUNT(*) DESC" \
		" LIMIT " << limit;

	return query.str();
}

std::string ctx::app_db_handle::create_sql_rarely_used(ctx::Json filter)
{
	std::stringstream query;
	int limit = DEFAULT_LIMIT;

	filter.get(NULL, STATS_RESULT_SIZE, &limit);

	query <<
		"SELECT i." STATS_APP_ID ", " \
			"COUNT(u." STATS_DURATION ") AS " STATS_TOTAL_COUNT ", " \
			"IFNULL(SUM(u." STATS_DURATION "),0) AS " STATS_TOTAL_DURATION ", " \
			"IFNULL(MAX(u." STATS_UNIV_TIME "),-1) AS " STATS_LAST_TIME \
		" FROM " APP_TABLE_REMOVABLE_APP " i LEFT OUTER JOIN (" \
			" SELECT * FROM " APP_TABLE_USAGE_LOG \
			" WHERE " << create_where_clause_with_device_status(filter) << ") u" \
			" ON i." STATS_APP_ID " = u." STATS_APP_ID \
		" GROUP BY i." STATS_APP_ID \
		" ORDER BY " STATS_TOTAL_COUNT " ASC" \
		" LIMIT " << limit;

	return query.str();
}

void ctx::app_db_handle::reply_trigger_item(int error, ctx::Json &json_result)
{
	IF_FAIL_VOID_TAG(STR_EQ(req_subject.c_str(), APP_SUBJ_FREQUENCY), _E, "Invalid subject");

	ctx::Json results;
	std::string val_str;
	int val;

	json_result.get(NULL, STATS_APP_ID, &val_str);
	results.set(NULL, STATS_APP_ID, val_str);
	json_result.get(NULL, STATS_TOTAL_COUNT, &val);
	results.set(NULL, STATS_TOTAL_COUNT, val);
	json_result.get(NULL, STATS_RANK, &val);
	results.set(NULL, STATS_RANK, val);

	context_manager::reply_to_read(req_subject.c_str(), req_filter, error, results);
}
