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

#ifndef __CONTEXT_PRESSURE_LOGGER_H__
#define __CONTEXT_PRESSURE_LOGGER_H__

#include "../SensorLogger.h"
#include "../SensorProxy.h"

namespace ctx {

	class PressureLogger : public SensorLogger, public SensorProxy {
	public:
		PressureLogger();
		~PressureLogger();

		bool start();
		void stop();
		void flushCache(bool force = false);

	protected:
		void onEvent(sensor_data_t *eventData);

	private:
		void __record(float pressure, uint64_t eventTime);
		void __resetInsertionQuery();

		uint64_t __lastEventTime;
		uint32_t __cacheCount;
		std::string __insertionQuery;
	};
}

#endif /* __CONTEXT_PRESSURE_LOGGER_H__ */
