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

#include "item.h"

ctx::Item::Item(const std::string &s)
{
	size_t leftBracket = s.find('[');
	size_t rightBracket = s.find(']', leftBracket);
	category = s.substr(leftBracket + 1, rightBracket - 1);
	value = s.substr(rightBracket + 1);
	return;
}

bool ctx::Item::operator==(const ctx::Item& other) const
{
	return other.category == category && other.value == value;
}

bool ctx::Item::operator!=(const ctx::Item& other) const
{
	return !operator==(other);
}

std::ostream& ctx::operator<<(std::ostream& out, const ctx::Item& item)
{
	return out << '[' << item.category << ']' << item.value;
}
