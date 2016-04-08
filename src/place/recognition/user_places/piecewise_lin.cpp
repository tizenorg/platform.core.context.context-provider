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

#include "piecewise_lin.h"
#include <types_internal.h>

ctx::PiecewiseLin::PiecewiseLin(std::vector<num_t> _xs, std::vector<num_t> _vs)
	: n(0)
{
	if (_xs.size() != _vs.size()) {
		_E("Input arguments have different sizes");
		return;
	}
	xs = _xs;
	vs = _vs;
	n = xs.size();
}

ctx::num_t ctx::PiecewiseLin::val(num_t x)
{
	if (x <= xs[0]) {
		return vs[0];
	} else if (x >= xs[n-1]) {
		return vs[n - 1];
	} else {
		num_t xp = xs[0];
		for (size_t i = 1; i < n; i++) {
			num_t xn = xs[i];
			if (x <= xn) {
				num_t d = xn - xp;
				num_t dxp = x - xp;
				num_t dxn = xn - x;
				return (dxn * vs[i-1] + dxp * vs[i]) / d;
			}
			xp = xn;
		}
	}
	_E("Function should return result earlier");
	return 0.0; // this value does not make any sense
}
