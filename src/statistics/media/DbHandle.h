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

#ifndef _CONTEXT_STATS_MEDIA_DB_HANDLE_H_
#define _CONTEXT_STATS_MEDIA_DB_HANDLE_H_

#include <string>
#include <Json.h>
#include "../shared/DbHandleBase.h"

namespace ctx {
	class MediaDbHandle : public StatsDbHandleBase {
	public:
		MediaDbHandle(ContextProvider *provider);
		~MediaDbHandle();

		int read(Json filter);

	protected:
		std::string createWhereClause(int mediaType, Json filter);
		std::string createSqlPeakTime(int mediaType, Json filter);
		std::string createSqlCommonSetting(int mediaType, Json filter);
		std::string createSqlFrequency(int mediaType, Json filter);
		void replyTriggerItem(int error, Json &jsonResult);
	};
}

#endif	/* End of _CONTEXT_STATS_MEDIA_DB_HANDLE_H_ */
