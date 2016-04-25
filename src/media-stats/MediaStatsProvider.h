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

#ifndef _CONTEXT_MEDIA_STATS_PROVIDER_H_
#define _CONTEXT_MEDIA_STATS_PROVIDER_H_

#include <ContextProvider.h>
#include "MediaStatsTypesInternal.h"

namespace ctx {

	class MediaStatsProvider : public ContextProvider {
	public:
		int read(Json option, Json* requestResult);

		bool isSupported();
		virtual void submitTriggerItem();

	protected:
		MediaStatsProvider(const char *subject);
		virtual ~MediaStatsProvider();
	};	/* class MediaStatsProvider */


	class MusicPeakTimeProvider : public MediaStatsProvider {
	public:
		MusicPeakTimeProvider() :
			MediaStatsProvider(MEDIA_SUBJ_PEAK_TIME_FOR_MUSIC) {}
	};


	class VideoPeakTimeProvider : public MediaStatsProvider {
	public:
		VideoPeakTimeProvider() :
			MediaStatsProvider(MEDIA_SUBJ_PEAK_TIME_FOR_VIDEO) {}
	};


	class MusicSettingProvider: public MediaStatsProvider {
	public:
		MusicSettingProvider() :
			MediaStatsProvider(MEDIA_SUBJ_COMMON_SETTING_FOR_MUSIC) {}
	};


	class VideoSettingProvider: public MediaStatsProvider {
	public:
		VideoSettingProvider() :
			MediaStatsProvider(MEDIA_SUBJ_COMMON_SETTING_FOR_VIDEO) {}
	};


	class MusicFreqProvider: public MediaStatsProvider {
	public:
		MusicFreqProvider() :
			MediaStatsProvider(MEDIA_SUBJ_MUSIC_FREQUENCY) {}

		void submitTriggerItem();
	};


	class VideoFreqProvider: public MediaStatsProvider {
	public:
		VideoFreqProvider() :
			MediaStatsProvider(MEDIA_SUBJ_VIDEO_FREQUENCY) {}

		void submitTriggerItem();
	};

}	/* namespace ctx */

#endif	/* _CONTEXT_MEDIA_STATS_PROVIDER_H_ */
