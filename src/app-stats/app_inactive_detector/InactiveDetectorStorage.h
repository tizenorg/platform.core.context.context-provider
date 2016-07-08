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

#ifndef _CONTEXT_INACTIVE_DETECTOR_STORAGE_H_
#define _CONTEXT_INACTIVE_DETECTOR_STORAGE_H_

#include <vector>
#include "AppInactiveDetectorTypes.h"
#include <Json.h>
#include <DatabaseManager.h>

namespace ctx {

	class InactiveDetectorStorage : public IDatabaseListener
	{
	private:
		//int type;  //TODO: enum
		void __injectParams(std::string& str, const std::string& from, const std::string& to);
		void __jsonToObject(std::vector<Json>& records, std::vector<AppInfo> *appsWithWeights, bool resultMode);
		std::string __getQueryGetApps(const char *subject, ctx::Json filter);
		std::string __getQueryUpdateApps(std::vector<AppInfo> *appsWithWeights);
		std::string __getSubqueryFormValues(std::vector<AppInfo> *appsWithWeights);

		void onTableCreated(unsigned int queryId, int error) {}
		void onInserted(unsigned int queryId, int error, int64_t rowId) {}
		void onExecuted(unsigned int queryId, int error, std::vector<Json>& records);

		DatabaseManager __dbManager;

	public:
		InactiveDetectorStorage();
		~InactiveDetectorStorage();

		int read(const char *subject, ctx::Json filter);

		int updateRanks();
		int getAppsInfoWithWeights(double timestampFrom);
	};	/* class InactiveDetectorStorage */

}	/* namespace ctx */

#endif /* _CONTEXT_INACTIVE_DETECTOR_STORAGE_H_ */
