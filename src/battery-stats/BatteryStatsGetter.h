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

#ifndef _CONTEXT_BATTERY_STATS_GETTER_H_
#define _CONTEXT_BATTERY_STATS_GETTER_H_
#include "DbInit.h"
#include <map>
#include <utility>
#include <vector>
#include <string>
#include <time.h>
typedef std::vector<std::pair<std::string,int> > vec;
namespace ctx {
	class BatteryStatsGetter {
	public:
		BatteryStatsGetter();
		~BatteryStatsGetter();
		vec calculatePercentage (time_t start_time, time_t end_time);
	private:
		std::string make_query(time_t start_time, time_t end_time);
		vec getAppTimes (time_t start_time, time_t end_time);
	};

}	/* namespace ctx */

#endif	/* _CONTEXT_BATTERY_STATS_GETTER_H_ */
