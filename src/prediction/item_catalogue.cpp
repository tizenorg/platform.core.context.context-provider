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

#include "item_catalogue.h"
#include "item_string_converter.h"

int ctx::ItemCatalogue::id_of(const ctx::Item& item)
{
	std::string s = ItemStringConverter::item_to_string(item);
	std::map<std::string,int>::iterator it = mItemIdsOfStrings.find(s);
	if (it == mItemIdsOfStrings.end()) { // new item
		mMaxId++;
		mItemIdsOfStrings[s] = mMaxId;
		mItemStrings.push_back(s);
		extendCategoryItemIds(item.category, mMaxId);
		return mMaxId;
	} else { // existing item
		return mItemIdsOfStrings[s];
	}
}

bool ctx::ItemCatalogue::exists_item_of_id(int id) const
{
	return id > 0 && id <= mMaxId;
}

ctx::Item ctx::ItemCatalogue::item_of_id(int id) const
{
	return ItemStringConverter::string_to_item(mItemStrings[id]);
}

std::set<int> ctx::ItemCatalogue::category_item_ids(std::string category) const
{
	auto it = mCategoryItemIds.find(category);
	if (it == mCategoryItemIds.end()) {
		return std::set<int>();
	} else {
		return it->second;
	}
}

void ctx::ItemCatalogue::extendCategoryItemIds(std::string category, int itemId)
{
	if (mCategoryItemIds.find(category) == mCategoryItemIds.end()) {
		mCategoryItemIds[category] = std::set<int>();
	}
	mCategoryItemIds[category].insert(itemId);
}
