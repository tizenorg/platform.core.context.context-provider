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

#ifndef _PREDICTION_BASKET_PRODUCER_H_
#define _PREDICTION_BASKET_PRODUCER_H_

#include "event.h"
#include "basket.h"
#include "item_catalogue.h"
#include <list>

namespace ctx {

	class BasketProducer {

	public:
		BasketProducer(ItemCatalogue& itemCatalogue) : mItemCatalogue(itemCatalogue) {};
		void put_event(const Event& e);
		std::list<Basket> make_baskets();

	private:
		void put_change(time_t time, int value);

		ItemCatalogue& mItemCatalogue;

		/*
		 * Changes in timestamps.
		 * If an item starts than its id is stored.
		 * If an item ends than its negated id is stored.
		 */
		std::map<time_t, std::list<int>> mChanges;

	};

}	/* namespace ctx */

#endif /* _PREDICTION_BASKET_PRODUCER_H_ */
