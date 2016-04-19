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

#ifndef _PREDICTION_ITEM_CATALOGUE_H_
#define _PREDICTION_ITEM_CATALOGUE_H_

#include "item.h"
#include <map>
#include <set>
#include <vector>

namespace ctx {

	class ItemCatalogue {

	public:
		ItemCatalogue() : mMaxId(0) {
			mItemStrings.push_back(std::string(""));
		}

		int id_of(Item const &item);

		bool exists_item_of_id(int id) const;
		Item item_of_id(int id) const;
		std::set<int> category_item_ids(std::string category) const;
		int maxId() const {return mMaxId;};

	private:
		/* Translates item string to its id */
		std::map<std::string,int> mItemIdsOfStrings;

		/* Translating category string to int */
		std::map<std::string,std::set<int>> mCategoryItemIds;

		/* Stores item strings (index is item's id) */
		std::vector<std::string> mItemStrings;

		/* Preserves maximal item's id */
		int mMaxId;

		void extendCategoryItemIds(std::string category, int itemId);

	};

}	/* namespace ctx */

#endif /* _PREDICTION_ITEM_CATALOGUE_H_ */
