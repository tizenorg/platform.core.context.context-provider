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
#include "HeartRateLogger.h"
#include "HeartRateQuerier.h"
#include "HeartRate.h"

using namespace ctx;

HeartRateProvider::HeartRateProvider() :
	SensorProvider(SUBJ_SENSOR_HEART_RATE)
{
	IF_FAIL_VOID(isSupported());

	sensorLogger = new(std::nothrow) HeartRateLogger();
	IF_FAIL_VOID_TAG(sensorLogger, _E, "Memory allocation failed");
}

HeartRateProvider::~HeartRateProvider()
{
}

void HeartRateProvider::getPrivilege(std::vector<const char*> &privilege)
{
	privilege.push_back(PRIV_HEALTHINFO);
}

bool HeartRateProvider::isSupported()
{
	return util::getSystemInfoBool("tizen.org/feature/sensor.heart_rate_monitor");
}

Querier* HeartRateProvider::getQuerier(Json option)
{
	HeartRateQuerier *querier = new(std::nothrow) HeartRateQuerier(this, option);
	IF_FAIL_RETURN_TAG(querier, NULL, _E, "Memory allocation failed");
	return querier;
}
