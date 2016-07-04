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

#include <Types.h>
#include "../TimeUtil.h"
#include "SleepDetector.h"

#define IDX_SLEEP_STATE	0

using namespace ctx;

SleepDetector::SleepDetector(SleepLogger *logger) :
	__logger(logger)
{
	setSensor(HUMAN_SLEEP_DETECTOR_SENSOR);
	setPowerSave(false);
}

SleepDetector::~SleepDetector()
{
}

bool SleepDetector::start()
{
	_D("START");
	return listen();
}

void SleepDetector::stop()
{
	_D("STOP");
	unlisten();
}

void SleepDetector::onEvent(sensor_data_t *eventData)
{
	_D("%d at %llu", (int)eventData->values[0], eventData->timestamp);

	uint64_t eventTime = TimeUtil::getTime(eventData->timestamp);

	if (static_cast<int>(eventData->values[IDX_SLEEP_STATE]))
		__logger->fallAsleep(eventTime);
	else
		__logger->wakeUp(eventTime);
}
