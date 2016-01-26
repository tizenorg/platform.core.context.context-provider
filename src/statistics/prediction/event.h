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

#ifndef PREDICTION_EVENT_H_
#define PREDICTION_EVENT_H_

#include "item.h"
#include "interval.h"

namespace ctx {

	struct Event {
		Item item;
		Interval interval;

		/* long time event */
		Event(const Item& item_, const Interval& interval_) : item(item_), interval(interval_) {}

		/* instant event */
		Event(const Item& item_, const time_t& start_time_) : item(item_), interval(start_time_, start_time_ + 1) {}
	};

	bool operator==(const Event& left, const Event& right);
	bool operator!=(const Event& left, const Event& right);

}	/* namespace ctx */

#endif /* PREDICTION_EVENT_H_ */
