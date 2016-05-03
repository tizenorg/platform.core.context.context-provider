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

#include "basket_producer.h"
#include <set>

void ctx::BasketProducer::put_event(const ctx::Event& event)
{
	int iid = mItemCatalogue.id_of(event.item);
	put_change(event.interval.start, iid);  // positive value indicates start
	put_change(event.interval.end,  -iid);  // negative value indicates end
}

std::list<ctx::Basket> ctx::BasketProducer::make_baskets()
{
	std::list<Basket> baskets;

	time_t intervalStart;
	std::set<int> currentItemIds;

	bool first = true;
	for (auto changePair : mChanges) {
		if (first) {
			first = false;
		} else {
			time_t intervalEnd = changePair.first;
			int weight = intervalEnd - intervalStart;
			ItemIdSet itemIdSet;
			for (int itemId : currentItemIds) {
				itemIdSet.push_back(itemId);
			}
			baskets.push_back(Basket(itemIdSet, weight));
		}

		for (int itemId : changePair.second) {
			if (itemId > 0) {  // item with itemId starts now
				currentItemIds.insert(itemId);
			} else {           // item with -itemId ends now
				currentItemIds.erase(-itemId);
			}
		}

		intervalStart = changePair.first;
	}

	return baskets;
}

void ctx::BasketProducer::put_change(time_t time, int value)
{
	if (mChanges.find(time) == mChanges.end()) {
		mChanges[time] = std::list<int>();
	}
	mChanges[time].push_back(value);
}
