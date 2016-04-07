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
#include <Json.h>
#include <DatabaseManager.h>

namespace ctx {
	class StatsDbHandleBase : public IDatabaseListener {
	protected:
		bool __isTriggerItem;
		std::string __reqSubject;
		ctx::Json __reqFilter;
		DatabaseManager __dbManager;

		StatsDbHandleBase();
		~StatsDbHandleBase();

		std::string createWhereClause(ctx::Json filter);
		std::string createSqlPeakTime(ctx::Json filter, const char* tableName, std::string whereClause);
		std::string createSqlCommonSetting(ctx::Json filter, const char* tableName, std::string whereClause);

		bool executeQuery(const char* subject, ctx::Json filter, const char* query);
		virtual void replyTriggerItem(int error, ctx::Json &jsonResult) = 0;
		static int generateQid();

	private:
		void jsonVectorToArray(std::vector<Json> &vecJson, ctx::Json &jsonResult);

		void onTableCreated(unsigned int queryId, int error);
		void onInserted(unsigned int queryId, int error, int64_t rowId);
		void onExecuted(unsigned int queryId, int error, std::vector<Json>& records);
	};
}

#endif	/* End of _CONTEXT_STATS_DB_HANDLE_BASE_H_ */
