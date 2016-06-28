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

#ifndef __CONTEXT_PEDOMETER_LOGGER_H__
#define __CONTEXT_PEDOMETER_LOGGER_H__

#include "SensorLogger.h"
#include "SensorProxy.h"

namespace ctx {

	class PedometerLogger : public SensorLogger, public SensorProxy {
	public:
		PedometerLogger();
		~PedometerLogger();

		bool start();
		void stop();

	protected:
		void onEvent(sensor_data_t *eventData);

	private:
		struct DataRecord {
			uint64_t timestamp;
			unsigned int walkSteps;
			unsigned int runSteps;
			float distance;
			float calories;
		};

		void __recordSingle(sensor_pedometer_data_t *eventData, uint64_t timestamp);
		void __recordBatch(sensor_pedometer_data_t *eventData, uint64_t timestamp);

		bool __firstEvent;
		DataRecord __baseline;
	};
}

#endif /* __CONTEXT_PEDOMETER_LOGGER_H__ */
