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

#ifndef __CONTEXT_PLACE_STATUS_SIMILAR_H__
#define __CONTEXT_PLACE_STATUS_SIMILAR_H__

#include "user_places_types.h"

namespace ctx {

	/* similarity functions */

	template <class T> ctx::share_t overlap_first_over_second(const T &s1, const T &s2)
	{
		if (s2.empty()) {
			return 0;
		}
		int count = 0;
		for (auto e : s2) {
			if (s1.find(e) != s1.end()) {
				count++;
			}
		}
		return (ctx::share_t) count / s2.size();
	}

	template <class T> ctx::share_t overlap_bigger_over_smaller(const T &s1, const T &s2)
	{
		if (s1.size() > s2.size()) {
			return overlap_first_over_second(s1, s2);
		} else {
			return overlap_first_over_second(s2, s1);
		}
	}

	template <class T> bool is_joint(const T &s1, const T &s2)
	{
		for (auto e : s2) {
			if (s1.find(e) != s1.end()) {
				return true;
			}
		}
		return false;
	}

}	/* namespace ctx */

#endif /* __CONTEXT_PLACE_STATUS_SIMILAR_H__ */
