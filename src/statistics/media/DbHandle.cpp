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
#include <db_mgr.h>
#include "../shared/SystemInfo.h"
#include "MediaStatisticsTypes.h"
#include "DbHandle.h"

ctx::MediaDbHandle::MediaDbHandle()
{
}

ctx::MediaDbHandle::~MediaDbHandle()
{
}

int ctx::MediaDbHandle::read(const char* subject, ctx::Json filter)
{
	//TODO: filter validation (in the API side?)
	std::string query;

	if (STR_EQ(subject, MEDIA_SUBJ_PEAK_TIME_FOR_MUSIC)) {
		query = createSqlPeakTime(MEDIA_TYPE_MUSIC, filter);

	} else if (STR_EQ(subject, MEDIA_SUBJ_PEAK_TIME_FOR_VIDEO)) {
		query = createSqlPeakTime(MEDIA_TYPE_VIDEO, filter);

	} else if (STR_EQ(subject, MEDIA_SUBJ_COMMON_SETTING_FOR_MUSIC)) {
		query = createSqlCommonSetting(MEDIA_TYPE_MUSIC, filter);

	} else if (STR_EQ(subject, MEDIA_SUBJ_COMMON_SETTING_FOR_VIDEO)) {
		query = createSqlCommonSetting(MEDIA_TYPE_VIDEO, filter);

	} else if (STR_EQ(subject, MEDIA_SUBJ_MUSIC_FREQUENCY)) {
		__isTriggerItem = true;
		query = createSqlFrequency(MEDIA_TYPE_MUSIC, filter);

	} else if (STR_EQ(subject, MEDIA_SUBJ_VIDEO_FREQUENCY)) {
		__isTriggerItem = true;
		query = createSqlFrequency(MEDIA_TYPE_VIDEO, filter);
	}

	IF_FAIL_RETURN(!query.empty(), ERR_OPERATION_FAILED);

	bool ret = executeQuery(subject, filter, query.c_str());
	IF_FAIL_RETURN(ret, ERR_OPERATION_FAILED);

	return ERR_NONE;
}

std::string ctx::MediaDbHandle::createWhereClause(int mediaType, ctx::Json filter)
{
	std::stringstream whereClause;

	whereClause << CX_MEDIA_TYPE " = " << mediaType << " AND ";
	whereClause << StatsDbHandleBase::createWhereClause(filter);

	return whereClause.str();
}

std::string ctx::MediaDbHandle::createSqlPeakTime(int mediaType, ctx::Json filter)
{
	std::string where = createWhereClause(mediaType, filter);
	return StatsDbHandleBase::createSqlPeakTime(filter, MEDIA_TABLE_NAME, where);
}

std::string ctx::MediaDbHandle::createSqlCommonSetting(int mediaType, ctx::Json filter)
{
	std::string where = createWhereClause(mediaType, filter);
	return StatsDbHandleBase::createSqlCommonSetting(filter, MEDIA_TABLE_NAME, where);
}

std::string ctx::MediaDbHandle::createSqlFrequency(int mediaType, ctx::Json filter)
{
	ctx::Json filterCleaned;
	std::string weekStr;
	std::string timeOfDay;

	if (filter.get(NULL, STATS_DAY_OF_WEEK, &weekStr))
		filterCleaned.set(NULL, STATS_DAY_OF_WEEK, weekStr);

	if (filter.get(NULL, STATS_TIME_OF_DAY, &timeOfDay))
		filterCleaned.set(NULL, STATS_TIME_OF_DAY, timeOfDay);

	std::string whereClause = createWhereClause(mediaType, filterCleaned);

	std::stringstream query;
	query <<
		"SELECT IFNULL(COUNT(*),0) AS " STATS_TOTAL_COUNT \
		" FROM " MEDIA_TABLE_NAME \
		" WHERE " << whereClause;

	return query.str();
}

void ctx::MediaDbHandle::replyTriggerItem(int error, ctx::Json &jsonResult)
{
	IF_FAIL_VOID_TAG(STR_EQ(__reqSubject.c_str(), MEDIA_SUBJ_MUSIC_FREQUENCY) ||
		STR_EQ(__reqSubject.c_str(), MEDIA_SUBJ_VIDEO_FREQUENCY), _E, "Invalid subject");

	ctx::Json results;
	int val;

	jsonResult.get(NULL, STATS_TOTAL_COUNT, &val);
	results.set(NULL, STATS_TOTAL_COUNT, val);

	context_manager::replyToRead(__reqSubject.c_str(), __reqFilter, error, results);
}
