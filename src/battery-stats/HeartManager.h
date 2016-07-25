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

#ifndef _CONTEXT_BATTERY_HEART_MANAGER_H_
#define _CONTEXT_BATTERY_HEART_MANAGER_H_
#include "../shared/DbHandleBase.h"
#include <time.h>
#include <sqlite3.h>


namespace ctx {

	class HeartManager {
	public:
		HeartManager();
		~HeartManager();
		sqlite3 *__databaseHandle;
		bool getAppData(std::string query, std::vector<Json>* app_vector);

	private:
		static int __executionCb(void *userData, int dim, char **value, char **column);
	};

}	/* namespace ctx */

#endif	/* _CONTEXT_BATTERY_HEART_MANAGER_H_ */
