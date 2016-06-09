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

#include "PiecewiseLin.h"
#include <Types.h>

ctx::PiecewiseLin::PiecewiseLin(std::vector<num_t> xs, std::vector<num_t> vs) :
	__xs(xs),
	__vs(vs),
	__n(xs.size())
{
	if (xs.size() != vs.size()) {
		_E("Input arguments have different sizes");
		return;
	}
}

ctx::num_t ctx::PiecewiseLin::value(num_t x)
{
	if (x <= __xs[0]) {
		return __vs[0];
	} else if (x >= __xs[__n-1]) {
		return __vs[__n - 1];
	} else {
		num_t xp = __xs[0];
		for (size_t i = 1; i < __n; i++) {
			num_t xn = __xs[i];
			if (x <= xn) {
				num_t d = xn - xp;
				num_t dxp = x - xp;
				num_t dxn = xn - x;
				return (dxn * __vs[i-1] + dxp * __vs[i]) / d;
			}
			xp = xn;
		}
	}
	_E("Function should return result earlier");
	return 0.0; // this value does not make any sense
}
