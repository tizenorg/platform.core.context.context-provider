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

#ifndef __CONTEXT_PLACE_STATUS_PIECEWISE_LIN__
#define __CONTEXT_PLACE_STATUS_PIECEWISE_LIN__

#include "user_places_types.h"

namespace ctx {

	/*
	 * Piecewise linear function. Used for approximation.
	 */
	class PiecewiseLin {

	private:
		std::vector<num_t> xs; // nodes
		std::vector<num_t> vs; // values in nodes
		size_t n;

	public:
		PiecewiseLin(std::vector<num_t> _xs, std::vector<num_t> _vs);
		num_t val(num_t x);

	}; 	/* PiecewiseLin */

}	/* namespace ctx */

#endif /* __CONTEXT_PLACE_STATUS_PIECEWISE_LIN__ */
