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
#include <ctime>
#include <string>
#include <memory>
#include <types_internal.h>
#include "inactive_detector.h"
#include "inactive_detector_storage.h"
#include "inactive_detector_storage_queries.h"
#include "inactive_detector_classificator.h"
#include "app_inactive_detector_types.h"

/*int ctx::inactive_detector_storage::create_table()
{
	bool ret = __dbManager.createTable(0, WIFI_TABLE_NAME, WIFI_CREATE_TABLE_COLUMNS, NULL, NULL);
	_D("Table Creation Request: %s", ret ? "SUCCESS" : "FAIL");
	return ret;
}*/



// expected Json format example: {timeframe: 1; is_active: 0}
int ctx::inactive_detector_storage::read(
			const char *subject,
			ctx::Json filter)
{
	std::string query;
	query = query_get_apps(subject, filter);

	IF_FAIL_RETURN(!query.empty(), ERR_OPERATION_FAILED);

	bool ret = __dbManager.execute(
		STR_EQ(subject, APP_INACTIVE_SUBJ_GET_APPS_INACTIVE) ?
			APP_INACTIVE_QUERY_ID_GET_APPS_INACTIVE :
			APP_INACTIVE_QUERY_ID_GET_APPS_ACTIVE,
		query.c_str(),
		this);

	IF_FAIL_RETURN(ret, ERR_OPERATION_FAILED);

	return ERR_NONE;
}

std::string ctx::inactive_detector_storage::query_get_apps(const char *subject, ctx::Json filter)
{
	double timeframe;
	int is_active;

	std::string query(GET_APP_INFO_INACTIVE_QUERY);
	std::string placeholder_timeframe(APP_INACTIVE_DETECTOR_VALUE_PLACEHOLDER_TIMEFRAME);
	std::string placeholder_is_active(APP_INACTIVE_DETECTOR_VALUE_PLACEHOLDER_CLUSTER);

	filter.get(NULL, APP_INACTIVE_DETECTOR_DATA_TIMEFRAME, &timeframe);
	filter.get(NULL, APP_INACTIVE_DETECTOR_DATA_ISACTIVE, &is_active);

	std::stringstream timeframe_stream;
	timeframe_stream << timeframe;

	std::stringstream is_active_stream;
	is_active_stream << is_active;

	inject_params(query, placeholder_timeframe, timeframe_stream.str());
	inject_params(query, placeholder_is_active, is_active_stream.str());

	return query;
}

std::string ctx::inactive_detector_storage::query_update_apps(std::vector<app_t> *apps_with_weights)
{
	std::string delete_query(DELETE_APP_ACTIVITY_CLASSIFIED_BY_TIMEFRAME);
	std::string insert_query(INSERT_APP_ACTIVITY_CLASSIFIED);
	std::string placeholder_timeframe(APP_INACTIVE_DETECTOR_VALUE_PLACEHOLDER_TIMEFRAME);
	std::string placeholder_values(APP_INACTIVE_DETECTOR_VALUE_PLACEHOLDER_VALUES);
	std::string placeholder_is_active(APP_INACTIVE_DETECTOR_VALUE_PLACEHOLDER_CLUSTER);

	std::stringstream timeframe_stream;
	timeframe_stream << apps_with_weights->front().timeframe;

	inject_params(delete_query, placeholder_timeframe, timeframe_stream.str());
	inject_params(insert_query, placeholder_values, subquery_form_values(apps_with_weights));

	std::stringstream result;
	result << delete_query << insert_query;
	return result.str();
}

// foreach app_info id+cluster -> select for insert
std::string ctx::inactive_detector_storage::subquery_form_values(std::vector<app_t> *apps_with_weights)
{
	std::stringstream select_elements;

	for(std::vector<app_t>::iterator row = apps_with_weights->begin();
			row != apps_with_weights->end(); row++)
	{
		//SELECT 1 as is_active, 1 as timeframe,  3964 as context_app_info_id
		std::stringstream select_element;
		select_element << " SELECT " << row->is_active << " as ";
		select_element << APP_INACTIVE_DETECTOR_ACTIVITYCLASSIFIED_COLUMN_IS_ACTIVE;
		select_element << ", " << row->timeframe << " as ";
		select_element << APP_INACTIVE_DETECTOR_ACTIVITYCLASSIFIED_COLUMN_TIMEFRAME;
		select_element << ", " << row->id << " as ";
		select_element << APP_INACTIVE_DETECTOR_ACTIVITYCLASSIFIED_COLUMN_CONTEXT_APP_INFO_ID;

		if ((row != apps_with_weights->end()) && (row == --apps_with_weights->end()))
			select_element << " UNION ";

		select_elements << select_element;
	}

	return select_elements.str();
}

void ctx::inactive_detector_storage::json_to_object(std::vector<Json>& records,
	std::vector<app_t> *apps_with_weights, bool result_mode)
{
	for(std::vector<Json>::iterator row = records.begin();
		row != records.end(); row++)
	{
		app_t app_with_weight;
		if (result_mode)
		{
			row->get(NULL, APP_INACTIVE_DETECTOR_APPINFO_COLUMN_PACKAGE_NAME,
				&app_with_weight.package_name);
			row->get(NULL, APP_INACTIVE_DETECTOR_APPINFO_COLUMN_IS_NODISPLAY,
				&app_with_weight.is_nodisplay);
			row->get(NULL, APP_INACTIVE_DETECTOR_APPINFO_COLUMN_IS_ENABLED,
				&app_with_weight.is_enabled);
			row->get(NULL, APP_INACTIVE_DETECTOR_APPINFO_COLUMN_IS_ATBOOT,
				&app_with_weight.is_atboot);
			row->get(NULL, APP_INACTIVE_DETECTOR_APPINFO_COLUMN_IS_PRELOADED,
				&app_with_weight.is_preloaded);
			row->get(NULL, APP_INACTIVE_DETECTOR_VIRTUAL_COLUMN_WEIGHT,
				&app_with_weight.weight);
		}
		else
		{
			row->get(NULL, APP_INACTIVE_DETECTOR_APPINFO_COLUMN_ID,
				&app_with_weight.id);
			row->get(NULL, APP_INACTIVE_DETECTOR_APPINFO_COLUMN_TIMESTAMP,
				&app_with_weight.timestamp);
			row->get(NULL, APP_INACTIVE_DETECTOR_VIRTUAL_COLUMN_WEIGHT,
				&app_with_weight.weight);
			row->get(NULL, APP_INACTIVE_DETECTOR_ACTIVITYCLASSIFIED_COLUMN_IS_ACTIVE,
				&app_with_weight.is_active);
		}

		apps_with_weights->push_back(app_with_weight);
	}
}

void ctx::inactive_detector_storage::onExecuted(unsigned int query_id,
	int error,
	std::vector<Json>& records)
{
	if (error != ERR_NONE) {
		_E("query_id:%d, error:%d", query_id, error);
		return;
	}

	std::vector<app_t> *apps_with_weights = NULL;
	if (query_id == APP_INACTIVE_QUERY_ID_GET_APPS_INACTIVE ||
		query_id == APP_INACTIVE_QUERY_ID_GET_APPS_ACTIVE)
	{
		json_to_object(records, apps_with_weights, TRUE);
	}
	else if (query_id == APP_INACTIVE_QUERY_ID_GET_APPS_WEIGHT) {

		json_to_object(records, apps_with_weights, FALSE);

		if (apps_with_weights->size() > 0) {
			inactive_detector_classificator i_detector_classificator;
			int _error = i_detector_classificator.classify(apps_with_weights);

			if(_error == ERR_NONE)
			{
				std::string query;
				query = query_update_apps(apps_with_weights);
				bool ret = __dbManager.execute(APP_INACTIVE_QUERY_ID_UPDATE_CLUSTERS,
					query.c_str(),
					this);
				_D("load visits execute query result: %s", ret ? "SUCCESS" : "FAIL");
			}
			else
			{
				_E("classification query_id:%d, error:%d",
					query_id, _error);
			}
		}
	}
	else if (query_id == APP_INACTIVE_QUERY_ID_UPDATE_CLUSTERS) {
		_D("UPDATE_CLUSTERS execute query id: %d", query_id);
	}
	else {
		_E("unknown query_id:%d", query_id);
	}
}

void ctx::inactive_detector_storage::inject_params(std::string& str, const std::string& from, const std::string& to)
{
	if(from.empty())
		return;
	size_t start_pos = 0;
	while((start_pos = str.find(from, start_pos)) != std::string::npos) {
		str.replace(start_pos, from.length(), to);
		start_pos += to.length();
	}
}

// normalzie weights
int ctx::inactive_detector_storage::update_ranks()
{
	return ERR_NONE;
}

int ctx::inactive_detector_storage::get_apps_info_w_weights(
		double timestamp_from)
{
	std::stringstream timestamp_stream;
	timestamp_stream << timestamp_from;
	std::string timestamp_str = timestamp_stream.str();

	std::string query(GET_APP_INFO_W_WEIGHT_QUERY);
	std::string placeholder(APP_INACTIVE_DETECTOR_VALUE_PLACEHOLDER_TIMESTAMP);

	inject_params(query, placeholder, timestamp_str);

	bool ret = __dbManager.execute(APP_INACTIVE_QUERY_ID_GET_APPS_WEIGHT,
		query.c_str(),
		this);
	_D("load visits execute query result: %s", ret ? "SUCCESS" : "FAIL");

	return ERR_NONE	;
}
