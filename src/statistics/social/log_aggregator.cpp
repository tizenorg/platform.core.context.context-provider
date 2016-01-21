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
#include <json.h>
#include <db_mgr.h>
#include <timer_mgr.h>
#include <types_internal.h>
#include "social_stats_types.h"
#include "log_aggregator.h"

ctx::contact_log_aggregator::contact_log_aggregator()
	: timer_id(-1)
	, time_diff(0)
{
	create_table();
	timer_id = timer_manager::set_at(3, 0, timer_types::EVERYDAY, this, NULL);
}

ctx::contact_log_aggregator::~contact_log_aggregator()
{
	timer_manager::remove(timer_id);
}

void ctx::contact_log_aggregator::create_table()
{
	static bool done = false;
	IF_FAIL_VOID(!done);

	db_manager::create_table(0, SOCIAL_TABLE_CONTACT_LOG, SOCIAL_TABLE_CONTACT_LOG_COLUMNS, NULL, NULL);
	db_manager::execute(0, SOCIAL_TEMP_CONTACT_FREQ_SQL, NULL);

	done = true;
}

bool ctx::contact_log_aggregator::on_timer_expired(int timer, void* user_data)
{
	aggregate_contact_log();
	return true;
}

void ctx::contact_log_aggregator::aggregate_contact_log()
{
	db_manager::execute(0,
			"SELECT IFNULL(MAX(" STATS_UNIV_TIME "),0) AS " STATS_LAST_TIME \
			", (strftime('%s', 'now', 'localtime')) - (strftime('%s', 'now')) AS " TIME_DIFFERENCE \
			" FROM " SOCIAL_TABLE_CONTACT_LOG, this);
}

void ctx::contact_log_aggregator::on_query_result_received(unsigned int query_id, int error, std::vector<json>& records)
{
	IF_FAIL_VOID_TAG(!records.empty(), _E, "Invalid query result");

	int last_time = 0;
	records[0].get(NULL, STATS_LAST_TIME, &last_time);
	records[0].get(NULL, TIME_DIFFERENCE, &time_diff);

	_D("Last Time: %d / Local - UTC: %d", last_time, time_diff);

	contacts_list_h list = NULL;

	get_updated_contact_log_list(last_time, &list);
	IF_FAIL_VOID(list);

	remove_expired_log();
	insert_contact_log_list(list);
	destroy_contact_log_list(list);
}

void ctx::contact_log_aggregator::get_updated_contact_log_list(int last_time, contacts_list_h *list)
{
	contacts_filter_h filter = NULL;
	contacts_query_h query = NULL;

	int err = contacts_connect();
	IF_FAIL_VOID_TAG(err == CONTACTS_ERROR_NONE, _E, "contacts_connect() failed");

	err = contacts_filter_create(_contacts_phone_log._uri, &filter);
	IF_FAIL_CATCH_TAG(err == CONTACTS_ERROR_NONE, _E, "contacts_filter_create() failed");

	contacts_filter_add_int(filter, _contacts_phone_log.log_type, CONTACTS_MATCH_GREATER_THAN_OR_EQUAL, CONTACTS_PLOG_TYPE_VOICE_INCOMING);
	contacts_filter_add_operator(filter, CONTACTS_FILTER_OPERATOR_AND);
	contacts_filter_add_int(filter, _contacts_phone_log.log_type, CONTACTS_MATCH_LESS_THAN_OR_EQUAL, CONTACTS_PLOG_TYPE_MMS_BLOCKED);
	contacts_filter_add_operator(filter, CONTACTS_FILTER_OPERATOR_AND);
	contacts_filter_add_int(filter, _contacts_phone_log.log_time , CONTACTS_MATCH_GREATER_THAN, last_time);
	contacts_filter_add_operator(filter, CONTACTS_FILTER_OPERATOR_AND);

	err = contacts_query_create(_contacts_phone_log._uri, &query);
	IF_FAIL_CATCH_TAG(err == CONTACTS_ERROR_NONE, _E, "contacts_query_create() failed");

	contacts_query_set_filter(query, filter);
	contacts_query_set_sort(query, _contacts_phone_log.log_time, true);

	err = contacts_db_get_records_with_query(query, 0, 0, list);
	IF_FAIL_CATCH_TAG(err == CONTACTS_ERROR_NONE, _E, "contacts_db_get_records_with_query() failed");

CATCH:
	if (filter)
		contacts_filter_destroy(filter);
	if (query)
		contacts_query_destroy(query);
}

void ctx::contact_log_aggregator::destroy_contact_log_list(contacts_list_h list)
{
	if (list)
		contacts_list_destroy(list, true);

	contacts_disconnect();
}

void ctx::contact_log_aggregator::insert_contact_log_list(contacts_list_h list)
{
	IF_FAIL_VOID(contacts_list_first(list) == CONTACTS_ERROR_NONE);

	do {
		contacts_record_h record = NULL;
		contacts_list_get_current_record_p(list, &record);
		if (record == NULL) break;

		ctx::json data;

		char* address = NULL;
		int log_type;
		int duration = 0;
		int accesstime = 0;

		contacts_record_get_str_p(record, _contacts_phone_log.address, &address);

		if (!address) {
			_W("Getting address failed");
			continue;
		}

		contacts_record_get_int(record, _contacts_phone_log.log_type, &log_type);
		contacts_record_get_int(record, _contacts_phone_log.extra_data1, &duration);
		contacts_record_get_int(record, _contacts_phone_log.log_time, &accesstime);

		data.set(NULL, SOCIAL_ADDRESS, address);
		data.set(NULL, SOCIAL_PHONE_LOG_TYPE, log_type);
		data.set(NULL, STATS_DURATION, duration);
		data.set(NULL, STATS_UNIV_TIME, accesstime);
		data.set(NULL, STATS_LOCAL_TIME, accesstime + time_diff);

		db_manager::insert(0, SOCIAL_TABLE_CONTACT_LOG, data, NULL);

	} while(contacts_list_next(list) == CONTACTS_ERROR_NONE);
}

void ctx::contact_log_aggregator::remove_expired_log()
{
	std::stringstream query;
	query << "DELETE FROM " SOCIAL_TABLE_CONTACT_LOG " WHERE " \
		STATS_UNIV_TIME " < strftime('%s', 'now') - " << LOG_RETENTION_PERIOD;
	db_manager::execute(0, query.str().c_str(), NULL);
}
