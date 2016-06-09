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

#ifndef _CONTEXT_PLACE_RECOGNITION_USER_PLACES_H_
#define _CONTEXT_PLACE_RECOGNITION_USER_PLACES_H_

#include <vector>
#include <Json.h>
#include <TimerManager.h>
#include "../visit-detector/VisitDetector.h"
#include "../place/PlacesDetector.h"
#include "UserPlacesTypes.h"

namespace ctx {

	class UserPlaces {

	private:
		VisitDetector *__visitDetector;
		PlacesDetector *__placesDetector;
		int __placesDetectorTimerId;
		TimerManager __timerManager;
		std::vector<std::shared_ptr<Place>> __getPlaces();
		static Json __composeJson(std::vector<std::shared_ptr<Place>> places);

	public:
		UserPlaces(PlaceRecogMode energyMode = PLACE_RECOG_HIGH_ACCURACY_MODE);
		~UserPlaces();

		void setMode(PlaceRecogMode energyMode);
		ctx::Json getPlaces();

	};	/* class UserPlaces */

}	/* namespace ctx */

#endif /* End of _CONTEXT_PLACE_RECOGNITION_USER_PLACES_H_ */
