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

#ifndef __CONTEXT_SENSOR_LOGGER_H__
#define __CONTEXT_SENSOR_LOGGER_H__

#include <DatabaseManager.h>

namespace ctx {

	class SensorLogger {
	public:
		SensorLogger();
		virtual ~SensorLogger();

		virtual bool start() = 0;
		virtual void stop() = 0;
		virtual void flushCache(bool force = false);

	protected:
		bool executeQuery(const char *query);

		virtual void removeExpired(const char *subject, const char *tableName, const char *timeKey);

	private:
		uint64_t __lastRemovalTime;
		DatabaseManager __dbMgr;
	};
}

#endif /* __CONTEXT_SENSOR_LOGGER_H__ */
