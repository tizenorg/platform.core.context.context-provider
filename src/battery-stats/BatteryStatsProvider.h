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

#ifndef _CONTEXT_BATTERY_STATS_PROVIDER_H_
#define _CONTEXT_BATTERY_STATS_PROVIDER_H_

#include <ContextProvider.h>
#include "BatteryStatisticsTypes.h"

namespace ctx {

	class BatteryStatsProvider : public ContextProvider {
	public:
		int read(Json option, Json *requestResult);
		void getPrivilege(std::vector<const char*> &privilege);

	protected:
		BatteryStatsProvider(const char *subject);
		virtual ~BatteryStatsProvider();
	};


	class BatteryHistoryProvider : public BatteryStatsProvider {
	public:
		BatteryHistoryProvider() :
			BatteryStatsProvider(SUBJ_BATTERY_HISTORY) {}
	};


	class BatterySinceLastChargeProvider : public BatteryStatsProvider {
	public:
		BatterySinceLastChargeProvider() :
			BatteryStatsProvider(SUBJ_BATTERY_SINCE_LAST_CHARGE) {}
	};


}	/* namespace ctx */

#endif	/* _CONTEXT_BATTERY_STATS_PROVIDER_H_ */
