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

#include "single_category_item_id_filter.h"

ctx::SingleCategoryItemIdFilter::SingleCategoryItemIdFilter(std::string category, const ctx::ItemCatalogue &itemCatalogue)
{
	mAcceptedItemIds = itemCatalogue.category_item_ids(category);
}

bool ctx::SingleCategoryItemIdFilter::pass(int itemId) const
{
	return mAcceptedItemIds.find(itemId) != mAcceptedItemIds.end();
}
