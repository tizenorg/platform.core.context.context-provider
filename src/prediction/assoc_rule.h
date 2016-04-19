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

#ifndef _PREDICTION_ASSOC_RULE_H_
#define _PREDICTION_ASSOC_RULE_H_

#include "item_set.h"
#include <iostream>

namespace ctx {

	struct AssocRule {
		ItemSet antecedent;
		ItemSet consequent;
		double support;
		double confidence;

		AssocRule(const ItemSet& antecedent_,
				const ItemSet& consequent_,
				const double& support_,
				const double& confidence_)
				: antecedent(antecedent_)
				, consequent(consequent_)
				, support(support_)
				, confidence(confidence_) {}
	};

	bool operator==(const AssocRule& left, const AssocRule& right);

	std::ostream& operator<<(std::ostream &out, const AssocRule &rule);

}	/* namespace ctx */

#endif /* _PREDICTION_ASSOC_RULE_H_ */
