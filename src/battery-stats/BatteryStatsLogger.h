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

#ifndef _CONTEXT_BATTERY_STATS_LOGGER_H_
#define _CONTEXT_BATTERY_STATS_LOGGER_H_
#include <ContextProvider.h>
#include "DbInit.h"

namespace ctx {

	class HeartManager;
	class BatteryStatsLogger : public ContextProvider {
	public:
		BatteryStatsLogger();
		~BatteryStatsLogger();

		int subscribe(Json option, Json *requestResult);
		int unsubscribe(Json option);

		void getPrivilege(std::vector<const char*> &privilege);

	private:
		BatteryDbInitializer *__initializer;
		HeartManager *__heartMgr;
	};

}	/* namespace ctx */

#endif	/* _CONTEXT_BATTERY_STATS_LOGGER_H_ */
