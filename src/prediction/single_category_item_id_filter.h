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

#ifndef _PREDICTION_SINGLE_CATEGORY_ITEM_ID_FILTER_H_
#define _PREDICTION_SINGLE_CATEGORY_ITEM_ID_FILTER_H_

#include "i_item_id_filter.h"
#include "item_catalogue.h"

namespace ctx {

	class SingleCategoryItemIdFilter : public IItemIdFilter {

	public:
		SingleCategoryItemIdFilter(std::string category, const ItemCatalogue& itemCatalogue);
		bool pass(int itemId) const override;

	private:
		std::set<int> mAcceptedItemIds;

	};

}	/* namespace ctx */

#endif /* _PREDICTION_SINGLE_CATEGORY_ITEM_ID_FILTER_H_ */
