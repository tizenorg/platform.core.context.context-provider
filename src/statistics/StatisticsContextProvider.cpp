/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd.  *
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

#include <Types.h>
#include <ContextProvider.h>
#include <StatisticsContextProvider.h>

//#include "app/AppStatisticsProvider.h"

#ifndef _DISABLE_PREDICTION_ENGINE_
// include prediction engine header files here
#endif

#ifdef _MOBILE_
#include "media/MediaStatsProvider.h"
#include "media/MediaStatsLogger.h"
#include "social/SocialStatsProvider.h"
#include "social/SocialStatsLogger.h"
#endif

using namespace ctx;

template<typename Provider>
void registerProvider(const char *subject, const char *privilege)
{
	Provider *provider = new(std::nothrow) Provider();
	IF_FAIL_VOID_TAG(provider, _E, "Memory allocation failed");

	if (!provider->isSupported()) {
		delete provider;
		return;
	}

	provider->registerProvider(privilege, provider);
	provider->submitTriggerItem();
}

SO_EXPORT bool initStatisticsContextProvider()
{
	/*
	registerProvider<AppStatisticsProvider>(APP_SUBJ_RECENTLY_USED, APP_HISTORY_PRIV);
	registerProvider<AppStatisticsProvider>(APP_SUBJ_FREQUENTLY_USED, APP_HISTORY_PRIV);
	registerProvider<AppStatisticsProvider>(APP_SUBJ_RARELY_USED, APP_HISTORY_PRIV);
	registerProvider<AppStatisticsProvider>(APP_SUBJ_PEAK_TIME, APP_HISTORY_PRIV);
	registerProvider<AppStatisticsProvider>(APP_SUBJ_COMMON_SETTING, APP_HISTORY_PRIV);
	registerProvider<AppStatisticsProvider>(APP_SUBJ_FREQUENCY, APP_HISTORY_PRIV);
	*/

#ifndef _DISABLE_PREDICTION_ENGINE_
// initialize the prediction engine here
#endif

#ifdef _MOBILE_
	registerProvider<MediaStatsLogger>(MEDIA_SUBJ_LOGGER, MEDIA_HISTORY_PRIV);
	registerProvider<MusicPeakTimeProvider>(MEDIA_SUBJ_PEAK_TIME_FOR_MUSIC, MEDIA_HISTORY_PRIV);
	registerProvider<VideoPeakTimeProvider>(MEDIA_SUBJ_PEAK_TIME_FOR_VIDEO, MEDIA_HISTORY_PRIV);
	registerProvider<MusicSettingProvider>(MEDIA_SUBJ_COMMON_SETTING_FOR_MUSIC, MEDIA_HISTORY_PRIV);
	registerProvider<VideoSettingProvider>(MEDIA_SUBJ_COMMON_SETTING_FOR_VIDEO, MEDIA_HISTORY_PRIV);
	registerProvider<MusicFreqProvider>(MEDIA_SUBJ_MUSIC_FREQUENCY, MEDIA_HISTORY_PRIV);
	registerProvider<VideoFreqProvider>(MEDIA_SUBJ_VIDEO_FREQUENCY, MEDIA_HISTORY_PRIV);

	registerProvider<SocialStatsLogger>(SOCIAL_SUBJ_LOGGER, SOCIAL_HISTORY_PRIV);
	registerProvider<ContactFreqProvider>(SOCIAL_SUBJ_FREQUENCY, SOCIAL_HISTORY_PRIV);
	registerProvider<TopContactsProvider>(SOCIAL_SUBJ_FREQ_ADDRESS, SOCIAL_HISTORY_PRIV);
#endif

	return true;
}
