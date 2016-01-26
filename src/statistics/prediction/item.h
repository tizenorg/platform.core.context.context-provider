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

#ifndef PREDICTION_ITEM_H_
#define PREDICTION_ITEM_H_

#include <string>
#include <iostream>

namespace ctx {

	struct Item {
		std::string category;
		std::string value;

		Item(const std::string &category_, const std::string &value_) : category(category_), value(value_) {}
		Item(const std::string &s); // in the form of "[Category]Value"

		bool operator==(const Item& other) const;
		bool operator!=(const Item& other) const;
	};

	std::ostream& operator<<(std::ostream& out, const Item& item);

}	/* namespace ctx */

namespace std {

	template <> struct hash<ctx::Item> {
		size_t operator()(const ctx::Item &item) const {
			hash<std::string> hasher;
			return hasher(item.category + ":" + item.value);
		}
	};

}

#endif /* PREDICTION_ITEM_H_ */
