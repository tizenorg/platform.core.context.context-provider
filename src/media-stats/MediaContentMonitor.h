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

#ifndef _CONTEXT_STATS_MEDIA_CONTENT_MONITOR_H_
#define _CONTEXT_STATS_MEDIA_CONTENT_MONITOR_H_

#include <sstream>
#include <DBusSignalWatcher.h>
#include <DatabaseManager.h>

namespace ctx {

	class MediaContentMonitor : public IDatabaseListener, public IDBusSignalListener {
	private:
		bool __started;
		int __lastCleanupTime;
		DatabaseManager __dbManager;
		int64_t __dbusSignalId;
		DBusSignalWatcher __dbusWatcher;

		bool __startMonitoring();
		void __stopMonitoring();

		void __appendCleanupQuery(std::stringstream &query);
		void __updatePlayCount(const char *uuid, int type, int count);
		void __insertLog(int mediaType);

		bool __getPlayCount(int updateItem, int updateType, int mediaType, char *uuid, int *count);

		void onTableCreated(unsigned int queryId, int error) {}
		void onInserted(unsigned int queryId, int error, int64_t rowId) {}
		void onExecuted(unsigned int queryId, int error, std::vector<Json>& records);
		void onSignal(const char *sender, const char *path, const char *iface, const char *name, GVariant *param);

	public:
		MediaContentMonitor();
		~MediaContentMonitor();
	};

}	/* namespace ctx */

#endif	/* End of _CONTEXT_STATS_MEDIA_CONTENT_MONITOR_H_ */
