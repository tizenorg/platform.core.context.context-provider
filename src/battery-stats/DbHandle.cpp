/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd.
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
#include "BatteryStatisticsTypes.h"
#include "DbHandle.h"

using namespace ctx;

BatteryDbHandle::BatteryDbHandle(ContextProvider *provider) :
	StatsDbHandleBase(provider)
{
}

BatteryDbHandle::~BatteryDbHandle()
{
}

int BatteryDbHandle::read(Json filter)
{
	std::string query;
	const char *subject = reqProvider->getSubject();

	if (STR_EQ(subject, BATTERY_SUBJ_HISTORY)) {
		query = createSqlHistory(filter);

	} else if (STR_EQ(subject, BATTERY_SUBJ_SINCE_LAST_CHARGE)) {
		query = createSqlSinceLastCharge(filter);

	}

	IF_FAIL_RETURN(!query.empty(), ERR_OPERATION_FAILED);

	bool ret = executeQuery(filter, query.c_str());
	IF_FAIL_RETURN(ret, ERR_OPERATION_FAILED);

	return ERR_NONE;
}

std::string BatteryDbHandle::createWhereClauseWithDeviceStatus(Json filter)
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


std::string BatteryDbHandle::createSqlHistory(Json filter)
{
	std::stringstream query;
	int limit = DEFAULT_LIMIT;

	filter.get(NULL, KEY_RESULT_SIZE, &limit);

	// query <<
	// 	"SELECT " KEY_BATTERY_ID ", " \
	// 		"COUNT(*) AS " KEY_TOTAL_COUNT ", " \
	// 		"SUM(" KEY_DURATION ") AS " KEY_TOTAL_DURATION ", " \
	// 		"MAX(" KEY_UNIV_TIME ") AS " KEY_LAST_TIME \
	// 	" FROM " BATTERY_TABLE_USAGE_LOG \
	// 	" WHERE " << createWhereClauseWithDeviceStatus(filter) <<
	// 	" GROUP BY " KEY_BATTERY_ID \
	// 	" ORDER BY MAX(" KEY_UNIV_TIME ") DESC" \
	// 	" LIMIT " << limit;

	return query.str();
}

std::string BatteryDbHandle::createSqlSinceLastCharge(Json filter)
{
	std::stringstream query;
	int limit = DEFAULT_LIMIT;

	filter.get(NULL, KEY_RESULT_SIZE, &limit);

	// query <<
	// 	"SELECT " KEY_BATTERY_ID ", " \
	// 		"COUNT(*) AS " KEY_TOTAL_COUNT ", " \
	// 		"SUM(" KEY_DURATION ") AS " KEY_TOTAL_DURATION ", " \
	// 		"MAX(" KEY_UNIV_TIME ") AS " KEY_LAST_TIME \
	// 	" FROM " BATTERY_TABLE_USAGE_LOG \
	// 	" WHERE " << createWhereClauseWithDeviceStatus(filter) <<
	// 	" GROUP BY " KEY_BATTERY_ID \
	// 	" ORDER BY COUNT(*) DESC" \
	// 	" LIMIT " << limit;

	return query.str();
}


void BatteryDbHandle::replyTriggerItem(int error, Json &jsonResult)
{
	//IF_FAIL_VOID_TAG(STR_EQ(reqProvider->getSubject(), BATTERY_SUBJ_FREQUENCY), _E, "Invalid subject");

	// Json results;
	// std::string valStr;
	// int val;

	// jsonResult.get(NULL, KEY_BATTERY_ID, &valStr);
	// results.set(NULL, KEY_BATTERY_ID, valStr);
	// jsonResult.get(NULL, KEY_TOTAL_COUNT, &val);
	// results.set(NULL, KEY_TOTAL_COUNT, val);
	// jsonResult.get(NULL, KEY_RANK, &val);
	// results.set(NULL, KEY_RANK, val);

	// reqProvider->replyToRead(reqFilter, error, results);
}
