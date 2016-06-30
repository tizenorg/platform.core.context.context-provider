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

#include <SensorRecorderTypes.h>
#include <Util.h>
#include "../TypesInternal.h"
#include "PedometerLogger.h"
#include "PedometerQuerier.h"
#include "Pedometer.h"

using namespace ctx;

PedometerProvider::PedometerProvider() :
	SensorProvider(SUBJ_SENSOR_PEDOMETER)
{
	IF_FAIL_VOID(isSupported());

	sensorLogger = new(std::nothrow) PedometerLogger();
	IF_FAIL_VOID_TAG(sensorLogger, _E, "Memory allocation failed");
}

PedometerProvider::~PedometerProvider()
{
}

void PedometerProvider::getPrivilege(std::vector<const char*> &privilege)
{
	privilege.push_back(PRIV_HEALTHINFO);
}

bool PedometerProvider::isSupported()
{
	return util::getSystemInfoBool("tizen.org/feature/sensor.pedometer");
}

Querier* PedometerProvider::getQuerier(Json option)
{
	PedometerQuerier *querier = new(std::nothrow) PedometerQuerier(this, option);
	IF_FAIL_RETURN_TAG(querier, NULL, _E, "Memory allocation failed");
	return querier;
}
