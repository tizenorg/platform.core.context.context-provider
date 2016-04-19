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

#ifndef _PREDICTION_WEIGHT_APRIORI_H_
#define _PREDICTION_WEIGHT_APRIORI_H_

#include "baskets_agregator.h"
#include <list>
#include <utility>

namespace ctx {

	class WeightApriori {

	public:
		static std::list<ItemIdSet> find_wider_candidates(const std::list<ItemIdSet> &narrowFreqSets, int widerSize);
		static void prune_wider_candidates(std::list<ItemIdSet> &widerCandidates, const std::list<ItemIdSet> &narrowFreqSets);

		static bool survives_prunning(ItemIdSet &widerCandidate, const std::list<ItemIdSet> &narrowFreqSets);

		/* Find frequent itemsets along with its support value. */
		static std::list<std::pair<ItemIdSet,double>> find_frequent_itemid_sets(BasketsAgregator &basketAgregator, double supportThreshold);

	private:
		static std::list<ItemIdSet> single_size_candidates(BasketsAgregator &basketAgregator);

	};

}	/* namespace ctx */

#endif /* _PREDICTION_WEIGHT_APRIORI_H_ */
