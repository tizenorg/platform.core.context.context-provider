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

#ifndef _PREDICTION_BASKETS_AGREGATOR_H_
#define _PREDICTION_BASKETS_AGREGATOR_H_

#include <list>
#include <memory>
#include "basket.h"

namespace ctx {

	class BasketsAgregator {

	public:
		BasketsAgregator(std::list<Basket>& baskets);
		void generate_bitsets(int maxId);
		ItemIdSet unique_item_ids();
		double supportOf(const ItemIdSet &itemIdSet);

	private:
		std::list<Basket>& mBaskets;

	};

}	/* namespace ctx */

#endif /* _PREDICTION_BASKETS_AGREGATOR_H_ */
