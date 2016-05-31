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

#ifndef _CONTEXT_PLACE_RECOGNITION_MAHAL_H_
#define _CONTEXT_PLACE_RECOGNITION_MAHAL_H_

#include <vector>
#include "user_places_types.h"

namespace ctx {

	/*
	 * Class for Mahalanobis distance computation.
	 */
	class MahalModel {

	private:
		std::vector<num_t> __mean;
		std::vector<num_t> __sigma; // represents square matrix row-wise
		static num_t __distance(const std::vector<num_t> &v1, const std::vector<num_t> &v2, const std::vector<num_t> &m);

	public:
		MahalModel(std::vector<num_t> mean, std::vector<num_t> sigma) :
			__mean(mean),
			__sigma(sigma) { }
		num_t distance(const std::vector<num_t> &v);

	};	/* class MahalModel */

}	/* namespace ctx */

#endif /* End of _CONTEXT_PLACE_RECOGNITION_MAHAL_H_ */
