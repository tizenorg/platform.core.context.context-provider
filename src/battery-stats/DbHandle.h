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

#ifndef _CONTEXT_STATS_APP_DB_HANDLE_H_
#define _CONTEXT_STATS_APP_DB_HANDLE_H_

#include <string>
#include <Json.h>
#include "../shared/DbHandleBase.h"

namespace ctx {
	class AppDbHandle : public StatsDbHandleBase {
	public:
		AppDbHandle(ContextProvider *provider);
		~AppDbHandle();

		int read(Json filter);

	protected:
		std::string createWhereClauseWithDeviceStatus(Json filter);
		std::string createSqlRecentlyUsed(Json filter);
		std::string createSqlFrequentlyUsed(Json filter);
		std::string createSqlRarelyUsed(Json filter);
		std::string createSqlPeakTime(Json filter);
		std::string createSqlCommonSetting(Json filter);
		std::string createSqlFrequency(Json filter);
		void replyTriggerItem(int error, Json &jsonResult);
	};
}

#endif	/* End of _CONTEXT_STATS_APP_DB_HANDLE_H_ */
