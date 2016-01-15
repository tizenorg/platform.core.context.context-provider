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
#include <contacts.h>
#include <types_internal.h>
#include <context_mgr.h>
#include <db_mgr.h>
#include "social_stats_types.h"
#include "db_handle.h"

ctx::social_db_handle::social_db_handle()
{
}

ctx::social_db_handle::~social_db_handle()
{
}

int ctx::social_db_handle::read(const char* subject, ctx::json filter)
{
	std::string query;

	if (STR_EQ(subject, SOCIAL_SUBJ_FREQ_ADDRESS)) {
		query = create_sql_freq_address(filter);

	} else if (STR_EQ(subject, SOCIAL_SUBJ_FREQUENCY)) {
		is_trigger_item = true;
		query = create_sql_frequency(filter);
	}

	IF_FAIL_RETURN(!query.empty(), ERR_OPERATION_FAILED);

	bool ret = execute_query(subject, filter, query.c_str());
	IF_FAIL_RETURN(ret, ERR_OPERATION_FAILED);

	return ERR_NONE;
}

std::string ctx::social_db_handle::create_where_clause(ctx::json filter)
{
	std::stringstream where_clause;
	int comm_type = -1;

	where_clause << stats_db_handle_base::create_where_clause(filter);

	filter.get(NULL, SOCIAL_COMMUNICATION_TYPE, &comm_type);

	switch(comm_type) {
	case SOCIAL_COMMUNICATION_TYPE_CALL:
		where_clause <<
			" AND " SOCIAL_PHONE_LOG_TYPE " >= " << CONTACTS_PLOG_TYPE_VOICE_INCOMMING <<
			" AND " SOCIAL_PHONE_LOG_TYPE " <= " << CONTACTS_PLOG_TYPE_VIDEO_BLOCKED;
		break;
	case SOCIAL_COMMUNICATION_TYPE_MESSAGE:
		where_clause <<
			" AND " SOCIAL_PHONE_LOG_TYPE " >= " << CONTACTS_PLOG_TYPE_MMS_INCOMMING <<
			" AND " SOCIAL_PHONE_LOG_TYPE " <= " << CONTACTS_PLOG_TYPE_MMS_BLOCKED;
		break;
	default:
		break;
	}

	return where_clause.str();
}

std::string ctx::social_db_handle::create_sql_freq_address(ctx::json filter)
{
	std::stringstream query;
	int limit = DEFAULT_LIMIT;

	filter.get(NULL, STATS_RESULT_SIZE, &limit);

	query <<
		"SELECT " SOCIAL_ADDRESS ", " \
			"COUNT(*) AS " STATS_TOTAL_COUNT ", " \
			"SUM(" STATS_DURATION ") AS " STATS_TOTAL_DURATION ", " \
			"MAX(" STATS_UNIV_TIME ") AS " STATS_LAST_TIME \
		" FROM " SOCIAL_TABLE_CONTACT_LOG \
		" WHERE " << create_where_clause(filter) <<
		" GROUP BY " SOCIAL_ADDRESS \
		" ORDER BY COUNT(*) DESC" \
		" LIMIT " << limit;

	return query.str();
}

std::string ctx::social_db_handle::create_sql_frequency(ctx::json filter)
{
	ctx::json filter_cleaned;
	std::string week_str;
	std::string time_of_day;
	std::string address;

	if (!filter.get(NULL, SOCIAL_ADDRESS, &address)) {
		_E("Invalid parameter");
		return "";
	}

	if (filter.get(NULL, STATS_DAY_OF_WEEK, &week_str))
		filter_cleaned.set(NULL, STATS_DAY_OF_WEEK, week_str);

	if (filter.get(NULL, STATS_TIME_OF_DAY, &time_of_day))
		filter_cleaned.set(NULL, STATS_TIME_OF_DAY, time_of_day);

	std::stringstream query;

	query <<
		"DELETE FROM " SOCIAL_TEMP_CONTACT_FREQ ";";

	query <<
		"INSERT INTO " SOCIAL_TEMP_CONTACT_FREQ \
		" SELECT " SOCIAL_ADDRESS ", COUNT(*) AS " STATS_TOTAL_COUNT \
		" FROM " SOCIAL_TABLE_CONTACT_LOG \
		" WHERE " << create_where_clause(filter_cleaned) <<
		" GROUP BY " SOCIAL_ADDRESS ";";

	query <<
		"INSERT OR IGNORE INTO " SOCIAL_TEMP_CONTACT_FREQ " (" SOCIAL_ADDRESS ")" \
		" VALUES ('" << address << "');";

	query <<
		"SELECT S." SOCIAL_ADDRESS ", S." STATS_TOTAL_COUNT ", 1+COUNT(lesser." STATS_TOTAL_COUNT ") AS " STATS_RANK \
		" FROM " SOCIAL_TEMP_CONTACT_FREQ " AS S" \
		" LEFT JOIN " SOCIAL_TEMP_CONTACT_FREQ " AS lesser" \
		" ON S." STATS_TOTAL_COUNT " < lesser." STATS_TOTAL_COUNT \
		" WHERE S." SOCIAL_ADDRESS " = '" << address << "'";


	return query.str();
}

void ctx::social_db_handle::reply_trigger_item(int error, ctx::json &json_result)
{
	IF_FAIL_VOID_TAG(STR_EQ(req_subject.c_str(), SOCIAL_SUBJ_FREQUENCY), _E, "Invalid subject");

	ctx::json results;
	std::string val_str;
	int val;

	json_result.get(NULL, SOCIAL_ADDRESS, &val_str);
	results.set(NULL, SOCIAL_ADDRESS, val_str);
	json_result.get(NULL, STATS_TOTAL_COUNT, &val);
	results.set(NULL, STATS_TOTAL_COUNT, val);
	json_result.get(NULL, STATS_RANK, &val);
	results.set(NULL, STATS_RANK, val);

	context_manager::reply_to_read(req_subject.c_str(), req_filter, error, results);
}
