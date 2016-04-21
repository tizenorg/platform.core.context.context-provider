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
#include "ProviderTypes.h"
#include "DbHandleBase.h"

#define DAY_OF_WEEK(SECOND) "CAST(strftime('%w', " SECOND ", 'unixepoch') AS INTEGER)"
#define HOUR_OF_DAY(SECOND) "CAST(strftime('%H', " SECOND ", 'unixepoch') AS INTEGER)"

/* TODO: replace these macros */
#define STATS_SUN		"Sun"
#define STATS_MON		"Mon"
#define STATS_TUE		"Tue"
#define STATS_WED		"Wed"
#define STATS_THU		"Thu"
#define STATS_FRI		"Fri"
#define STATS_SAT		"Sat"
#define STATS_WEEKDAY	"Weekday"
#define STATS_WEEKEND	"Weekend"

enum StatsDayOfWeek_e {
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


using namespace ctx;

StatsDbHandleBase::StatsDbHandleBase(ContextProvider *provider) :
	isTriggerItem(false),
	reqProvider(provider)
{
}

StatsDbHandleBase::~StatsDbHandleBase()
{
}

int StatsDbHandleBase::generateQid()
{
	static int qid = 0;

	if (qid++ < 0) qid = 1;
	return qid;
}

bool StatsDbHandleBase::executeQuery(Json filter, const char* query)
{
	bool ret = __dbManager.execute(generateQid(), query, this);
	IF_FAIL_RETURN(ret, false);

	reqFilter = filter;

	return true;
}

std::string StatsDbHandleBase::createWhereClause(Json filter)
{
	std::stringstream whereClause;
	int week = 0;
	int start = 0;
	int end = 0;
	int timespan = DEFAULT_TIMESPAN;
	std::string appId;
	std::string weekStr;
	std::string timeOfDay;

	if (filter.get(NULL, KEY_DAY_OF_WEEK, &weekStr)) {
		// In case of string (from Trigger)
		if (weekStr == STATS_WEEKDAY) {
			week = STATS_DAY_OF_WEEK_WEEKDAY;

		} else if (weekStr == STATS_WEEKEND) {
			week = STATS_DAY_OF_WEEK_WEEKEND;

		} else if (weekStr == STATS_SUN) {
			week = STATS_DAY_OF_WEEK_SUN;

		} else if (weekStr == STATS_MON) {
			week = STATS_DAY_OF_WEEK_MON;

		} else if (weekStr == STATS_TUE) {
			week = STATS_DAY_OF_WEEK_TUE;

		} else if (weekStr == STATS_WED) {
			week = STATS_DAY_OF_WEEK_WED;

		} else if (weekStr == STATS_THU) {
			week = STATS_DAY_OF_WEEK_THU;

		} else if (weekStr == STATS_FRI) {
			week = STATS_DAY_OF_WEEK_FRI;

		} else if (weekStr == STATS_SAT) {
			week = STATS_DAY_OF_WEEK_SAT;
		}
	} else {
		// In case of integer (from History)
		filter.get(NULL, KEY_DAY_OF_WEEK, &week);
	}

	switch(week) {
	case STATS_DAY_OF_WEEK_WEEKDAY:
		whereClause << "(" DAY_OF_WEEK(KEY_LOCAL_TIME) " > 0 AND " DAY_OF_WEEK(KEY_LOCAL_TIME) " < 6) AND ";
		break;
	case STATS_DAY_OF_WEEK_WEEKEND:
		whereClause << "(" DAY_OF_WEEK(KEY_LOCAL_TIME) " = 0 OR " DAY_OF_WEEK(KEY_LOCAL_TIME) " = 6) AND ";
		break;
	case STATS_DAY_OF_WEEK_SUN:
	case STATS_DAY_OF_WEEK_MON:
	case STATS_DAY_OF_WEEK_TUE:
	case STATS_DAY_OF_WEEK_WED:
	case STATS_DAY_OF_WEEK_THU:
	case STATS_DAY_OF_WEEK_FRI:
	case STATS_DAY_OF_WEEK_SAT:
		whereClause << DAY_OF_WEEK(KEY_LOCAL_TIME) " = " << week - STATS_DAY_OF_WEEK_SUN << " AND ";
		break;
	default:
		break;
	}

	if (filter.get(NULL, KEY_APP_ID, &appId))
		whereClause << KEY_APP_ID " = '" << appId << "' AND ";

	if (filter.get(NULL, KEY_START_TIME, &start))
		whereClause << KEY_UNIV_TIME " >= " << start << " AND ";

	if (filter.get(NULL, KEY_END_TIME, &end))
		whereClause << KEY_UNIV_TIME " <= " << end << " AND ";

	if (filter.get(NULL, KEY_TIME_OF_DAY, &timeOfDay)) {
		size_t pivot = timeOfDay.find('-');
		if (pivot != std::string::npos) {
			std::string from = timeOfDay.substr(0, pivot);
			std::string to = timeOfDay.substr(pivot + 1);
			whereClause << "(" HOUR_OF_DAY(KEY_LOCAL_TIME) " >= " << from \
				<< " AND " HOUR_OF_DAY(KEY_LOCAL_TIME) " < " << to << ") AND ";
		}
	}

	filter.get(NULL, KEY_TIMESPAN, &timespan);
	whereClause << KEY_UNIV_TIME " > strftime('%s', 'now', '-" << timespan <<" day')";

	return whereClause.str();
}

std::string StatsDbHandleBase::createSqlPeakTime(Json filter, const char* tableName, std::string whereClause)
{
	std::stringstream query;
	int limit = DEFAULT_LIMIT;

	filter.get(NULL, KEY_RESULT_SIZE, &limit);

	query <<
		"SELECT " \
			HOUR_OF_DAY(KEY_LOCAL_TIME) " AS " KEY_HOUR_OF_DAY ", COUNT(*) AS " KEY_TOTAL_COUNT \
		" FROM " << tableName << \
		" WHERE " << whereClause << \
		" GROUP BY " HOUR_OF_DAY(KEY_LOCAL_TIME) \
		" ORDER BY " KEY_TOTAL_COUNT " DESC" \
		" LIMIT " << limit;

	return query.str();
}

std::string StatsDbHandleBase::createSqlCommonSetting(Json filter, const char* tableName, std::string whereClause)
{
	std::stringstream query;

	query <<
		"SELECT ( SELECT " KEY_AUDIO_JACK \
				" FROM " << tableName << \
				" WHERE " << whereClause << \
				" GROUP BY " KEY_AUDIO_JACK \
				" ORDER BY count(" KEY_AUDIO_JACK ") DESC" \
				" LIMIT 1 ) AS " KEY_AUDIO_JACK \
			", ( SELECT " KEY_SYSTEM_VOLUME \
				" FROM " << tableName << \
				" WHERE " << whereClause << \
				" GROUP BY " KEY_SYSTEM_VOLUME \
				" ORDER BY count(" KEY_SYSTEM_VOLUME ") DESC" \
				" LIMIT 1 ) AS " KEY_SYSTEM_VOLUME \
			", ( SELECT " KEY_MEDIA_VOLUME \
				" FROM " << tableName << \
				" WHERE " << whereClause << \
				" GROUP BY " KEY_MEDIA_VOLUME \
				" ORDER BY count(" KEY_MEDIA_VOLUME ") DESC" \
				" LIMIT 1 ) AS " KEY_MEDIA_VOLUME;

	return query.str();
}

void StatsDbHandleBase::onTableCreated(unsigned int queryId, int error)
{
}

void StatsDbHandleBase::onInserted(unsigned int queryId, int error, int64_t rowId)
{
	delete this;
}

void StatsDbHandleBase::__jsonVectorToArray(std::vector<Json> &vecJson, Json &jsonResult)
{
	std::vector<Json>::iterator vecJsonEnd = vecJson.end();

	for(auto vecJsonPos = vecJson.begin(); vecJsonPos != vecJsonEnd; ++vecJsonPos) {
		Json originJson = *vecJsonPos;
		jsonResult.append(NULL, KEY_QUERY_RESULT, originJson);
	}
}

void StatsDbHandleBase::onExecuted(unsigned int queryId, int error, std::vector<Json>& records)
{
	if (isTriggerItem) {
		if (records.size() == 1) {
			replyTriggerItem(error, records[0]);
		} else {
			_E("Invalid query result");
			Json dummy;
			reqProvider->replyToRead(reqFilter, ERR_OPERATION_FAILED, dummy);
		}
	} else {
		Json results = "{\"" KEY_QUERY_RESULT "\":[]}";
		__jsonVectorToArray(records, results);
		reqProvider->replyToRead(reqFilter, error, results);
	}

	delete this;
}
