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

#include "assoc_rule_miner.h"
#include "single_category_item_id_filter.h"
#include "basket_filter.h"
#include "basket_compressor.h"
#include "baskets_agregator.h"
#include "assoc_rule_producer.h"
#include "weight_apriori.h"

std::list<ctx::AssocRule> ctx::AssocRuleMiner::mine_from_baskets(
		std::list<ctx::Basket>& initBaskets,
		ctx::ItemCatalogue& itemCatalogue,
		double minSupport,
		double minConfidence,
		std::string consequentCategory)
{
	SingleCategoryItemIdFilter itemIdFilter(consequentCategory, itemCatalogue);
	BasketFilter::filter_in_place(initBaskets,itemIdFilter);

	std::list<Basket> compressedBaskets = BasketCompressor::compress(initBaskets);

	BasketsAgregator basketsAgregator(compressedBaskets);
	basketsAgregator.generate_bitsets(itemCatalogue.maxId());
	auto freqItemSets = WeightApriori::find_frequent_itemid_sets(basketsAgregator, minSupport);
	auto rulesOfIds = AssocRuleProducer::generate_rules(freqItemSets, itemIdFilter, minConfidence);
	return remove_assoc_rule_ids(rulesOfIds, itemCatalogue);
}

std::list<ctx::AssocRule> ctx::AssocRuleMiner::mine_from_events(
		const ctx::EventSet& events,
		double minSupport,
		double minConfidence,
		std::string consequentCategory)
{
	ItemCatalogue itemCatalogue;

	BasketProducer basketProducer(itemCatalogue);
	for (auto& event : events) {
		basketProducer.put_event(event);
	}

	std::list<Basket> initBaskets = basketProducer.make_baskets();

	return mine_from_baskets(initBaskets, itemCatalogue, minSupport, minConfidence, consequentCategory);
}

std::list<ctx::AssocRule> ctx::AssocRuleMiner::remove_assoc_rule_ids(
		const std::list<ctx::AssocRuleOfIds>& rulesOfIds,
		const ctx::ItemCatalogue& itemCatalogue)
{
	std::list<AssocRule> result;
	for (const AssocRuleOfIds ruleOfIds : rulesOfIds) {
		AssocRule rule(remove_assoc_rule_ids(ruleOfIds.antecedent, itemCatalogue),
					remove_assoc_rule_ids(ruleOfIds.consequent, itemCatalogue),
					ruleOfIds.support,
					ruleOfIds.confidence);
		result.push_back(rule);
	}
	return result;
}

ctx::ItemSet ctx::AssocRuleMiner::remove_assoc_rule_ids(const ctx::ItemIdSet& itemIdSet, const ctx::ItemCatalogue& itemCatalogue)
{
	ItemSet itemSet;
	for (int id : itemIdSet) {
		itemSet.push_back(itemCatalogue.item_of_id(id));
	}
	return itemSet;
}
