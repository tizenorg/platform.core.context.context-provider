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

#include <algorithm>
#include <Types.h>
#include "median.h"

ctx::num_t ctx::median(std::vector<ctx::num_t> &v)
{
	if (v.empty()) {
		_E("Median of empty set");
		return 0; // this value does not make any sense
	}
	size_t n = v.size() / 2;
	std::nth_element(v.begin(), v.begin() + n, v.end());
	num_t vn = v[n];
	if (v.size() % 2 == 1) {
		return vn;
	} else {
		std::nth_element(v.begin(), v.begin() + n - 1, v.end());
		return 0.5 * (vn + v[n - 1]);
	}
}
