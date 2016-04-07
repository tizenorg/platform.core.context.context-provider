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

#ifndef _CONTEXT_STATS_APP_INSTALL_MONITOR_H_
#define _CONTEXT_STATS_APP_INSTALL_MONITOR_H_

#include <package_manager.h>
#include <DatabaseManager.h>

namespace ctx {

	class AppInstallMonitor : public IDatabaseListener {
	private:
		package_manager_h __pkgmgrHandle;

		bool __startMonitoring();
		void __stopMonitoring();

		static std::string __createDeletionQuery(const char* tableName, const char* appId);
		static void __packageEventCb(const char *type, const char *package, package_manager_event_type_e eventType, package_manager_event_state_e eventState, int progress, package_manager_error_e error, void *userData);
		static bool __appInfoCb(package_info_app_component_type_e compType, const char *appId, void *userData);

		void onTableCreated(unsigned int queryId, int error) {}
		void onInserted(unsigned int queryId, int error, int64_t rowId) {}
		void onExecuted(unsigned int queryId, int error, std::vector<Json>& records) {}

	public:
		AppInstallMonitor();
		~AppInstallMonitor();
	};

}	/* namespace ctx */

#endif	/* End of _CONTEXT_STATS_APP_INSTALL_MONITOR_H_ */
