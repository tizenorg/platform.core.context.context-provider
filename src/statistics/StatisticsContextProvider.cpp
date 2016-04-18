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
#include <ContextManager.h>
#include <ContextProvider.h>
#include <StatisticsContextProvider.h>

#include "app/AppStatisticsProvider.h"

#ifndef _DISABLE_PREDICTION_ENGINE_
// include prediction engine header files here
#endif

#ifdef _MOBILE_
#include "media/MediaStatisticsProvider.h"
#include "social/SocialStatisticsProvider.h"
#endif

#ifdef _TV_
#include "media/MediaStatisticsProvider.h"
#endif

template<typename Provider>
void registerProvider(const char *subject, const char *privilege)
{
	if (!Provider::isSupported(subject))
		return;

	ctx::ContextProviderInfo providerInfo(Provider::create, Provider::destroy, NULL, privilege);
	ctx::context_manager::registerProvider(subject, providerInfo);
}

SO_EXPORT bool ctx::initStatisticsContextProvider()
{
	AppStatisticsProvider::create(NULL);
	registerProvider<AppStatisticsProvider>(APP_SUBJ_RECENTLY_USED, APP_HISTORY_PRIV);
	registerProvider<AppStatisticsProvider>(APP_SUBJ_FREQUENTLY_USED, APP_HISTORY_PRIV);
	registerProvider<AppStatisticsProvider>(APP_SUBJ_RARELY_USED, APP_HISTORY_PRIV);
	registerProvider<AppStatisticsProvider>(APP_SUBJ_PEAK_TIME, APP_HISTORY_PRIV);
	registerProvider<AppStatisticsProvider>(APP_SUBJ_COMMON_SETTING, APP_HISTORY_PRIV);
	registerProvider<AppStatisticsProvider>(APP_SUBJ_FREQUENCY, APP_HISTORY_PRIV);
	AppStatisticsProvider::submitTriggerItem();

#ifndef _DISABLE_PREDICTION_ENGINE_
// initialize the prediction engine here
#endif

#ifdef _MOBILE_
	MediaStatisticsProvider::create(NULL);
	registerProvider<MediaStatisticsProvider>(MEDIA_SUBJ_PEAK_TIME_FOR_MUSIC, MEDIA_HISTORY_PRIV);
	registerProvider<MediaStatisticsProvider>(MEDIA_SUBJ_PEAK_TIME_FOR_VIDEO, MEDIA_HISTORY_PRIV);
	registerProvider<MediaStatisticsProvider>(MEDIA_SUBJ_COMMON_SETTING_FOR_MUSIC, MEDIA_HISTORY_PRIV);
	registerProvider<MediaStatisticsProvider>(MEDIA_SUBJ_COMMON_SETTING_FOR_VIDEO, MEDIA_HISTORY_PRIV);
	registerProvider<MediaStatisticsProvider>(MEDIA_SUBJ_MUSIC_FREQUENCY, MEDIA_HISTORY_PRIV);
	registerProvider<MediaStatisticsProvider>(MEDIA_SUBJ_VIDEO_FREQUENCY, MEDIA_HISTORY_PRIV);
	MediaStatisticsProvider::submitTriggerItem();

	SocialStatisticsProvider::create(NULL);
	registerProvider<SocialStatisticsProvider>(SOCIAL_SUBJ_FREQ_ADDRESS, SOCIAL_HISTORY_PRIV);
	registerProvider<SocialStatisticsProvider>(SOCIAL_SUBJ_FREQUENCY, SOCIAL_HISTORY_PRIV);
	SocialStatisticsProvider::submitTriggerItem();
#endif

#ifdef _TV_
	MediaStatisticsProvider::create(NULL);
	registerProvider<MediaStatisticsProvider>(MEDIA_SUBJ_PEAK_TIME_FOR_MUSIC, MEDIA_HISTORY_PRIV);
	registerProvider<MediaStatisticsProvider>(MEDIA_SUBJ_PEAK_TIME_FOR_VIDEO, MEDIA_HISTORY_PRIV);
	registerProvider<MediaStatisticsProvider>(MEDIA_SUBJ_COMMON_SETTING_FOR_MUSIC, MEDIA_HISTORY_PRIV);
	registerProvider<MediaStatisticsProvider>(MEDIA_SUBJ_COMMON_SETTING_FOR_VIDEO, MEDIA_HISTORY_PRIV);
	registerProvider<MediaStatisticsProvider>(MEDIA_SUBJ_MUSIC_FREQUENCY, MEDIA_HISTORY_PRIV);
	registerProvider<MediaStatisticsProvider>(MEDIA_SUBJ_VIDEO_FREQUENCY, MEDIA_HISTORY_PRIV);
	MediaStatisticsProvider::submitTriggerItem();
#endif

	return true;
}
