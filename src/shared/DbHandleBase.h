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

#ifndef _CONTEXT_STATS_DB_HANDLE_BASE_H_
#define _CONTEXT_STATS_DB_HANDLE_BASE_H_

#include <string>
#include <ContextProvider.h>
#include <DatabaseManager.h>

namespace ctx {
	class SO_EXPORT StatsDbHandleBase : public IDatabaseListener {
	protected:
		bool isTriggerItem;
		ContextProvider *reqProvider;
		Json reqFilter;

		StatsDbHandleBase(ContextProvider *provider);
		~StatsDbHandleBase();

		std::string createWhereClause(Json filter);
		std::string createSqlPeakTime(Json filter, const char* tableName, std::string whereClause);
		std::string createSqlCommonSetting(Json filter, const char* tableName, std::string whereClause);

		bool executeQuery(Json filter, const char* query);
		virtual void replyTriggerItem(int error, Json &jsonResult) = 0;
		static int generateQid();

	private:
		void __jsonVectorToArray(std::vector<Json> &vecJson, Json &jsonResult);

		void onTableCreated(unsigned int queryId, int error);
		void onInserted(unsigned int queryId, int error, int64_t rowId);
		void onExecuted(unsigned int queryId, int error, std::vector<Json>& records);

		DatabaseManager __dbManager;
	};
}

#endif	/* End of _CONTEXT_STATS_DB_HANDLE_BASE_H_ */
