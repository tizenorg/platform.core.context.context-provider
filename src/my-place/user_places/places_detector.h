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

#ifndef _CONTEXT_PLACE_RECOGNITION_PLACES_DETECTOR_
#define _CONTEXT_PLACE_RECOGNITION_PLACES_DETECTOR_

#include <vector>
#include <cstdint>
#include <ITimerListener.h>
#include <DatabaseManager.h>
#include "visit_detector.h"
#include "user_places_types.h"
#include <MyPlaceTypes.h>

namespace ctx {

	class PlacesDetector : public ITimerListener {

	private:
		bool __testMode;
		DatabaseManager *__dbManager;

		double __doubleValueFromJson(Json &row, const char* key);
		Categs __visitCategsFromJson(Json &row);
		Visit __visitFromJson(Json &row);
		Visits __visitsFromJsons(std::vector<Json>& records);
		std::shared_ptr<ctx::Place> __placeFromJson(Json &row);
		void __placeCategoryFromJson(Json &row, ctx::Place &place);
		void __placeLocationFromJson(Json &row, ctx::Place &place);
		void __placeCreateDateFromJson(Json &row, ctx::Place &place);
		std::vector<std::shared_ptr<Place>> __placesFromJsons(std::vector<Json>& records);

		std::shared_ptr<graph::Graph> __graphFromVisits(const std::vector<Visit> &visits);

		void __dbCreateTable();
		void __dbDeletePlaces();
		void __dbDeleteOldVisits();
		void __dbDeleteOlderVisitsThan(time_t threshold);
		std::vector<Json> __dbGetVisits();
		std::vector<Json> __dbGetPlaces();
		void __dbInsertPlace(const Place &place);

		std::shared_ptr<Place> __placeFromMergedVisits(Visits &mergedVisits);
		std::vector<std::shared_ptr<Place>> __detectedPlaces;
		void __detectedPlacesUpdate(std::vector<std::shared_ptr<Place>> &newPlaces);
		void __processVisits(Visits &visits);
		static void __mergeLocation(const Visits &mergedVisits, Place &place);
		std::shared_ptr<graph::Components> __mergeVisits(const std::vector<Visit> &visits);

		bool onTimerExpired(int timerId);

	public:
		PlacesDetector(bool testMode = false);
		static void reduceOutliers(Visits &visits); // TODO: move to private
		static void reduceOutliers(std::shared_ptr<graph::Components> &cc); // TODO: move to private
		std::vector<std::shared_ptr<Place>> getPlaces();

	};  /* class PlacesDetector */

}	/* namespace ctx */

#endif /* End of _CONTEXT_PLACE_RECOGNITION_PLACES_DETECTOR_ */
