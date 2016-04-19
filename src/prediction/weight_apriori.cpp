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

#include "weight_apriori.h"
#include <algorithm>
#include <iostream>

std::list<ctx::ItemIdSet> ctx::WeightApriori::find_wider_candidates(const std::list<ctx::ItemIdSet> &narrowFreqSets, int widerSize)
{
	std::list<ItemIdSet> candidates;
	for (auto iter1 = narrowFreqSets.begin(); iter1 != narrowFreqSets.end(); ++iter1) {
		auto iter2 = iter1; ++iter2;
		for (;iter2 != narrowFreqSets.end(); ++iter2) {
			ItemIdSet itemIdSet1 = *iter1;
			ItemIdSet itemIdSet2 = *iter2;
			itemIdSet1.merge(itemIdSet2);
			itemIdSet1.unique();
			if (int(itemIdSet1.size()) == widerSize) {
				candidates.push_back(itemIdSet1);
			}
		}
	}
	candidates.sort();
	candidates.unique();
	return candidates;
}

void ctx::WeightApriori::prune_wider_candidates(std::list<ctx::ItemIdSet>& widerCandidates, const std::list<ctx::ItemIdSet>& narrowFreqSets)
{
	std::list<ItemIdSet> pruned;
	for (auto iter = widerCandidates.begin(); iter != widerCandidates.end();) {
		if (survives_prunning(*iter, narrowFreqSets)) {
			++iter;
		} else {
			iter = widerCandidates.erase(iter);
		}
	}
}

bool ctx::WeightApriori::survives_prunning(ctx::ItemIdSet& widerCandidate, const std::list<ctx::ItemIdSet>& narrowFreqSets)
{
	int removedItemId = widerCandidate.front();
	widerCandidate.pop_front();
	for (auto iter = widerCandidate.begin();; ++iter) {
		if (std::find(narrowFreqSets.begin(), narrowFreqSets.end(), widerCandidate) == narrowFreqSets.end()) {
			return false; // Caution: In this case the original contents of widerCandidate is not preserved.
		}
		if (iter == widerCandidate.end()) {
			widerCandidate.push_back(removedItemId); // Restoring original contents of widerCandidate.
			return true;
		}
		int newRemovedItemId = *iter;
		*iter = removedItemId;
		removedItemId = newRemovedItemId;
	}
}

std::list<std::pair<ctx::ItemIdSet, double>> ctx::WeightApriori::find_frequent_itemid_sets(ctx::BasketsAgregator &basketAgregator, double supportThreshold)
{
	std::list<std::pair<ItemIdSet, double>> allFreqItemsets;
	std::list<ItemIdSet> freqItemsetsOfCurrSize;
	std::list<ItemIdSet> candidates;

	int candidateSize = 1;
	candidates = single_size_candidates(basketAgregator);

	while (!candidates.empty()) {
		for (ItemIdSet candidate : candidates) {
			double support = basketAgregator.supportOf(candidate);
			if (support >= supportThreshold) {
				freqItemsetsOfCurrSize.push_back(candidate);
				allFreqItemsets.push_back(std::make_pair(candidate,support));
			}
		}
		candidates = find_wider_candidates(freqItemsetsOfCurrSize, ++candidateSize);
		freqItemsetsOfCurrSize.clear();
	}
	return allFreqItemsets;
}

std::list<ctx::ItemIdSet> ctx::WeightApriori::single_size_candidates(ctx::BasketsAgregator& basketAgregator)
{
	std::list<ItemIdSet> candidates;
	for (int itemId : basketAgregator.unique_item_ids()) {
		candidates.push_back({itemId});
	}
	return candidates;
}
