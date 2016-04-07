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
#include "AppStatisticsTypes.h"
#include "DbHandle.h"

ctx::AppDbHandle::AppDbHandle()
{
}

ctx::AppDbHandle::~AppDbHandle()
{
}

int ctx::AppDbHandle::read(const char* subject, ctx::Json filter)
{
	std::string query;

	if (STR_EQ(subject, APP_SUBJ_RECENTLY_USED)) {
		query = createSqlRecentlyUsed(filter);

	} else if (STR_EQ(subject, APP_SUBJ_FREQUENTLY_USED)) {
		query = createSqlFrequentlyUsed(filter);

	} else if (STR_EQ(subject, APP_SUBJ_RARELY_USED)) {
		query = createSqlRarelyUsed(filter);

	} else if (STR_EQ(subject, APP_SUBJ_PEAK_TIME)) {
		query = createSqlPeakTime(filter);

	} else if (STR_EQ(subject, APP_SUBJ_COMMON_SETTING)) {
		query = createSqlCommonSetting(filter);

	} else if (STR_EQ(subject, APP_SUBJ_FREQUENCY)) {
		__isTriggerItem = true;
		query = createSqlFrequency(filter);
	}

	IF_FAIL_RETURN(!query.empty(), ERR_OPERATION_FAILED);

	bool ret = executeQuery(subject, filter, query.c_str());
	IF_FAIL_RETURN(ret, ERR_OPERATION_FAILED);

	return ERR_NONE;
}

std::string ctx::AppDbHandle::createWhereClauseWithDeviceStatus(ctx::Json filter)
{
	std::stringstream whereClause;
	std::string bssid;
	int audioJack;

	whereClause << StatsDbHandleBase::createWhereClause(filter);

	if (filter.get(NULL, STATS_BSSID, &bssid))
		whereClause << " AND " STATS_BSSID " = '" << bssid << "'";

	if (filter.get(NULL, STATS_AUDIO_JACK, &audioJack))
		whereClause << " AND " STATS_AUDIO_JACK " = " << audioJack;

	return whereClause.str();
}

std::string ctx::AppDbHandle::createSqlPeakTime(ctx::Json filter)
{
	return StatsDbHandleBase::createSqlPeakTime(filter, APP_TABLE_USAGE_LOG, createWhereClause(filter));
}

std::string ctx::AppDbHandle::createSqlCommonSetting(ctx::Json filter)
{
	return StatsDbHandleBase::createSqlCommonSetting(filter, APP_TABLE_USAGE_LOG, createWhereClause(filter));
}

std::string ctx::AppDbHandle::createSqlFrequency(ctx::Json filter)
{
	ctx::Json filterCleaned;
	std::string weekStr;
	std::string timeOfDay;
	std::string appId;

	if (!filter.get(NULL, STATS_APP_ID, &appId)) {
		_E("Invalid parameter");
		return "";
	}

	if (filter.get(NULL, STATS_DAY_OF_WEEK, &weekStr))
		filterCleaned.set(NULL, STATS_DAY_OF_WEEK, weekStr);

	if (filter.get(NULL, STATS_TIME_OF_DAY, &timeOfDay))
		filterCleaned.set(NULL, STATS_TIME_OF_DAY, timeOfDay);

	std::string whereClause = createWhereClause(filterCleaned);

	std::stringstream query;

	query <<
		"DELETE FROM " APP_TEMP_USAGE_FREQ ";";

	query <<
		"INSERT INTO " APP_TEMP_USAGE_FREQ \
		" SELECT " STATS_APP_ID ", COUNT(*) AS " STATS_TOTAL_COUNT \
		" FROM " APP_TABLE_USAGE_LOG \
		" WHERE " << whereClause <<
		" GROUP BY " STATS_APP_ID ";";

	query <<
		"INSERT OR IGNORE INTO " APP_TEMP_USAGE_FREQ " (" STATS_APP_ID ")" \
		" VALUES ('" << appId << "');";

	query <<
		"SELECT S." STATS_APP_ID ", S." STATS_TOTAL_COUNT ", 1+COUNT(lesser." STATS_TOTAL_COUNT ") AS " STATS_RANK \
		" FROM " APP_TEMP_USAGE_FREQ " AS S" \
		" LEFT JOIN " APP_TEMP_USAGE_FREQ " AS lesser" \
		" ON S." STATS_TOTAL_COUNT " < lesser." STATS_TOTAL_COUNT \
		" WHERE S." STATS_APP_ID " = '" << appId << "'";

	return query.str();
}

std::string ctx::AppDbHandle::createSqlRecentlyUsed(ctx::Json filter)
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
		" WHERE " << createWhereClauseWithDeviceStatus(filter) <<
		" GROUP BY " STATS_APP_ID \
		" ORDER BY MAX(" STATS_UNIV_TIME ") DESC" \
		" LIMIT " << limit;

	return query.str();
}

std::string ctx::AppDbHandle::createSqlFrequentlyUsed(ctx::Json filter)
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
		" WHERE " << createWhereClauseWithDeviceStatus(filter) <<
		" GROUP BY " STATS_APP_ID \
		" ORDER BY COUNT(*) DESC" \
		" LIMIT " << limit;

	return query.str();
}

std::string ctx::AppDbHandle::createSqlRarelyUsed(ctx::Json filter)
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
			" WHERE " << createWhereClauseWithDeviceStatus(filter) << ") u" \
			" ON i." STATS_APP_ID " = u." STATS_APP_ID \
		" GROUP BY i." STATS_APP_ID \
		" ORDER BY " STATS_TOTAL_COUNT " ASC" \
		" LIMIT " << limit;

	return query.str();
}

void ctx::AppDbHandle::replyTriggerItem(int error, ctx::Json &jsonResult)
{
	IF_FAIL_VOID_TAG(STR_EQ(__reqSubject.c_str(), APP_SUBJ_FREQUENCY), _E, "Invalid subject");

	ctx::Json results;
	std::string valStr;
	int val;

	jsonResult.get(NULL, STATS_APP_ID, &valStr);
	results.set(NULL, STATS_APP_ID, valStr);
	jsonResult.get(NULL, STATS_TOTAL_COUNT, &val);
	results.set(NULL, STATS_TOTAL_COUNT, val);
	jsonResult.get(NULL, STATS_RANK, &val);
	results.set(NULL, STATS_RANK, val);

	context_manager::replyToRead(__reqSubject.c_str(), __reqFilter, error, results);
}
