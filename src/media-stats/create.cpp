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
#include "MediaStatsProvider.h"
#include "MediaStatsLogger.h"

using namespace ctx;

/* TODO: This function will be changed into the following form:
   ContextProvider* create(const char *subject) */

SO_EXPORT bool create()
{
	registerProvider<MediaStatsLogger>(MEDIA_SUBJ_LOGGER, MEDIA_HISTORY_PRIV);
	registerProvider<MusicPeakTimeProvider>(MEDIA_SUBJ_PEAK_TIME_FOR_MUSIC, MEDIA_HISTORY_PRIV);
	registerProvider<VideoPeakTimeProvider>(MEDIA_SUBJ_PEAK_TIME_FOR_VIDEO, MEDIA_HISTORY_PRIV);
	registerProvider<MusicSettingProvider>(MEDIA_SUBJ_COMMON_SETTING_FOR_MUSIC, MEDIA_HISTORY_PRIV);
	registerProvider<VideoSettingProvider>(MEDIA_SUBJ_COMMON_SETTING_FOR_VIDEO, MEDIA_HISTORY_PRIV);
	registerProvider<MusicFreqProvider>(MEDIA_SUBJ_MUSIC_FREQUENCY, MEDIA_HISTORY_PRIV);
	registerProvider<VideoFreqProvider>(MEDIA_SUBJ_VIDEO_FREQUENCY, MEDIA_HISTORY_PRIV);

	return true;
}
