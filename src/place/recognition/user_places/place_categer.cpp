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

#include "place_categer.h"
#include "median.h"
#include "../place_recognition_types.h"
#include "user_places_params.h"
#include "user_places_types.h"
#include <algorithm>
#include <types_internal.h>

void ctx::PlaceCateger::reduceOutliers(ctx::visits_t &visits)
{
	int size = visits.size();
	visits.erase(std::remove_if(
					visits.begin(),
					visits.end(),
					[](Visit v) {
						return v.categs[PLACE_CATEG_ID_HOME] < PLACES_CATEGER_MIN_VISITS_SCORE
								&& v.categs[PLACE_CATEG_ID_WORK] < PLACES_CATEGER_MIN_VISITS_SCORE
								&& v.categs[PLACE_CATEG_ID_OTHER] < PLACES_CATEGER_MIN_VISITS_SCORE;
					}),
				visits.end());
	int new_size = visits.size();
	if (size != new_size) {
		_D("Visits number from %d to %d (visits min scores checking)", size, new_size);
	}
}

/*
 * Change category if home or work has to few visits
 */
bool ctx::PlaceCateger::__reduceCategory(const PlaceCategId &categ, const ctx::visits_t &visits)
{
	return (categ == PLACE_CATEG_ID_HOME && visits.size() < PLACES_CATEGER_MIN_VISITS_PER_HOME)
		|| (categ == PLACE_CATEG_ID_WORK && visits.size() < PLACES_CATEGER_MIN_VISITS_PER_WORK);
}

void ctx::PlaceCateger::categorize(ctx::visits_t &visits, ctx::Place &place)
{
	reduceOutliers(visits);

	place.categ_id = PLACE_CATEG_ID_NONE;
	place.categ_confidence = 0.0;

	if (!visits.empty()) {
		const std::vector<PlaceCategId> categ_ids = {
			PLACE_CATEG_ID_HOME,
			PLACE_CATEG_ID_WORK,
			PLACE_CATEG_ID_OTHER
		};
		num_t sum_score = 0.0;
		num_t max_score = 0.0;
		for (PlaceCategId categ_id : categ_ids) {
			std::vector<num_t> categ_vector = categVectorFromVisits(visits, categ_id);
			num_t score = median(categ_vector);
			sum_score += score;
			if (score > max_score) {
				max_score = score;
				place.categ_id = categ_id;
			}
		}
		if (sum_score > 0) {
			place.categ_confidence = max_score / sum_score;
		}
		if (__reduceCategory(place.categ_id, visits)) {
			place.categ_id = PLACE_CATEG_ID_OTHER;
			place.categ_confidence = 0.0;
		}
	}

	place.name = categId2Name(place.categ_id);
}

std::vector<ctx::num_t> ctx::PlaceCateger::categVectorFromVisits(const ctx::visits_t &visits, PlaceCategId categ_id)
{
	std::vector<ctx::num_t> vec;
	for (auto &visit : visits) {
		auto search = visit.categs.find(categ_id);
		if (search != visit.categs.end()) {
			vec.push_back(search->second);
		}
	}
	return vec;
}

std::string ctx::PlaceCateger::categId2Name(PlaceCategId categ_id) {
	switch (categ_id) {
	case PLACE_CATEG_ID_HOME:  return "home";
	case PLACE_CATEG_ID_WORK:  return "work";
	case PLACE_CATEG_ID_OTHER: return "other";
	case PLACE_CATEG_ID_NONE:  return "none";
	default: return "";
	}
}
