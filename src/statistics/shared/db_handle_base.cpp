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
#include <ContextManager.h>
#include "common_types.h"
#include "db_handle_base.h"

#define DAY_OF_WEEK(SECOND) "CAST(strftime('%w', " SECOND ", 'unixepoch') AS INTEGER)"
#define HOUR_OF_DAY(SECOND) "CAST(strftime('%H', " SECOND ", 'unixepoch') AS INTEGER)"

ctx::stats_db_handle_base::stats_db_handle_base()
	: is_trigger_item(false)
{
}

ctx::stats_db_handle_base::~stats_db_handle_base()
{
}

int ctx::stats_db_handle_base::generate_qid()
{
	static int qid = 0;

	if (qid++ < 0) qid = 1;
	return qid;
}

bool ctx::stats_db_handle_base::execute_query(const char* subject, ctx::Json filter, const char* query)
{
	bool ret = __dbManager.execute(generate_qid(), query, this);
	IF_FAIL_RETURN(ret, false);

	req_subject = subject;
	req_filter = filter;

	return true;
}

std::string ctx::stats_db_handle_base::create_where_clause(ctx::Json filter)
{
	std::stringstream where_clause;
	int week = 0;
	int start = 0;
	int end = 0;
	int timespan = DEFAULT_TIMESPAN;
	std::string app_id;
	std::string week_str;
	std::string time_of_day;

	if (filter.get(NULL, STATS_DAY_OF_WEEK, &week_str)) {
		// In case of string (from Trigger)
		if (week_str == STATS_WEEKDAY) {
			week = STATS_DAY_OF_WEEK_WEEKDAY;

		} else if (week_str == STATS_WEEKEND) {
			week = STATS_DAY_OF_WEEK_WEEKEND;

		} else if (week_str == STATS_SUN) {
			week = STATS_DAY_OF_WEEK_SUN;

		} else if (week_str == STATS_MON) {
			week = STATS_DAY_OF_WEEK_MON;

		} else if (week_str == STATS_TUE) {
			week = STATS_DAY_OF_WEEK_TUE;

		} else if (week_str == STATS_WED) {
			week = STATS_DAY_OF_WEEK_WED;

		} else if (week_str == STATS_THU) {
			week = STATS_DAY_OF_WEEK_THU;

		} else if (week_str == STATS_FRI) {
			week = STATS_DAY_OF_WEEK_FRI;

		} else if (week_str == STATS_SAT) {
			week = STATS_DAY_OF_WEEK_SAT;
		}
	} else {
		// In case of integer (from History)
		filter.get(NULL, STATS_DAY_OF_WEEK, &week);
	}

	switch(week) {
	case STATS_DAY_OF_WEEK_WEEKDAY:
		where_clause << "(" DAY_OF_WEEK(STATS_LOCAL_TIME) " > 0 AND " DAY_OF_WEEK(STATS_LOCAL_TIME) " < 6) AND ";
		break;
	case STATS_DAY_OF_WEEK_WEEKEND:
		where_clause << "(" DAY_OF_WEEK(STATS_LOCAL_TIME) " = 0 OR " DAY_OF_WEEK(STATS_LOCAL_TIME) " = 6) AND ";
		break;
	case STATS_DAY_OF_WEEK_SUN:
	case STATS_DAY_OF_WEEK_MON:
	case STATS_DAY_OF_WEEK_TUE:
	case STATS_DAY_OF_WEEK_WED:
	case STATS_DAY_OF_WEEK_THU:
	case STATS_DAY_OF_WEEK_FRI:
	case STATS_DAY_OF_WEEK_SAT:
		where_clause << DAY_OF_WEEK(STATS_LOCAL_TIME) " = " << week - STATS_DAY_OF_WEEK_SUN << " AND ";
		break;
	default:
		break;
	}

	if (filter.get(NULL, STATS_APP_ID, &app_id))
		where_clause << STATS_APP_ID " = '" << app_id << "' AND ";

	if (filter.get(NULL, STATS_START_TIME, &start))
		where_clause << STATS_UNIV_TIME " >= " << start << " AND ";

	if (filter.get(NULL, STATS_END_TIME, &end))
		where_clause << STATS_UNIV_TIME " <= " << end << " AND ";

	if (filter.get(NULL, STATS_TIME_OF_DAY, &time_of_day)) {
		size_t pivot = time_of_day.find('-');
		if (pivot != std::string::npos) {
			std::string from = time_of_day.substr(0, pivot);
			std::string to = time_of_day.substr(pivot + 1);
			where_clause << "(" HOUR_OF_DAY(STATS_LOCAL_TIME) " >= " << from \
				<< " AND " HOUR_OF_DAY(STATS_LOCAL_TIME) " < " << to << ") AND ";
		}
	}

	filter.get(NULL, STATS_TIMESPAN, &timespan);
	where_clause << STATS_UNIV_TIME " > strftime('%s', 'now', '-" << timespan <<" day')";

	return where_clause.str();
}

std::string ctx::stats_db_handle_base::create_sql_peak_time(ctx::Json filter, const char* table_name, std::string where_clause)
{
	std::stringstream query;
	int limit = DEFAULT_LIMIT;

	filter.get(NULL, STATS_RESULT_SIZE, &limit);

	query <<
		"SELECT " \
			HOUR_OF_DAY(STATS_LOCAL_TIME) " AS " STATS_HOUR_OF_DAY ", COUNT(*) AS " STATS_TOTAL_COUNT \
		" FROM " << table_name << \
		" WHERE " << where_clause << \
		" GROUP BY " HOUR_OF_DAY(STATS_LOCAL_TIME) \
		" ORDER BY " STATS_TOTAL_COUNT " DESC" \
		" LIMIT " << limit;

	return query.str();
}

std::string ctx::stats_db_handle_base::create_sql_common_setting(ctx::Json filter, const char* table_name, std::string where_clause)
{
	std::stringstream query;

	query <<
		"SELECT ( SELECT " STATS_AUDIO_JACK \
				" FROM " << table_name << \
				" WHERE " << where_clause << \
				" GROUP BY " STATS_AUDIO_JACK \
				" ORDER BY count(" STATS_AUDIO_JACK ") DESC" \
				" LIMIT 1 ) AS " STATS_AUDIO_JACK \
			", ( SELECT " STATS_SYSTEM_VOLUME \
				" FROM " << table_name << \
				" WHERE " << where_clause << \
				" GROUP BY " STATS_SYSTEM_VOLUME \
				" ORDER BY count(" STATS_SYSTEM_VOLUME ") DESC" \
				" LIMIT 1 ) AS " STATS_SYSTEM_VOLUME \
			", ( SELECT " STATS_MEDIA_VOLUME \
				" FROM " << table_name << \
				" WHERE " << where_clause << \
				" GROUP BY " STATS_MEDIA_VOLUME \
				" ORDER BY count(" STATS_MEDIA_VOLUME ") DESC" \
				" LIMIT 1 ) AS " STATS_MEDIA_VOLUME;

	return query.str();
}

void ctx::stats_db_handle_base::onTableCreated(unsigned int query_id, int error)
{
}

void ctx::stats_db_handle_base::onInserted(unsigned int query_id, int error, int64_t row_id)
{
	delete this;
}

void ctx::stats_db_handle_base::json_vector_to_array(std::vector<Json> &vec_json, ctx::Json &json_result)
{
	std::vector<Json>::iterator json_vec_end = vec_json.end();

	for(std::vector<Json>::iterator json_vec_pos = vec_json.begin(); json_vec_pos != json_vec_end; ++json_vec_pos) {
		Json origin_j = *json_vec_pos;
		json_result.append(NULL, STATS_QUERY_RESULT, origin_j);
	}
}

void ctx::stats_db_handle_base::onExecuted(unsigned int query_id, int error, std::vector<Json>& records)
{
	if (is_trigger_item) {
		if (records.size() == 1) {
			reply_trigger_item(error, records[0]);
		} else {
			_E("Invalid query result");
			Json dummy;
			context_manager::replyToRead(req_subject.c_str(), req_filter, ERR_OPERATION_FAILED, dummy);
		}
	} else {
		Json results = "{\"" STATS_QUERY_RESULT "\":[]}";
		json_vector_to_array(records, results);
		context_manager::replyToRead(req_subject.c_str(), req_filter, error, results);
	}

	delete this;
}
