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
#include <system_info.h>
#include "media_stats_types.h"
#include "db_handle.h"

ctx::media_db_handle::media_db_handle()
{
}

ctx::media_db_handle::~media_db_handle()
{
}

int ctx::media_db_handle::read(const char* subject, ctx::json filter)
{
	//TODO: filter validation (in the API side?)
	std::string query;

	if (STR_EQ(subject, MEDIA_SUBJ_PEAK_TIME_FOR_MUSIC)) {
		query = create_sql_peak_time(MEDIA_TYPE_MUSIC, filter);

	} else if (STR_EQ(subject, MEDIA_SUBJ_PEAK_TIME_FOR_VIDEO)) {
		query = create_sql_peak_time(MEDIA_TYPE_VIDEO, filter);

	} else if (STR_EQ(subject, MEDIA_SUBJ_COMMON_SETTING_FOR_MUSIC)) {
		query = create_sql_common_setting(MEDIA_TYPE_MUSIC, filter);

	} else if (STR_EQ(subject, MEDIA_SUBJ_COMMON_SETTING_FOR_VIDEO)) {
		query = create_sql_common_setting(MEDIA_TYPE_VIDEO, filter);

	} else if (STR_EQ(subject, MEDIA_SUBJ_MUSIC_FREQUENCY)) {
		is_trigger_item = true;
		query = create_sql_frequency(MEDIA_TYPE_MUSIC, filter);

	} else if (STR_EQ(subject, MEDIA_SUBJ_VIDEO_FREQUENCY)) {
		is_trigger_item = true;
		query = create_sql_frequency(MEDIA_TYPE_VIDEO, filter);
	}

	IF_FAIL_RETURN(!query.empty(), ERR_OPERATION_FAILED);

	bool ret = execute_query(subject, filter, query.c_str());
	IF_FAIL_RETURN(ret, ERR_OPERATION_FAILED);

	return ERR_NONE;
}

std::string ctx::media_db_handle::create_where_clause(int media_type, ctx::json filter)
{
	std::stringstream where_clause;

	where_clause << CX_MEDIA_TYPE " = " << media_type << " AND ";
	where_clause << stats_db_handle_base::create_where_clause(filter);

	return where_clause.str();
}

std::string ctx::media_db_handle::create_sql_peak_time(int media_type, ctx::json filter)
{
	std::string where = create_where_clause(media_type, filter);
	return stats_db_handle_base::create_sql_peak_time(filter, MEDIA_TABLE_NAME, where);
}

std::string ctx::media_db_handle::create_sql_common_setting(int media_type, ctx::json filter)
{
	std::string where = create_where_clause(media_type, filter);
	return stats_db_handle_base::create_sql_common_setting(filter, MEDIA_TABLE_NAME, where);
}

std::string ctx::media_db_handle::create_sql_frequency(int media_type, ctx::json filter)
{
	ctx::json filter_cleaned;
	std::string week_str;
	std::string time_of_day;

	if (filter.get(NULL, STATS_DAY_OF_WEEK, &week_str))
		filter_cleaned.set(NULL, STATS_DAY_OF_WEEK, week_str);

	if (filter.get(NULL, STATS_TIME_OF_DAY, &time_of_day))
		filter_cleaned.set(NULL, STATS_TIME_OF_DAY, time_of_day);

	std::string where_clause = create_where_clause(media_type, filter_cleaned);

	std::stringstream query;
	query <<
		"SELECT IFNULL(COUNT(*),0) AS " STATS_TOTAL_COUNT \
		" FROM " MEDIA_TABLE_NAME \
		" WHERE " << where_clause;

	return query.str();
}

void ctx::media_db_handle::reply_trigger_item(int error, ctx::json &json_result)
{
	IF_FAIL_VOID_TAG(STR_EQ(req_subject.c_str(), MEDIA_SUBJ_MUSIC_FREQUENCY) ||
		STR_EQ(req_subject.c_str(), MEDIA_SUBJ_VIDEO_FREQUENCY), _E, "Invalid subject");

	ctx::json results;
	int val;

	json_result.get(NULL, STATS_TOTAL_COUNT, &val);
	results.set(NULL, STATS_TOTAL_COUNT, val);

	context_manager::reply_to_read(req_subject.c_str(), req_filter, error, results);
}
