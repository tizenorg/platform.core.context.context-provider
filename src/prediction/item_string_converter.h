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

#ifndef _PREDICTION_ITEM_STRING_CONVERTER_H_
#define _PREDICTION_ITEM_STRING_CONVERTER_H_

#include "item.h"

namespace ctx {

	class ItemStringConverter {

	public:
		static const char SEPARATOR = ':';
		static std::string item_to_string(const Item &item);
		static Item string_to_item(const std::string& s);

	};

}	/* namespace ctx */

#endif /* _PREDICTION_ITEM_STRING_CONVERTER_H_ */
