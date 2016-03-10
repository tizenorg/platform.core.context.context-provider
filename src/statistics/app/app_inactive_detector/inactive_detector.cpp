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

#include <ctime>
#include <types_internal.h>
#include "inactive_detector.h"
#include "inactive_detector_weight.h"
#include "inactive_detector_storage.h"
#include "inactive_detector_classificator.h"
#include "timer_mgr.h"

ctx::inactive_detector::inactive_detector()
{
	time_t now = std::time(NULL);
	timer_id = timer_manager::set_at( // execute once every night
					APP_INACTIVE_TASK_START_HOUR,
					APP_INACTIVE_TASK_START_MINUTE,
					timer_manager::EVERYDAY,
					this);
	if (timer_id < 0) {
		_E("inactive_detector timer set FAIL");
		return;
	} else {
		update_clusters();
		_D("inactive_detector timer set SUCCESS");
	}
}

ctx::inactive_detector::~inactive_detector()
{
	if (timer_id >= 0) {
		timer_manager::remove(timer_id);
		_D("inactive_detector timer removed");
	}
}

int ctx::inactive_detector::update_clusters()
{
	// define timeframes stack
	std::vector<int> timeframes;
	timeframes.push_back(CONTEXT_HISTORY_FILTER_TIME_INTERVAL_ONEDAY);
	timeframes.push_back(CONTEXT_HISTORY_FILTER_TIME_INTERVAL_THREEDAYS);
	timeframes.push_back(CONTEXT_HISTORY_FILTER_TIME_INTERVAL_ONEWEEK);
	timeframes.push_back(CONTEXT_HISTORY_FILTER_TIME_INTERVAL_TWOWEEKS);
	timeframes.push_back(CONTEXT_HISTORY_FILTER_TIME_INTERVAL_ONEMONTH);

	// recalculate weights
	// foreach timeframe
	for(std::vector<int>::iterator timeframe = timeframes.begin();
		timeframe != timeframes.end(); timeframe++)
	{
		inactive_detector_weight i_detector_weight;
		int error = i_detector_weight.request_weights(*timeframe);
		_E("update_clusters request_weights:%d, error:%d", *timeframe, error);
	}

	return ERR_NONE;
}

void ctx::inactive_detector::on_cluster_update_finished(std::vector<app_t> *apps_with_weight, int error)
{
	if (error == ERR_NONE) {
		_I("on_cluster_update_finished success");
	}
	// clean memory
	delete apps_with_weight;
}

int ctx::inactive_detector::read(
			const char *subject,
			ctx::json option)
{
	ctx::inactive_detector_storage *handle = new(std::nothrow) ctx::inactive_detector_storage();
	IF_FAIL_RETURN_TAG(handle, ERR_OPERATION_FAILED, _E, "Memory allocation failed");

	int err = handle->read(subject, option);
	if (err != ERR_NONE) {
		delete handle;
		return err;
	}

	return ERR_NONE;
}

ctx::json ctx::inactive_detector::to_json(std::vector<app_t> *app_infos)
{
	ctx::json data;

	for(std::vector<app_t>::iterator app_info = app_infos->begin();
		app_info != app_infos->end(); app_info++)
	{
		ctx::json app_j;
		app_j.set(NULL, APP_INACTIVE_DETECTOR_APPINFO_COLUMN_PACKAGE_NAME, app_info->package_name);
		app_j.set(NULL, APP_INACTIVE_DETECTOR_APPINFO_COLUMN_IS_NODISPLAY, app_info->is_nodisplay);
		app_j.set(NULL, APP_INACTIVE_DETECTOR_APPINFO_COLUMN_IS_ENABLED, app_info->is_enabled);
		app_j.set(NULL, APP_INACTIVE_DETECTOR_APPINFO_COLUMN_IS_ATBOOT, app_info->is_atboot);
		app_j.set(NULL, APP_INACTIVE_DETECTOR_APPINFO_COLUMN_IS_PRELOADED, app_info->is_preloaded);
		app_j.set(NULL, APP_INACTIVE_DETECTOR_APPINFO_COLUMN_TIMESTAMP, app_info->timestamp);
	}
	return data;
}