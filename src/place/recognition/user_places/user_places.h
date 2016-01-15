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

#ifndef __CONTEXT_PLACE_STATUS_USER_PLACES_ENGINE_H__
#define __CONTEXT_PLACE_STATUS_USER_PLACES_ENGINE_H__

#include "visit_detector.h"
#include "places_detector.h"
#include <vector>
#include "user_places_types.h"
#include <json.h>

namespace ctx {

	class UserPlaces {

	private:
		VisitDetector *visit_detector;
		PlacesDetector *places_detector;
		int places_detector_timer_id;

	public:
		UserPlaces(place_recog_mode_e energy_mode = PLACE_RECOG_HIGH_ACCURACY_MODE);
		~UserPlaces();

		void set_mode(place_recog_mode_e energy_mode);
		std::vector<std::shared_ptr<Place>> get_places();
		static json compose_json(std::vector<std::shared_ptr<Place>> places);

	};	/* class UserPlaces */

}	/* namespace ctx */

#endif /* __CONTEXT_PLACE_STATUS_USER_PLACES_ENGINE_H__ */

