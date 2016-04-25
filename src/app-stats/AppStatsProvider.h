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

#ifndef _CONTEXT_APP_STATS_PROVIDER_H_
#define _CONTEXT_APP_STATS_PROVIDER_H_

#include <ContextProvider.h>
#include "AppStatsTypesInternal.h"

namespace ctx {

	class AppStatsProvider : public ContextProvider {
	public:
		int read(Json option, Json *requestResult);
		virtual void submitTriggerItem();

	protected:
		AppStatsProvider(const char *subject);
		virtual ~AppStatsProvider();
	};


	class RecentAppProvider : public AppStatsProvider {
	public:
		RecentAppProvider() :
			AppStatsProvider(APP_SUBJ_RECENTLY_USED) {}
	};


	class FrequentAppProvider : public AppStatsProvider {
	public:
		FrequentAppProvider() :
			AppStatsProvider(APP_SUBJ_FREQUENTLY_USED) {}
	};


	class RareAppProvider : public AppStatsProvider {
	public:
		RareAppProvider() :
			AppStatsProvider(APP_SUBJ_RARELY_USED) {}
	};


	class AppPeakTimeProvider : public AppStatsProvider {
	public:
		AppPeakTimeProvider() :
			AppStatsProvider(APP_SUBJ_PEAK_TIME) {}
	};


	class AppSettingProvider : public AppStatsProvider {
	public:
		AppSettingProvider() :
			AppStatsProvider(APP_SUBJ_COMMON_SETTING) {}
	};


	class AppFreqProvider : public AppStatsProvider {
	public:
		AppFreqProvider() :
			AppStatsProvider(APP_SUBJ_FREQUENCY) {}

		void submitTriggerItem();
	};

}	/* namespace ctx */

#endif	/* _CONTEXT_APP_STATS_PROVIDER_H_ */
