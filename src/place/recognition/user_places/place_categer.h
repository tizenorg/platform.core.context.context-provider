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

#ifndef __CONTEXT_PLACE_STATUS_PLACE_CATEGER__
#define __CONTEXT_PLACE_STATUS_PLACE_CATEGER__

#include "user_places_types.h"
#include <utility>
#include <vector>
#include <string>
#include "../place_recognition_types.h"

namespace ctx {

	class PlaceCateger {

	private:
		static bool reduce_category(const place_categ_id_e &categ, const ctx::visits_t &visits);

	public:
		static void reduce_outliers(visits_t &visits);
		static std::vector<ctx::num_t> categ_vector_from_visits(const ctx::visits_t &visits, place_categ_id_e categ_id);
		static void categorize(ctx::visits_t &visits, ctx::Place &place);
		static std::string categ_id_to_name(place_categ_id_e categ_id);

	};	/* class PlaceCateger */

}	/* namespace ctx */

#endif /* __CONTEXT_PLACE_STATUS_PLACE_CATEGER__ */
