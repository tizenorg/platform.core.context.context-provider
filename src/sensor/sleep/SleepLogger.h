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

#ifndef __CONTEXT_SLEEP_LOGGER_H__
#define __CONTEXT_SLEEP_LOGGER_H__

#include "../SensorLogger.h"
#include "../SensorProxy.h"

#define STATE_SLEEP	1
#define STATE_WAKE	0

namespace ctx {

	class SleepLogger : public SensorLogger {
	public:
		SleepLogger();
		~SleepLogger();

		bool start();
		void stop();

		void fallAsleep(uint64_t timestamp);
		void wakeUp(uint64_t timestamp);

		void record(uint64_t startTime, uint64_t endTime, int state);
		void flush();

	private:
		void __resetInsertionQuery();
		void __appendQuery(uint64_t startTime, uint64_t endTime);

		std::string __insertionQuery;
		SensorProxy *__sleepDetector;
		SensorProxy *__sleepMonitor;
		uint64_t __startTime;
		uint64_t __endTime;
	};
}

#endif /* __CONTEXT_SLEEP_LOGGER_H__ */
