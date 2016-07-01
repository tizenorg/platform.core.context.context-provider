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

#ifndef __CONTEXT_SLEEP_MONITOR_H__
#define __CONTEXT_SLEEP_MONITOR_H__

#include "../SensorProxy.h"
#include "SleepLogger.h"

namespace ctx {

	class SleepMonitor : public SensorProxy {
	public:
		SleepMonitor(SleepLogger *logger);
		~SleepMonitor();

		bool start();
		void stop();

		void lazyStop();

	protected:
		void onEvent(sensor_data_t *eventData);

	private:
		SleepLogger *__logger;
		bool __lazyStopOn;
	};
}

#endif /* __CONTEXT_SLEEP_MONITOR_H__ */
