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

#ifndef __CONTEXT_CLIENT_INFO_H__
#define __CONTEXT_CLIENT_INFO_H__

#include <string>
#include <vector>
#include <Json.h>
#include <DatabaseManager.h>
#include "UninstallMonitor.h"

namespace ctx {

	class ClientInfo {
	public:
		ClientInfo();
		~ClientInfo();

		int get(std::string subject, std::string pkgId, Json& option);
		int get(std::string subject, std::vector<Json>& options);
		bool exist(std::string subject);

		bool set(std::string subject, std::string pkgId, Json option, int retentionPeriod);
		bool remove(std::string subject, std::string pkgId);

		static void purgeClient(std::string pkgId);

	private:
		static unsigned int __refCnt;
		static DatabaseManager *__dbMgr;
		static UninstallMonitor *__uninstallMonitor;
	};
}

#endif /* _CONTEXT_CLIENT_INFO_H_ */
