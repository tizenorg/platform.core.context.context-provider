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

#ifndef _PREDICTION_ASSOC_RULE_MINER_H_
#define _PREDICTION_ASSOC_RULE_MINER_H_

#include <list>
#include <string>
#include "event_set.h"
#include "assoc_rule.h"
#include "assoc_rule_of_ids.h"
#include "basket_producer.h"
#include "i_item_id_filter.h"

namespace ctx {

	class AssocRuleMiner {

	public:
		static std::list<AssocRule> mine_from_events(
				const EventSet& events,
				double minSupport,
				double minConfidence,
				std::string consequentCategory);
		static std::list<AssocRule> mine_from_baskets(
				std::list<Basket>& initBaskets,
				ItemCatalogue& itemCatalogue,
				double minSupport,
				double minConfidence,
				std::string consequentCategory);
	private:
		static std::list<AssocRule> remove_assoc_rule_ids(const std::list<AssocRuleOfIds>& rulesOfIds, const ItemCatalogue& itemCatalogue);
		static ItemSet remove_assoc_rule_ids(const ItemIdSet& itemIdSet, const ItemCatalogue& itemCatalogue);

	};

}	/* namespace ctx */

#endif /* _PREDICTION_ASSOC_RULE_MINER_H_ */
