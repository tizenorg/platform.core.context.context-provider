/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by Batterylicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef _CONTEXT_STATS_BATTERY_DB_HANDLE_H_
#define _CONTEXT_STATS_BATTERY_DB_HANDLE_H_

#include <string>
#include <Json.h>
#include "../shared/DbHandleBase.h"

namespace ctx {
	class BatteryDbHandle : public StatsDbHandleBase {
	public:
		BatteryDbHandle(ContextProvider *provider);
		~BatteryDbHandle();

		int read(Json filter);

	protected:
		std::string createWhereClauseWithDeviceStatus(Json filter);
		std::string createSqlHistory(Json filter);
		std::string createSqlSinceLastCharge(Json filter);
		void replyTriggerItem(int error, Json &jsonResult);
	};
}

#endif	/* End of _CONTEXT_STATS_BATTERY_DB_HANDLE_H_ */
