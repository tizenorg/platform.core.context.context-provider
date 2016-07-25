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

#include "BatteryStatisticsTypes.h"
#include "BatteryStatsLogger.h"
#include "HeartManager.h"

using namespace ctx;

BatteryStatsLogger::BatteryStatsLogger() :
	ContextProvider(BATTERY_SUBJ_LOGGER),
	__initializer(NULL)
{
	__initializer = new(std::nothrow) BatteryDbInitializer();
	IF_FAIL_VOID_TAG(__initializer, _E, "Memory allocation failed");

	__heartMgr = new(std::nothrow) HeartManager();
	__heartMgr->getAppData(NULL, 0, 0);
	return;

}

BatteryStatsLogger::~BatteryStatsLogger()
{
	// memory management
}

void BatteryStatsLogger::getPrivilege(std::vector<const char*> &privilege)
{
	privilege.push_back(PRIV_APP_HISTORY);
}

int BatteryStatsLogger::subscribe(Json option, Json* requestResult)
{
	_D("[kunal] Battery Logger Launched");
	return ERR_NONE;
}

int BatteryStatsLogger::unsubscribe(Json option)
{
	return ERR_NONE;
}
