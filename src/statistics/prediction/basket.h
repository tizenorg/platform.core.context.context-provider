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

#ifndef INCLUDE_BASKET_H_
#define INCLUDE_BASKET_H_

#include "item_id_set.h"
#include <vector>

namespace ctx {

	class Basket {

	public:
		ItemIdSet itemIdSet;
		int weight;

		Basket(const ItemIdSet& itemIdSet_, const int& weight_) : itemIdSet(itemIdSet_), weight(weight_) {};
		void compute_bitset(int maxId);
		bool includes(const ItemIdSet& potentialSubset); // compute_bitset() should be invoked first

	private:
		std::vector<bool> mBitSet;

	};

	// TODO If there is no "inline" below the project does not compile.
	inline bool operator==(const Basket& left, const Basket& right)
	{
		return left.weight == right.weight && left.itemIdSet == right.itemIdSet;
	}

}	/* namespace ctx */

#endif /* INCLUDE_BASKET_H_ */
