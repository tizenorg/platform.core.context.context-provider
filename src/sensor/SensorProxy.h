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
 *
 */

#ifndef __CONTEXT_SENSOR_PROXY_H__
#define __CONTEXT_SENSOR_PROXY_H__

#include <sensor_internal.h>

namespace ctx {

	class SensorProxy {
	public:
		SensorProxy();
		virtual ~SensorProxy();

		void setSensor(sensor_type_t type);
		void setPowerSave(bool ps);
		void setSamplingInterval(unsigned int interval);
		void setBatchLatency(unsigned int latency);
		void setUserData(void *data);

		virtual bool start();
		virtual void stop();
		bool isRunning();
		void flush();

	protected:
		int sensorHandle;
		sensor_type_t sensorType;
		bool powerSave;
		unsigned int samplingInterval;
		unsigned int batchLatency;
		void *userData;

		static bool isSupported(sensor_type_t type);

		virtual void onEvent(sensor_data_t *eventData) = 0;

	private:
		static void __eventCb(sensor_t sensor, unsigned int eventType, sensor_data_t *eventData, void *cbData);
	};

}

#endif /* __CONTEXT_SENSOR_PROXY_H__ */
