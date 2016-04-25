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
#include <Types.h>
#include "AppStatsTypesInternal.h"
#include "DbHandle.h"

using namespace ctx;

AppDbHandle::AppDbHandle(ContextProvider *provider) :
	StatsDbHandleBase(provider)
{
}

AppDbHandle::~AppDbHandle()
{
}

int AppDbHandle::read(Json filter)
{
	std::string query;
	const char *subject = reqProvider->getSubject();

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
		isTriggerItem = true;
		query = createSqlFrequency(filter);
	}

	IF_FAIL_RETURN(!query.empty(), ERR_OPERATION_FAILED);

	bool ret = executeQuery(filter, query.c_str());
	IF_FAIL_RETURN(ret, ERR_OPERATION_FAILED);

	return ERR_NONE;
}

std::string AppDbHandle::createWhereClauseWithDeviceStatus(Json filter)
{
	std::stringstream whereClause;
	std::string bssid;
	int audioJack;

	whereClause << StatsDbHandleBase::createWhereClause(filter);

	if (filter.get(NULL, KEY_BSSID, &bssid))
		whereClause << " AND " KEY_BSSID " = '" << bssid << "'";

	if (filter.get(NULL, KEY_AUDIO_JACK, &audioJack))
		whereClause << " AND " KEY_AUDIO_JACK " = " << audioJack;

	return whereClause.str();
}

std::string AppDbHandle::createSqlPeakTime(Json filter)
{
	return StatsDbHandleBase::createSqlPeakTime(filter, APP_TABLE_USAGE_LOG, createWhereClause(filter));
}

std::string AppDbHandle::createSqlCommonSetting(Json filter)
{
	return StatsDbHandleBase::createSqlCommonSetting(filter, APP_TABLE_USAGE_LOG, createWhereClause(filter));
}

std::string AppDbHandle::createSqlFrequency(Json filter)
{
	Json filterCleaned;
	std::string weekStr;
	std::string timeOfDay;
	std::string appId;

	if (!filter.get(NULL, KEY_APP_ID, &appId)) {
		_E("Invalid parameter");
		return "";
	}

	if (filter.get(NULL, KEY_DAY_OF_WEEK, &weekStr))
		filterCleaned.set(NULL, KEY_DAY_OF_WEEK, weekStr);

	if (filter.get(NULL, KEY_TIME_OF_DAY, &timeOfDay))
		filterCleaned.set(NULL, KEY_TIME_OF_DAY, timeOfDay);

	std::string whereClause = createWhereClause(filterCleaned);

	std::stringstream query;

	query <<
		"DELETE FROM " APP_TEMP_USAGE_FREQ ";";

	query <<
		"INSERT INTO " APP_TEMP_USAGE_FREQ \
		" SELECT " KEY_APP_ID ", COUNT(*) AS " KEY_TOTAL_COUNT \
		" FROM " APP_TABLE_USAGE_LOG \
		" WHERE " << whereClause <<
		" GROUP BY " KEY_APP_ID ";";

	query <<
		"INSERT OR IGNORE INTO " APP_TEMP_USAGE_FREQ " (" KEY_APP_ID ")" \
		" VALUES ('" << appId << "');";

	query <<
		"SELECT S." KEY_APP_ID ", S." KEY_TOTAL_COUNT ", 1+COUNT(lesser." KEY_TOTAL_COUNT ") AS " KEY_RANK \
		" FROM " APP_TEMP_USAGE_FREQ " AS S" \
		" LEFT JOIN " APP_TEMP_USAGE_FREQ " AS lesser" \
		" ON S." KEY_TOTAL_COUNT " < lesser." KEY_TOTAL_COUNT \
		" WHERE S." KEY_APP_ID " = '" << appId << "'";

	return query.str();
}

std::string AppDbHandle::createSqlRecentlyUsed(Json filter)
{
	std::stringstream query;
	int limit = DEFAULT_LIMIT;

	filter.get(NULL, KEY_RESULT_SIZE, &limit);

	query <<
		"SELECT " KEY_APP_ID ", " \
			"COUNT(*) AS " KEY_TOTAL_COUNT ", " \
			"SUM(" KEY_DURATION ") AS " KEY_TOTAL_DURATION ", " \
			"MAX(" KEY_UNIV_TIME ") AS " KEY_LAST_TIME \
		" FROM " APP_TABLE_USAGE_LOG \
		" WHERE " << createWhereClauseWithDeviceStatus(filter) <<
		" GROUP BY " KEY_APP_ID \
		" ORDER BY MAX(" KEY_UNIV_TIME ") DESC" \
		" LIMIT " << limit;

	return query.str();
}

std::string AppDbHandle::createSqlFrequentlyUsed(Json filter)
{
	std::stringstream query;
	int limit = DEFAULT_LIMIT;

	filter.get(NULL, KEY_RESULT_SIZE, &limit);

	query <<
		"SELECT " KEY_APP_ID ", " \
			"COUNT(*) AS " KEY_TOTAL_COUNT ", " \
			"SUM(" KEY_DURATION ") AS " KEY_TOTAL_DURATION ", " \
			"MAX(" KEY_UNIV_TIME ") AS " KEY_LAST_TIME \
		" FROM " APP_TABLE_USAGE_LOG \
		" WHERE " << createWhereClauseWithDeviceStatus(filter) <<
		" GROUP BY " KEY_APP_ID \
		" ORDER BY COUNT(*) DESC" \
		" LIMIT " << limit;

	return query.str();
}

std::string AppDbHandle::createSqlRarelyUsed(Json filter)
{
	std::stringstream query;
	int limit = DEFAULT_LIMIT;

	filter.get(NULL, KEY_RESULT_SIZE, &limit);

	query <<
		"SELECT i." KEY_APP_ID ", " \
			"COUNT(u." KEY_DURATION ") AS " KEY_TOTAL_COUNT ", " \
			"IFNULL(SUM(u." KEY_DURATION "),0) AS " KEY_TOTAL_DURATION ", " \
			"IFNULL(MAX(u." KEY_UNIV_TIME "),-1) AS " KEY_LAST_TIME \
		" FROM " APP_TABLE_REMOVABLE_APP " i LEFT OUTER JOIN (" \
			" SELECT * FROM " APP_TABLE_USAGE_LOG \
			" WHERE " << createWhereClauseWithDeviceStatus(filter) << ") u" \
			" ON i." KEY_APP_ID " = u." KEY_APP_ID \
		" GROUP BY i." KEY_APP_ID \
		" ORDER BY " KEY_TOTAL_COUNT " ASC" \
		" LIMIT " << limit;

	return query.str();
}

void AppDbHandle::replyTriggerItem(int error, Json &jsonResult)
{
	IF_FAIL_VOID_TAG(STR_EQ(reqProvider->getSubject(), APP_SUBJ_FREQUENCY), _E, "Invalid subject");

	Json results;
	std::string valStr;
	int val;

	jsonResult.get(NULL, KEY_APP_ID, &valStr);
	results.set(NULL, KEY_APP_ID, valStr);
	jsonResult.get(NULL, KEY_TOTAL_COUNT, &val);
	results.set(NULL, KEY_TOTAL_COUNT, val);
	jsonResult.get(NULL, KEY_RANK, &val);
	results.set(NULL, KEY_RANK, val);

	reqProvider->replyToRead(reqFilter, error, results);
}
