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

#ifndef __CONTEXT_HEARTRATE_LOGGER_H__
#define __CONTEXT_HEARTRATE_LOGGER_H__

#include <TimerManager.h>
#include "../SensorLogger.h"
#include "../SensorProxy.h"

#define MIN_MEASURING_INTERVAL	10		/* minutes */
#define MAX_MEASURING_INTERVAL	1440	/* minutes */

namespace ctx {

	class HeartRateLogger : public SensorLogger, public SensorProxy, public ITimerListener {
	public:
		HeartRateLogger();
		~HeartRateLogger();

		bool start();
		void stop();
		void flushCache(bool force = false);

	protected:
		bool onTimerExpired(int timerId);
		void onEvent(sensor_data_t *eventData);

	private:
		void __record(float heartrate, uint64_t eventTime);

		TimerManager *__timerMgr;
		int __timerId;
		int __timerInterval;
		uint64_t __expiredTime;
	};
}

#endif /* __CONTEXT_HEARTRATE_LOGGER_H__ */
