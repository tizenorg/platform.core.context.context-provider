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

#ifndef __CONTEXT_PLACE_STATUS_MAHAL_H__
#define __CONTEXT_PLACE_STATUS_MAHAL_H__

#include <vector>
#include "user_places_types.h"

namespace ctx {

	/*
	 * Class for Mahalanobis distance computation.
	 */
	class MahalModel {

	private:
		std::vector<num_t> mean;
		std::vector<num_t> sigma; // represents square matrix row-wise

	public:
		static num_t dist_s(const std::vector<num_t> &v1, const std::vector<num_t> &v2, const std::vector<num_t> &m);
		MahalModel(std::vector<num_t> mean_, std::vector<num_t> sigma_) : mean(mean_), sigma(sigma_) { }
		num_t dist(const std::vector<num_t> &v);

	};	/* class MahalModel */

}	/* namespace ctx */

#endif /* __CONTEXT_PLACE_STATUS_MAHAL_H__ */
