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
#include <ctime>
#include <string>
#include <memory>
#include <Types.h>
#include "InactiveDetector.h"
#include "InactiveDetectorStorage.h"
#include "InactiveDetectorStorageQueries.h"
#include "InactiveDetectorClassificator.h"
#include "AppInactiveDetectorTypes.h"

/*int ctx::InactiveDetectorStorage::create_table()
{
	bool ret = __dbManager.createTable(0, WIFI_TABLE_NAME, WIFI_CREATE_TABLE_COLUMNS, NULL, NULL);
	_D("Table Creation Request: %s", ret ? "SUCCESS" : "FAIL");
	return ret;
}*/

// expected Json format example: {timeframe: 1; isActive: 0}
int ctx::InactiveDetectorStorage::read(const char *subject, ctx::Json filter)
{
	std::string query;
	query = __getQueryGetApps(subject, filter);

	IF_FAIL_RETURN(!query.empty(), ERR_OPERATION_FAILED);

	bool ret = __dbManager.execute(STR_EQ(subject, APP_INACTIVE_SUBJ_GET_APPS_INACTIVE) ?
			APP_INACTIVE_QUERY_ID_GET_APPS_INACTIVE : APP_INACTIVE_QUERY_ID_GET_APPS_ACTIVE,
			query.c_str(), this);

	IF_FAIL_RETURN(ret, ERR_OPERATION_FAILED);

	return ERR_NONE;
}

std::string ctx::InactiveDetectorStorage::__getQueryGetApps(const char *subject, ctx::Json filter)
{
	double timeframe;
	int isActive;

	std::string query(GET_APP_INFO_INACTIVE_QUERY);
	std::string placeholderTimeframe(APP_INACTIVE_DETECTOR_VALUE_PLACEHOLDER_TIMEFRAME);
	std::string placeholderIsActive(APP_INACTIVE_DETECTOR_VALUE_PLACEHOLDER_CLUSTER);

	filter.get(NULL, APP_INACTIVE_DETECTOR_DATA_TIMEFRAME, &timeframe);
	filter.get(NULL, APP_INACTIVE_DETECTOR_DATA_ISACTIVE, &isActive);

	std::stringstream timeframeStream;
	timeframeStream << timeframe;

	std::stringstream isActiveStream;
	isActiveStream << isActive;

	__injectParams(query, placeholderTimeframe, timeframeStream.str());
	__injectParams(query, placeholderIsActive, isActiveStream.str());

	return query;
}

std::string ctx::InactiveDetectorStorage::__getQueryUpdateApps(std::vector<AppInfo> *appsWithWeights)
{
	std::string deleteQuery(DELETE_APP_ACTIVITY_CLASSIFIED_BY_TIMEFRAME);
	std::string insertQuery(INSERT_APP_ACTIVITY_CLASSIFIED);
	std::string placeholderTimeframe(APP_INACTIVE_DETECTOR_VALUE_PLACEHOLDER_TIMEFRAME);
	std::string placeholderValues(APP_INACTIVE_DETECTOR_VALUE_PLACEHOLDER_VALUES);
	std::string placeholderIsActive(APP_INACTIVE_DETECTOR_VALUE_PLACEHOLDER_CLUSTER);

	std::stringstream timeframeStream;
	timeframeStream << appsWithWeights->front().timeframe;

	__injectParams(deleteQuery, placeholderTimeframe, timeframeStream.str());
	__injectParams(insertQuery, placeholderValues, __getSubqueryFormValues(appsWithWeights));

	std::stringstream result;
	result << deleteQuery << insertQuery;
	return result.str();
}

// foreach app_info id+cluster -> select for insert
std::string ctx::InactiveDetectorStorage::__getSubqueryFormValues(std::vector<AppInfo> *appsWithWeights)
{
	std::stringstream selectElements;

	for (auto row = appsWithWeights->begin(); row != appsWithWeights->end(); row++)
	{
		//SELECT 1 as isActive, 1 as timeframe,  3964 as context_app_info_id
		std::stringstream selectElement;
		selectElement << " SELECT " << row->isActive << " as ";
		selectElement << APP_INACTIVE_DETECTOR_ACTIVITYCLASSIFIED_COLUMN_IS_ACTIVE;
		selectElement << ", " << row->timeframe << " as ";
		selectElement << APP_INACTIVE_DETECTOR_ACTIVITYCLASSIFIED_COLUMN_TIMEFRAME;
		selectElement << ", " << row->id << " as ";
		selectElement << APP_INACTIVE_DETECTOR_ACTIVITYCLASSIFIED_COLUMN_CONTEXT_APP_INFO_ID;

		if ((row != appsWithWeights->end()) && (row == --appsWithWeights->end()))
			selectElement << " UNION ";

		selectElements << selectElement;
	}

	return selectElements.str();
}

void ctx::InactiveDetectorStorage::__jsonToObject(std::vector<Json>& records, std::vector<AppInfo> *appsWithWeights, bool resultMode)
{
	for (auto row = records.begin(); row != records.end(); row++) {
		AppInfo appWithWeight;
		if (resultMode) {
			row->get(NULL, APP_INACTIVE_DETECTOR_APPINFO_COLUMN_PACKAGE_NAME, &appWithWeight.packageName);
			row->get(NULL, APP_INACTIVE_DETECTOR_APPINFO_COLUMN_IS_NODISPLAY, &appWithWeight.isNodisplay);
			row->get(NULL, APP_INACTIVE_DETECTOR_APPINFO_COLUMN_IS_ENABLED, &appWithWeight.isEnabled);
			row->get(NULL, APP_INACTIVE_DETECTOR_APPINFO_COLUMN_IS_ATBOOT, &appWithWeight.isAtBoot);
			row->get(NULL, APP_INACTIVE_DETECTOR_APPINFO_COLUMN_IS_PRELOADED, &appWithWeight.isPreloaded);
			row->get(NULL, APP_INACTIVE_DETECTOR_VIRTUAL_COLUMN_WEIGHT, &appWithWeight.weight);
		} else {
			row->get(NULL, APP_INACTIVE_DETECTOR_APPINFO_COLUMN_ID, &appWithWeight.id);
			row->get(NULL, APP_INACTIVE_DETECTOR_APPINFO_COLUMN_TIMESTAMP, &appWithWeight.timestamp);
			row->get(NULL, APP_INACTIVE_DETECTOR_VIRTUAL_COLUMN_WEIGHT, &appWithWeight.weight);
			row->get(NULL, APP_INACTIVE_DETECTOR_ACTIVITYCLASSIFIED_COLUMN_IS_ACTIVE, &appWithWeight.isActive);
		}

		appsWithWeights->push_back(appWithWeight);
	}
}

void ctx::InactiveDetectorStorage::onExecuted(unsigned int queryId, int error, std::vector<Json>& records)
{
	if (error != ERR_NONE) {
		_E("queryId:%d, error:%d", queryId, error);
		return;
	}

	std::vector<AppInfo> *appsWithWeights = NULL;
	if (queryId == APP_INACTIVE_QUERY_ID_GET_APPS_INACTIVE || queryId == APP_INACTIVE_QUERY_ID_GET_APPS_ACTIVE) {
		__jsonToObject(records, appsWithWeights, TRUE);
	} else if (queryId == APP_INACTIVE_QUERY_ID_GET_APPS_WEIGHT) {
		__jsonToObject(records, appsWithWeights, FALSE);

		if (appsWithWeights->size() > 0) {
			InactiveDetectorClassificator inactDetClassificator;
			int err = inactDetClassificator.classify(appsWithWeights);

			if (err == ERR_NONE) {
				std::string query;
				query = __getQueryUpdateApps(appsWithWeights);
				bool ret = __dbManager.execute(APP_INACTIVE_QUERY_ID_UPDATE_CLUSTERS, query.c_str(), this);
				_D("load visits execute query result: %s", ret ? "SUCCESS" : "FAIL");
			} else {
				_E("classification queryId:%d, error:%d", queryId, err);
			}
		}
	} else if (queryId == APP_INACTIVE_QUERY_ID_UPDATE_CLUSTERS) {
		_D("UPDATE_CLUSTERS execute query id: %d", queryId);
	} else {
		_E("unknown queryId:%d", queryId);
	}
}

void ctx::InactiveDetectorStorage::__injectParams(std::string& str, const std::string& from, const std::string& to)
{
	if (from.empty())
		return;

	size_t startPos = 0;
	while((startPos = str.find(from, startPos)) != std::string::npos) {
		str.replace(startPos, from.length(), to);
		startPos += to.length();
	}
}

// normalzie weights
int ctx::InactiveDetectorStorage::updateRanks()
{
	return ERR_NONE;
}

int ctx::InactiveDetectorStorage::getAppsInfoWithWeights(double timestampFrom)
{
	std::stringstream timestampStream;
	timestampStream << timestampFrom;
	std::string timestampStr = timestampStream.str();

	std::string query(GET_APP_INFO_W_WEIGHT_QUERY);
	std::string placeholder(APP_INACTIVE_DETECTOR_VALUE_PLACEHOLDER_TIMESTAMP);

	__injectParams(query, placeholder, timestampStr);

	bool ret = __dbManager.execute(APP_INACTIVE_QUERY_ID_GET_APPS_WEIGHT, query.c_str(), this);
	_D("load visits execute query result: %s", ret ? "SUCCESS" : "FAIL");

	return ERR_NONE	;
}
