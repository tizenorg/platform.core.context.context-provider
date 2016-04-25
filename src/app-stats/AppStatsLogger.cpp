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

#include "AppStatsTypesInternal.h"
#include "AppStatsLogger.h"

using namespace ctx;

AppStatsLogger::AppStatsLogger() :
	ContextProvider(APP_SUBJ_LOGGER),
	__initializer(NULL),
	__installMon(NULL),
	__launchMon(NULL)
{
	__initializer = new(std::nothrow) AppDbInitializer();
	IF_FAIL_VOID_TAG(__initializer, _E, "Memory allocation failed");

	__installMon = new(std::nothrow) AppInstallMonitor();
	__launchMon = new(std::nothrow) AppUseMonitor();
	IF_FAIL_CATCH_TAG(__installMon && __launchMon, _E, "Memory allocation failed");
	return;

CATCH:
	delete __installMon;
	delete __launchMon;
	__installMon = NULL;
	__launchMon = NULL;
}

AppStatsLogger::~AppStatsLogger()
{
	delete __installMon;
	delete __launchMon;
}

int AppStatsLogger::subscribe(Json option, Json* requestResult)
{
	return ERR_NONE;
}

int AppStatsLogger::unsubscribe(Json option)
{
	return ERR_NONE;
}
