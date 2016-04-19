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

#include "assoc_rule_producer.h"

ctx::AssocRuleOfIds ctx::AssocRuleProducer::rule_template(const ctx::ItemIdSet& sourceItemIdSet,
		const ctx::IItemIdFilter& itemIdFilter)
{
	AssocRuleOfIds ruleTempl;
	for (int itemId : sourceItemIdSet) {
		ItemIdSet &destination = itemIdFilter.pass(itemId)
				? ruleTempl.consequent
				: ruleTempl.antecedent;
		destination.push_back(itemId);
	}
	return ruleTempl;
}

std::list<ctx::AssocRuleOfIds> ctx::AssocRuleProducer::generate_rules(
		const std::list<std::pair<ctx::ItemIdSet, double>>& freqItemIdSets,
		const ctx::IItemIdFilter& itemIdFilter, double minConfidence)
{
	std::list<AssocRuleOfIds> rules;

	for (auto & pair : freqItemIdSets) {
		ItemIdSet freqItemIdSet = pair.first;
		double support = pair.second;
		AssocRuleOfIds rule = AssocRuleProducer::rule_template(freqItemIdSet, itemIdFilter);
		if (rule.antecedent.size() > 0 && rule.consequent.size() > 0) {
			rule.support = support;
			double antecedentSupport = AssocRuleProducer::supportOf(freqItemIdSets, rule.antecedent);
			rule.confidence = support / antecedentSupport;
			if (rule.confidence >= minConfidence) {
				rules.push_back(rule);
			}
		}
	}
	return rules;
}

double ctx::AssocRuleProducer::supportOf(
		const std::list<std::pair<ctx::ItemIdSet, double>> &allFreqItemIdSets, const ctx::ItemIdSet &wanted)
{
	for (auto & pair : allFreqItemIdSets) {
		if (pair.first == wanted) {
			return pair.second;
		}
	}
	return 0.0;
}
