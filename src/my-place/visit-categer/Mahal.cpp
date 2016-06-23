/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd.
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

#include "Mahal.h"
#include <math.h>
#include <Types.h>

ctx::num_t ctx::MahalModel::__distance(const std::vector<num_t> &v1, const std::vector<num_t> &v2, const std::vector<num_t> &m)
{
	size_t n = v1.size();
	if (m.size() != n * n) {
		_E("m.size() != n * n");
		return 0.0; // this value does not make any sense
	}

	std::vector<num_t> diff(n);
	for (size_t i = 0; i < n; i++) {
		diff[i] = v2[i] - v1[i];
	}

	num_t dist2 = 0; // squared distance
	for (size_t j = 0; j < n; j++) {
		for (size_t i = 0; i < n; i++) {
			dist2 += m[i * n + j] * diff[i] * diff[j];
		}
	}
	return sqrt(dist2);
}

ctx::num_t ctx::MahalModel::distance(const std::vector<ctx::num_t> &v)
{
	return __distance(v, __mean, __sigma);
}
