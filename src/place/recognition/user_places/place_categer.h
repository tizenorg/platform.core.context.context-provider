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

#ifndef _CONTEXT_PLACE_RECOGNITION_PLACE_CATEGER_
#define _CONTEXT_PLACE_RECOGNITION_PLACE_CATEGER_

#include "user_places_types.h"
#include <utility>
#include <vector>
#include <string>
#include "../place_recognition_types.h"

namespace ctx {

	class PlaceCateger {

	private:
		static bool __reduceCategory(const PlaceCategId &categId, const ctx::Visits &visits);

	public:
		static void reduceOutliers(Visits &visits); // TODO: move to private
		static std::vector<ctx::num_t> categVectorFromVisits(const ctx::Visits &visits, PlaceCategId categId); // TODO: move to private
		static void categorize(ctx::Visits &visits, ctx::Place &place);
		static std::string categId2Name(PlaceCategId categId); // TODO: move to private

	};	/* class PlaceCateger */

}	/* namespace ctx */

#endif /* End of _CONTEXT_PLACE_RECOGNITION_PLACE_CATEGER_ */
