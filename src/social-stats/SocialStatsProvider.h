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

#ifndef _CONTEXT_SOCIAL_STATS_PROVIDER_H_
#define _CONTEXT_SOCIAL_STATS_PROVIDER_H_

#include <ContextProvider.h>
#include "SocialStatsTypesInternal.h"

namespace ctx {

	class SocialStatsProvider : public ContextProvider {
	public:
		int read(Json option, Json* requestResult);

		bool isSupported();
		virtual void submitTriggerItem();

	protected:
		SocialStatsProvider(const char *subject);
		virtual ~SocialStatsProvider();
	};


	class ContactFreqProvider: public SocialStatsProvider {
	public:
		ContactFreqProvider() :
			SocialStatsProvider(SOCIAL_SUBJ_FREQUENCY) {}

		void submitTriggerItem();
	};


	class TopContactsProvider: public SocialStatsProvider {
	public:
		TopContactsProvider() :
			SocialStatsProvider(SOCIAL_SUBJ_FREQ_ADDRESS) {}
	};

}	/* namespace ctx */

#endif	/* _CONTEXT_SOCIAL_STATS_PROVIDER_H_ */
