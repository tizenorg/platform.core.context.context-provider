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
#include <Types.h>
#include "SocialStatsTypesInternal.h"
#include "DbHandle.h"

using namespace ctx;

SocialDbHandle::SocialDbHandle(ContextProvider *provider) :
	StatsDbHandleBase(provider)
{
}

SocialDbHandle::~SocialDbHandle()
{
}

int SocialDbHandle::read(Json filter)
{
	std::string query;
	const char *subject = reqProvider->getSubject();

	if (STR_EQ(subject, SOCIAL_SUBJ_FREQ_ADDRESS)) {
		query = createSqlFreqAddress(filter);

	} else if (STR_EQ(subject, SOCIAL_SUBJ_FREQUENCY)) {
		isTriggerItem = true;
		query = createSqlFrequency(filter);
	}

	IF_FAIL_RETURN(!query.empty(), ERR_OPERATION_FAILED);

	bool ret = executeQuery(filter, query.c_str());
	IF_FAIL_RETURN(ret, ERR_OPERATION_FAILED);

	return ERR_NONE;
}

std::string SocialDbHandle::createWhereClause(Json filter)
{
	std::stringstream whereClause;
	int commType = -1;

	whereClause << StatsDbHandleBase::createWhereClause(filter);

	filter.get(NULL, SOCIAL_COMMUNICATION_TYPE, &commType);

	switch(commType) {
	case SOCIAL_COMMUNICATION_TYPE_CALL:
		whereClause <<
			" AND " SOCIAL_PHONE_LOG_TYPE " >= " << CONTACTS_PLOG_TYPE_VOICE_INCOMING <<
			" AND " SOCIAL_PHONE_LOG_TYPE " <= " << CONTACTS_PLOG_TYPE_VIDEO_BLOCKED;
		break;
	case SOCIAL_COMMUNICATION_TYPE_MESSAGE:
		whereClause <<
			" AND " SOCIAL_PHONE_LOG_TYPE " >= " << CONTACTS_PLOG_TYPE_MMS_INCOMING <<
			" AND " SOCIAL_PHONE_LOG_TYPE " <= " << CONTACTS_PLOG_TYPE_MMS_BLOCKED;
		break;
	default:
		break;
	}

	return whereClause.str();
}

std::string SocialDbHandle::createSqlFreqAddress(Json filter)
{
	std::stringstream query;
	int limit = DEFAULT_LIMIT;

	filter.get(NULL, KEY_RESULT_SIZE, &limit);

	query <<
		"SELECT " SOCIAL_ADDRESS ", " \
			"COUNT(*) AS " KEY_TOTAL_COUNT ", " \
			"SUM(" KEY_DURATION ") AS " KEY_TOTAL_DURATION ", " \
			"MAX(" KEY_UNIV_TIME ") AS " KEY_LAST_TIME \
		" FROM " SOCIAL_TABLE_CONTACT_LOG \
		" WHERE " << createWhereClause(filter) <<
		" GROUP BY " SOCIAL_ADDRESS \
		" ORDER BY COUNT(*) DESC" \
		" LIMIT " << limit;

	return query.str();
}

std::string SocialDbHandle::createSqlFrequency(Json filter)
{
	Json filterCleaned;
	std::string weekStr;
	std::string timeOfDay;
	std::string address;

	if (!filter.get(NULL, SOCIAL_ADDRESS, &address)) {
		_E("Invalid parameter");
		return "";
	}

	if (filter.get(NULL, KEY_DAY_OF_WEEK, &weekStr))
		filterCleaned.set(NULL, KEY_DAY_OF_WEEK, weekStr);

	if (filter.get(NULL, KEY_TIME_OF_DAY, &timeOfDay))
		filterCleaned.set(NULL, KEY_TIME_OF_DAY, timeOfDay);

	std::stringstream query;

	query <<
		"DELETE FROM " SOCIAL_TEMP_CONTACT_FREQ ";";

	query <<
		"INSERT INTO " SOCIAL_TEMP_CONTACT_FREQ \
		" SELECT " SOCIAL_ADDRESS ", COUNT(*) AS " KEY_TOTAL_COUNT \
		" FROM " SOCIAL_TABLE_CONTACT_LOG \
		" WHERE " << createWhereClause(filterCleaned) <<
		" GROUP BY " SOCIAL_ADDRESS ";";

	query <<
		"INSERT OR IGNORE INTO " SOCIAL_TEMP_CONTACT_FREQ " (" SOCIAL_ADDRESS ")" \
		" VALUES ('" << address << "');";

	query <<
		"SELECT S." SOCIAL_ADDRESS ", S." KEY_TOTAL_COUNT ", 1+COUNT(lesser." KEY_TOTAL_COUNT ") AS " KEY_RANK \
		" FROM " SOCIAL_TEMP_CONTACT_FREQ " AS S" \
		" LEFT JOIN " SOCIAL_TEMP_CONTACT_FREQ " AS lesser" \
		" ON S." KEY_TOTAL_COUNT " < lesser." KEY_TOTAL_COUNT \
		" WHERE S." SOCIAL_ADDRESS " = '" << address << "'";


	return query.str();
}

void SocialDbHandle::replyTriggerItem(int error, Json &jsonResult)
{
	IF_FAIL_VOID_TAG(STR_EQ(reqProvider->getSubject(), SOCIAL_SUBJ_FREQUENCY), _E, "Invalid subject");

	Json results;
	std::string valStr;
	int val;

	jsonResult.get(NULL, SOCIAL_ADDRESS, &valStr);
	results.set(NULL, SOCIAL_ADDRESS, valStr);
	jsonResult.get(NULL, KEY_TOTAL_COUNT, &val);
	results.set(NULL, KEY_TOTAL_COUNT, val);
	jsonResult.get(NULL, KEY_RANK, &val);
	results.set(NULL, KEY_RANK, val);

	reqProvider->replyToRead(reqFilter, error, results);
}
