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

#include "assoc_rule.h"

std::ostream& ctx::operator<<(std::ostream &out, const ctx::AssocRule &rule)
{
	return out << rule.antecedent << " => " << rule.consequent
			<< "; support:" << rule.support << "; confidence:" << rule.confidence;
}

bool ctx::operator==(const ctx::AssocRule& left, const ctx::AssocRule& right)
{
	return	left.antecedent == right.antecedent &&
			left.consequent == right.consequent &&
			left.support == right.support &&
			left.confidence == right.confidence;
}
