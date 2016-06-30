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

#include <time.h>
#include <sys/time.h>
#include <cmath>
#include "TypesInternal.h"
#include "TimeUtil.h"

using namespace ctx;

TimeUtil::TimeUtil()
{
}

uint64_t TimeUtil::getTime()
{
	struct timeval tv;
	double timestamp;

	gettimeofday(&tv, NULL);
	timestamp = tv.tv_sec * 1000.0 + tv.tv_usec / 1000.0;
	return static_cast<uint64_t>(round(timestamp));
}

uint64_t TimeUtil::getTime(unsigned long long monotonic)
{
	struct timespec ts;
	double timestamp;
	uint64_t currentTime = getTime();

	if (std::abs(currentTime / 1000 - monotonic / 1000000) < SECONDS_PER_DAY)
		return static_cast<uint64_t>(round(monotonic / 1000.0));

	clock_gettime(CLOCK_MONOTONIC, &ts);
	timestamp = static_cast<double>(currentTime) - (ts.tv_sec * 1000000000.0 + ts.tv_nsec) / 1000000.0 + monotonic / 1000.0;
	return static_cast<uint64_t>(round(timestamp));
}
