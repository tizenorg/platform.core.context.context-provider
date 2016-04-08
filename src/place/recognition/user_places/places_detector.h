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

#ifndef _CONTEXT_PLACE_RECOGNITION_PLACES_DETECTOR_
#define _CONTEXT_PLACE_RECOGNITION_PLACES_DETECTOR_

#include "visit_detector.h"
#include <ITimerListener.h>
#include <cstdint>
#include "db_listener_iface.h"
#include "user_places_types.h"
#include <vector>

namespace ctx {

	class PlacesDetector : public ITimerListener {

	private:
		bool __testMode;

		double __doubleValueFromJson(Json &row, const char* key);
		categs_t __visitCategsFromJson(Json &row);
		Visit __visitFromJson(Json &row);
		visits_t __visitsFromJsons(std::vector<Json>& records);
		std::shared_ptr<ctx::Place> __placeFromJson(Json &row);
		std::vector<std::shared_ptr<Place>> __placesFromJsons(std::vector<Json>& records);

		std::shared_ptr<graph::Graph> __graphFromVisits(const std::vector<Visit> &visits);

		void __dbCreateTable();
		void __dbDeletePlaces();
		void __dbDeleteOldVisits();
		void __dbDeleteOlderVisitsThan(time_t threshold);
		std::vector<Json> __dbGetVisits();
		std::vector<Json> __dbGetPlaces();
		void __dbInsertPlace(const Place &place);

		std::shared_ptr<Place> __placeFromMergedVisits(visits_t &merged_visits);
		std::vector<std::shared_ptr<Place>> __detectedPlaces;
		void __detectedPlacesUpdate(std::vector<std::shared_ptr<Place>> &new_places);
		void __processVisits(visits_t &visits);
		static void __mergeLocation(const visits_t &merged_visits, Place &place);
		std::shared_ptr<graph::Components> __mergeVisits(const std::vector<Visit> &visits);

		bool onTimerExpired(int timerId);

	public:
		PlacesDetector(bool testMode = false);
		static void reduceOutliers(visits_t &visits); // TODO: move to private
		static void reduceOutliers(std::shared_ptr<graph::Components> &cc); // TODO: move to private
		std::vector<std::shared_ptr<Place>> getPlaces();

	};  /* class PlacesDetector */

}	/* namespace ctx */

#endif /* End of _CONTEXT_PLACE_RECOGNITION_PLACES_DETECTOR_ */
