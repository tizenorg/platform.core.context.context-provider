/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd.
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

#ifndef INCLUDE_INTERVAL_H_
#define INCLUDE_INTERVAL_H_

#include <ctime>

namespace ctx {

	struct Interval {
		time_t start;
		time_t end;

		Interval(time_t start_, time_t end_) : start(start_), end(end_) {}
	};

	bool operator==(const Interval& left, const Interval& right);
	bool operator!=(const Interval& left, const Interval& right);

}	/* namespace ctx */

#endif /* INCLUDE_INTERVAL_H_ */
