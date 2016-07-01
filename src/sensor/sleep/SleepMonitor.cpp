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
#include "../TypesInternal.h"
#include "../TimeUtil.h"
#include "SleepMonitor.h"

#define IDX_SLEEP_STATE	0
#define IDX_REMAINING	6

using namespace ctx;

SleepMonitor::SleepMonitor(SleepLogger *logger) :
	__logger(logger),
	__lazyStopOn(false)
{
	setSensor(HUMAN_SLEEP_MONITOR_SENSOR);
	setPowerSave(false);
}

SleepMonitor::~SleepMonitor()
{
}

bool SleepMonitor::start()
{
	_D("START");
	__lazyStopOn = false;
	return listen();
}

void SleepMonitor::stop()
{
	_D("STOP");
	unlisten();
}

void SleepMonitor::lazyStop()
{
	if (!isRunning())
		return;

	__lazyStopOn = true;
}

void SleepMonitor::onEvent(sensor_data_t *eventData)
{
	_D("%d at %llu", (int)eventData->values[IDX_SLEEP_STATE], eventData->timestamp);

	uint64_t timestamp = TimeUtil::getTime(eventData->timestamp);
	__logger->record(timestamp - SEC_TO_MS(SECONDS_PER_MINUTE), timestamp,
			static_cast<int>(eventData->values[IDX_SLEEP_STATE]));

	if (static_cast<int>(eventData->values[IDX_REMAINING]) > 0)
		return;

	__logger->flushCache();

	if (__lazyStopOn)
		stop();
}
