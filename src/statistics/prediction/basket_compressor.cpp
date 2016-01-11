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

#include "basket_compressor.h"
#include <sstream>
#include <map>

std::string ctx::BasketCompressor::itemset_string(const ctx::ItemIdSet& itemIdSet)
{
	std::stringstream ss;
	for (int itemId : itemIdSet) {
		ss << itemId << ',';
	}
	return ss.str();
}

std::list<ctx::Basket> ctx::BasketCompressor::compress(const std::list<ctx::Basket>& inputBaskets)
{
	std::map<std::string,Basket> basketsMap;
	// TODO: maybe it is worth to resign from string keys and use std::map<ItemSet,weight> instead.

	for (const Basket& basket : inputBaskets) {
		std::string itemSetStr = itemset_string(basket.itemIdSet);

		auto findResult = basketsMap.find(itemSetStr);
		if (findResult == basketsMap.end()) {
			basketsMap.insert(std::pair<std::string,Basket>(itemSetStr, basket));
		} else {
			Basket& compressedBasket = findResult->second;
			compressedBasket.weight += basket.weight;
		}
	}

	std::list<Basket> compressedBaskets;
	for (auto pair : basketsMap) {
		compressedBaskets.push_back(pair.second);
	}
	return compressedBaskets;
}
