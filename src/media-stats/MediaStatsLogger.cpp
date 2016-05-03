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

#include "MediaStatisticsTypes.h"
#include "MediaStatsLogger.h"

using namespace ctx;

MediaStatsLogger::MediaStatsLogger() :
	ContextProvider(MEDIA_SUBJ_LOGGER)
{
}

MediaStatsLogger::~MediaStatsLogger()
{
}

void MediaStatsLogger::getPrivilege(std::vector<const char*> &privilege)
{
	privilege.push_back(PRIV_MEDIA_HISTORY);
}

int MediaStatsLogger::subscribe(Json option, Json* requestResult)
{
	return ERR_NONE;
}

int MediaStatsLogger::unsubscribe(Json option)
{
	return ERR_NONE;
}
