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

#ifndef __CONTEXT_QUERIER_H__
#define __CONTEXT_QUERIER_H__

#include <ContextProvider.h>
#include <DatabaseManager.h>

namespace ctx {

	class Querier : public IDatabaseListener {
	public:
		Querier(ContextProvider *provider, Json option);
		virtual ~Querier();

		virtual int queryRaw(int startTime, int endTime);
		virtual int query(int startTime, int endTime);
		virtual int query(int startTime, int endTime, int anchor, int interval);

	protected:
		int query(const char *sql);
		void onTableCreated(unsigned int queryId, int error);
		void onInserted(unsigned int queryId, int error, int64_t rowId);
		void onExecuted(unsigned int queryId, int error, std::vector<Json>& records);

	private:
		void __convertToResponse(std::vector<Json> &sqlResult, Json &response);

		DatabaseManager __dbMgr;
		ContextProvider *__provider;
		Json __option;
	};
}

#endif /* __CONTEXT_QUERIER_H__ */
