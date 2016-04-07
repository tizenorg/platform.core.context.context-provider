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
#include <Json.h>
#include <db_mgr.h>
#include <types_internal.h>
#include "SocialStatisticsTypes.h"
#include "LogAggregator.h"

ctx::ContactLogAggregator::ContactLogAggregator() :
	__timerId(-1),
	__timeDiff(0)
{
	__createTable();
	__timerId = __timerManager.setAt(3, 0, DayOfWeek::EVERYDAY, this);
}

ctx::ContactLogAggregator::~ContactLogAggregator()
{
	__timerManager.remove(__timerId);
}

void ctx::ContactLogAggregator::__createTable()
{
	static bool done = false;
	IF_FAIL_VOID(!done);

	db_manager::create_table(0, SOCIAL_TABLE_CONTACT_LOG, SOCIAL_TABLE_CONTACT_LOG_COLUMNS, NULL, NULL);
	db_manager::execute(0, SOCIAL_TEMP_CONTACT_FREQ_SQL, NULL);

	done = true;
}

bool ctx::ContactLogAggregator::onTimerExpired(int timerId)
{
	aggregateContactLog();
	return true;
}

void ctx::ContactLogAggregator::aggregateContactLog()
{
	db_manager::execute(0,
			"SELECT IFNULL(MAX(" STATS_UNIV_TIME "),0) AS " STATS_LAST_TIME \
			", (strftime('%s', 'now', 'localtime')) - (strftime('%s', 'now')) AS " TIME_DIFFERENCE \
			" FROM " SOCIAL_TABLE_CONTACT_LOG, this);
}

void ctx::ContactLogAggregator::on_query_result_received(unsigned int queryId, int error, std::vector<Json>& records)
{
	IF_FAIL_VOID_TAG(!records.empty(), _E, "Invalid query result");

	int lastTime = 0;
	records[0].get(NULL, STATS_LAST_TIME, &lastTime);
	records[0].get(NULL, TIME_DIFFERENCE, &__timeDiff);

	_D("Last Time: %d / Local - UTC: %d", lastTime, __timeDiff);

	contacts_list_h list = NULL;

	__getUpdatedContactLogList(lastTime, &list);
	IF_FAIL_VOID(list);

	__removeExpiredLog();
	__insertContactLogList(list);
	__destroyContactLogList(list);
}

void ctx::ContactLogAggregator::__getUpdatedContactLogList(int lastTime, contacts_list_h *list)
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
	contacts_filter_add_int(filter, _contacts_phone_log.log_time , CONTACTS_MATCH_GREATER_THAN, lastTime);
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

void ctx::ContactLogAggregator::__destroyContactLogList(contacts_list_h list)
{
	if (list)
		contacts_list_destroy(list, true);

	contacts_disconnect();
}

void ctx::ContactLogAggregator::__insertContactLogList(contacts_list_h list)
{
	IF_FAIL_VOID(contacts_list_first(list) == CONTACTS_ERROR_NONE);

	do {
		contacts_record_h record = NULL;
		contacts_list_get_current_record_p(list, &record);
		if (record == NULL) break;

		ctx::Json data;

		char* address = NULL;
		int logType;
		int duration = 0;
		int accessTime = 0;

		contacts_record_get_str_p(record, _contacts_phone_log.address, &address);

		if (!address) {
			_W("Getting address failed");
			continue;
		}

		contacts_record_get_int(record, _contacts_phone_log.log_type, &logType);
		contacts_record_get_int(record, _contacts_phone_log.extra_data1, &duration);
		contacts_record_get_int(record, _contacts_phone_log.log_time, &accessTime);

		data.set(NULL, SOCIAL_ADDRESS, address);
		data.set(NULL, SOCIAL_PHONE_LOG_TYPE, logType);
		data.set(NULL, STATS_DURATION, duration);
		data.set(NULL, STATS_UNIV_TIME, accessTime);
		data.set(NULL, STATS_LOCAL_TIME, accessTime + __timeDiff);

		db_manager::insert(0, SOCIAL_TABLE_CONTACT_LOG, data, NULL);

	} while(contacts_list_next(list) == CONTACTS_ERROR_NONE);
}

void ctx::ContactLogAggregator::__removeExpiredLog()
{
	std::stringstream query;
	query << "DELETE FROM " SOCIAL_TABLE_CONTACT_LOG " WHERE " \
		STATS_UNIV_TIME " < strftime('%s', 'now') - " << LOG_RETENTION_PERIOD;
	db_manager::execute(0, query.str().c_str(), NULL);
}
