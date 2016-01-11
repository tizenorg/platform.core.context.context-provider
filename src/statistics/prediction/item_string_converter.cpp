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

#include "item_string_converter.h"
#include <string>

std::string ctx::ItemStringConverter::item_to_string(const ctx::Item &item)
{
	return item.category + SEPARATOR + item.value;
}

ctx::Item ctx::ItemStringConverter::string_to_item(const std::string& s)
{
	size_t pos = s.find(SEPARATOR);
	//assert(pos != std::string::npos);
	return Item(s.substr(0, pos), s.substr(pos + 1));
}
