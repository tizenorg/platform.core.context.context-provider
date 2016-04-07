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

#ifndef _CONTEXT_STATS_APP_USE_MONITOR_H_
#define _CONTEXT_STATS_APP_USE_MONITOR_H_

#include <string>
#include <sstream>
#include <DBusSignalWatcher.h>

namespace ctx {

	class AppUseMonitor : public IDBusSignalListener {
	private:
		int64_t __signalId;
		int __lastCleanupTime;
		DBusSignalWatcher __dbusWatcher;

		bool __startLogging(void);
		void __stopLogging(void);

		bool __isSkippable(std::string appId);
		void __createRecord(std::string appId);
		void __finishRecord(std::string appId);
		void __removeExpired();
		void onSignal(const char *sender, const char *path, const char *iface, const char *name, GVariant *param);

	public:
		AppUseMonitor();
		~AppUseMonitor();
	};	/* class AppUseMonitor */

}	/* namespace ctx */

#endif	/* End of _CONTEXT_STATS_APP_USE_MONITOR_H_ */
