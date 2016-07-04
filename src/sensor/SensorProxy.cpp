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

#include <Types.h>
#include "SensorProxy.h"

#define SENSOR_EVENT(X) (((int)(X) << 16) | 0x01)

using namespace ctx;

SensorProxy::SensorProxy() :
	sensorHandle(-1),
	sensorType(UNKNOWN_SENSOR),
	powerSave(true),
	samplingInterval(0),
	batchLatency(0),
	userData(NULL)
{
}

SensorProxy::~SensorProxy()
{
	unlisten();
}

void SensorProxy::setSensor(sensor_type_t type)
{
	sensorType = type;
}

void SensorProxy::setPowerSave(bool ps)
{
	powerSave = ps;
}

void SensorProxy::setSamplingInterval(unsigned int interval)
{
	samplingInterval = interval;
}

void SensorProxy::setBatchLatency(unsigned int latency)
{
	batchLatency = latency;
}

void SensorProxy::setUserData(void *data)
{
	userData = data;
}

bool SensorProxy::listen()
{
	_D("#Sensor = %#x", sensorType);

	sensor_t sensor = sensord_get_sensor(sensorType);
	IF_FAIL_RETURN_TAG(sensor, false, _E, "Getting sensor failed");

	sensorHandle = sensord_connect(sensor);
	IF_FAIL_RETURN_TAG(sensorHandle >= 0, false, _E, "Connection failed");

	if (!sensord_register_event(sensorHandle, SENSOR_EVENT(sensorType), samplingInterval, batchLatency, __eventCb, this)) {
		_E("Event registration failed");
		sensord_disconnect(sensorHandle);
		sensorHandle = -1;
		return false;
	}

	if (!sensord_start(sensorHandle, powerSave ? SENSOR_OPTION_DEFAULT : SENSOR_OPTION_ALWAYS_ON)) {
		_E("Starting failed");
		sensord_unregister_event(sensorHandle, SENSOR_EVENT(sensorType));
		sensord_disconnect(sensorHandle);
		sensorHandle = -1;
		return false;
	}

	return true;
}

void SensorProxy::unlisten()
{
	IF_FAIL_VOID(sensorHandle >= 0);

	sensord_stop(sensorHandle);
	sensord_unregister_event(sensorHandle, SENSOR_EVENT(sensorType));
	sensord_disconnect(sensorHandle);
	sensorHandle = -1;
}

void SensorProxy::flush()
{
	IF_FAIL_VOID(sensorHandle >= 0);
	sensord_flush(sensorHandle);
}

bool SensorProxy::isRunning()
{
	return sensorHandle >= 0;
}

bool SensorProxy::isSupported(sensor_type_t type)
{
	sensor_t sensor = sensord_get_sensor(type);
	return (sensor != NULL);
}

void SensorProxy::__eventCb(sensor_t sensor, unsigned int eventType, sensor_data_t *eventData, void *cbData)
{
	SensorProxy *instance = static_cast<SensorProxy*>(cbData);
	instance->onEvent(eventData);
}
