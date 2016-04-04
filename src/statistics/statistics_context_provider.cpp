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

#include <types_internal.h>
#include <ContextManager.h>
#include <ContextProviderBase.h>
#include <statistics_context_provider.h>

#include "app/app_stats_provider.h"

#ifndef _DISABLE_PREDICTION_ENGINE_
// include prediction engine header files here
#endif

#ifdef _MOBILE_
#include "media/media_stats_provider.h"
#include "social/social_stats_provider.h"
#endif

#ifdef _TV_
#include "media/media_stats_provider.h"
#endif

template<typename provider>
void registerProvider(const char *subject, const char *privilege)
{
	if (!provider::is_supported(subject))
		return;

	ctx::ContextProviderInfo providerInfo(provider::create, provider::destroy, NULL, privilege);
	ctx::context_manager::registerProvider(subject, providerInfo);
}

EXTAPI bool ctx::init_statistics_context_provider()
{
	app_statistics_provider::create(NULL);
	registerProvider<app_statistics_provider>(APP_SUBJ_RECENTLY_USED, APP_HISTORY_PRIV);
	registerProvider<app_statistics_provider>(APP_SUBJ_FREQUENTLY_USED, APP_HISTORY_PRIV);
	registerProvider<app_statistics_provider>(APP_SUBJ_RARELY_USED, APP_HISTORY_PRIV);
	registerProvider<app_statistics_provider>(APP_SUBJ_PEAK_TIME, APP_HISTORY_PRIV);
	registerProvider<app_statistics_provider>(APP_SUBJ_COMMON_SETTING, APP_HISTORY_PRIV);
	registerProvider<app_statistics_provider>(APP_SUBJ_FREQUENCY, APP_HISTORY_PRIV);
	app_statistics_provider::submit_trigger_item();

#ifndef _DISABLE_PREDICTION_ENGINE_
// initialize the prediction engine here
#endif

#ifdef _MOBILE_
	media_statistics_provider::create(NULL);
	registerProvider<media_statistics_provider>(MEDIA_SUBJ_PEAK_TIME_FOR_MUSIC, MEDIA_HISTORY_PRIV);
	registerProvider<media_statistics_provider>(MEDIA_SUBJ_PEAK_TIME_FOR_VIDEO, MEDIA_HISTORY_PRIV);
	registerProvider<media_statistics_provider>(MEDIA_SUBJ_COMMON_SETTING_FOR_MUSIC, MEDIA_HISTORY_PRIV);
	registerProvider<media_statistics_provider>(MEDIA_SUBJ_COMMON_SETTING_FOR_VIDEO, MEDIA_HISTORY_PRIV);
	registerProvider<media_statistics_provider>(MEDIA_SUBJ_MUSIC_FREQUENCY, MEDIA_HISTORY_PRIV);
	registerProvider<media_statistics_provider>(MEDIA_SUBJ_VIDEO_FREQUENCY, MEDIA_HISTORY_PRIV);
	media_statistics_provider::submit_trigger_item();

	social_statistics_provider::create(NULL);
	registerProvider<social_statistics_provider>(SOCIAL_SUBJ_FREQ_ADDRESS, SOCIAL_HISTORY_PRIV);
	registerProvider<social_statistics_provider>(SOCIAL_SUBJ_FREQUENCY, SOCIAL_HISTORY_PRIV);
	social_statistics_provider::submit_trigger_item();
#endif

#ifdef _TV_
	media_statistics_provider::create(NULL);
	registerProvider<media_statistics_provider>(MEDIA_SUBJ_PEAK_TIME_FOR_MUSIC, MEDIA_HISTORY_PRIV);
	registerProvider<media_statistics_provider>(MEDIA_SUBJ_PEAK_TIME_FOR_VIDEO, MEDIA_HISTORY_PRIV);
	registerProvider<media_statistics_provider>(MEDIA_SUBJ_COMMON_SETTING_FOR_MUSIC, MEDIA_HISTORY_PRIV);
	registerProvider<media_statistics_provider>(MEDIA_SUBJ_COMMON_SETTING_FOR_VIDEO, MEDIA_HISTORY_PRIV);
	registerProvider<media_statistics_provider>(MEDIA_SUBJ_MUSIC_FREQUENCY, MEDIA_HISTORY_PRIV);
	registerProvider<media_statistics_provider>(MEDIA_SUBJ_VIDEO_FREQUENCY, MEDIA_HISTORY_PRIV);
	media_statistics_provider::submit_trigger_item();
#endif

	return true;
}
