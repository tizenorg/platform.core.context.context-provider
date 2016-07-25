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

#ifndef _CONTEXT_STATS_BATTERY_DB_INITIALIZER_H_
#define _CONTEXT_STATS_BATTERY_DB_INITIALIZER_H_

#include <package_manager.h>
#include <DatabaseManager.h>

namespace ctx {

	class BatteryDbInitializer : public IDatabaseListener {
	private:
		void __createTable();
		void __checkBatteryList();
		void __duplicateBatteryList();

		void onTableCreated(unsigned int queryId, int error);
		void onInserted(unsigned int queryId, int error, int64_t rowId);
		void onExecuted(unsigned int queryId, int error, std::vector<Json>& records);

		static bool __packageInfoCb(package_info_h packageInfo, void *userData);
		static bool __appInfoCb(package_info_app_component_type_e compType, const char *appId, void *userData);

		DatabaseManager __dbManager;

	public:
		BatteryDbInitializer();
		~BatteryDbInitializer();
	};

}	/* namespace ctx */

#endif	/* End of _CONTEXT_STATS_BATTERY_DB_INITIALIZER_H_ */
