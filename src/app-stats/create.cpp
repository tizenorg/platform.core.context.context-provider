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

#include <new>
#include <create.h>
#include "AppStatsProvider.h"
#include "AppStatsLogger.h"

using namespace ctx;

/* TODO: This function will be changed into the following form:
   ContextProvider* create(const char *subject) */

extern "C" SO_EXPORT bool create()
{
	registerProvider<AppStatsLogger>(APP_SUBJ_LOGGER, APP_HISTORY_PRIV);
	registerProvider<RecentAppProvider>(APP_SUBJ_RECENTLY_USED, APP_HISTORY_PRIV);
	registerProvider<FrequentAppProvider>(APP_SUBJ_FREQUENTLY_USED, APP_HISTORY_PRIV);
	registerProvider<RareAppProvider>(APP_SUBJ_RARELY_USED, APP_HISTORY_PRIV);
	registerProvider<AppPeakTimeProvider>(APP_SUBJ_PEAK_TIME, APP_HISTORY_PRIV);
	registerProvider<AppSettingProvider>(APP_SUBJ_COMMON_SETTING, APP_HISTORY_PRIV);
	registerProvider<AppFreqProvider>(APP_SUBJ_FREQUENCY, APP_HISTORY_PRIV);

	return true;
}
