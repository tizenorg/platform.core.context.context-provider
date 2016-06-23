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

#ifndef _CONTEXT_PLACE_RECOGNITION_PIECEWISE_LIN_
#define _CONTEXT_PLACE_RECOGNITION_PIECEWISE_LIN_

#include "../facade/UserPlacesTypes.h"

namespace ctx {

	/*
	 * Piecewise linear function. Used for approximation.
	 */
	class PiecewiseLin {

	private:
		std::vector<num_t> __xs; // nodes
		std::vector<num_t> __vs; // values in nodes
		size_t __n;

	public:
		PiecewiseLin(std::vector<num_t> xs, std::vector<num_t> vs);
		num_t value(num_t x);

	}; 	/* PiecewiseLin */

}	/* namespace ctx */

#endif /* End of _CONTEXT_PLACE_RECOGNITION_PIECEWISE_LIN_ */
