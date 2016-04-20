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

#include <ctime>
#include <Types.h>
#include "InactiveDetector.h"
#include "InactiveDetectorWeight.h"
#include "InactiveDetectorStorage.h"
#include "InactiveDetectorClassificator.h"
#include "TimerManager.h"

ctx::InactiveDetector::InactiveDetector()
{
	// execute once every night
	__timerId = __timerMgr.setAt(APP_INACTIVE_TASK_START_HOUR, APP_INACTIVE_TASK_START_MINUTE,
					DayOfWeek::EVERYDAY, this);

	if (__timerId < 0) {
		_E("InactiveDetector timer set FAIL");
		return;
	} else {
		updateClusters();
		_D("InactiveDetector timer set SUCCESS");
	}

}

ctx::InactiveDetector::~InactiveDetector()
{
	if (__timerId >= 0) {
		__timerMgr.remove(__timerId);
		_D("inactive_detejctor timer removed");
	}
}

bool ctx::InactiveDetector::onTimerExpired(int timerId)
{
	updateClusters();
	return true;
}

int ctx::InactiveDetector::updateClusters()
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
	for(auto timeframe = timeframes.begin(); timeframe != timeframes.end(); timeframe++) {
		InactiveDetectorWeight inactDetWeight;
		int error = inactDetWeight.requestWeights(*timeframe);
		_E("updateClusters requestWeights:%d, error:%d", *timeframe, error);
	}

	return ERR_NONE;
}

void ctx::InactiveDetector::onClusterUpdateFinished(std::vector<AppInfo> *appsWithWeight, int error)
{
	if (error == ERR_NONE) {
		_I("onClusterUpdateFinished success");
	}
	// clean memory
	delete appsWithWeight;
}

int ctx::InactiveDetector::read(const char *subject, ctx::Json option)
{
	ctx::InactiveDetectorStorage *handle = new(std::nothrow) ctx::InactiveDetectorStorage();
	IF_FAIL_RETURN_TAG(handle, ERR_OPERATION_FAILED, _E, "Memory allocation failed");

	int err = handle->read(subject, option);
	if (err != ERR_NONE) {
		delete handle;
		return err;
	}

	return ERR_NONE;
}

ctx::Json ctx::InactiveDetector::toJson(std::vector<AppInfo> *appInfos)
{
	ctx::Json data;

	for(auto appInfo = appInfos->begin(); appInfo != appInfos->end(); appInfo++) {
		ctx::Json appJ;
		appJ.set(NULL, APP_INACTIVE_DETECTOR_APPINFO_COLUMN_PACKAGE_NAME, appInfo->packageName);
		appJ.set(NULL, APP_INACTIVE_DETECTOR_APPINFO_COLUMN_IS_NODISPLAY, appInfo->isNodisplay);
		appJ.set(NULL, APP_INACTIVE_DETECTOR_APPINFO_COLUMN_IS_ENABLED, appInfo->isEnabled);
		appJ.set(NULL, APP_INACTIVE_DETECTOR_APPINFO_COLUMN_IS_ATBOOT, appInfo->isAtBoot);
		appJ.set(NULL, APP_INACTIVE_DETECTOR_APPINFO_COLUMN_IS_PRELOADED, appInfo->isPreloaded);
		appJ.set(NULL, APP_INACTIVE_DETECTOR_APPINFO_COLUMN_TIMESTAMP, appInfo->timestamp);
	}
	return data;
}
