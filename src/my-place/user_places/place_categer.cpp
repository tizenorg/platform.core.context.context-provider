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

#include "place_categer.h"
#include "median.h"
#include "../place_recognition_types.h"
#include "user_places_params.h"
#include "user_places_types.h"
#include <algorithm>
#include <Types.h>

void ctx::PlaceCateger::__reduceOutliers(ctx::Visits &visits)
{
	int size = visits.size();
	visits.erase(std::remove_if(
					visits.begin(),
					visits.end(),
					[](Visit v)->bool {
						return v.categs[PLACE_CATEG_ID_HOME] < PLACES_CATEGER_MIN_VISITS_SCORE
								&& v.categs[PLACE_CATEG_ID_WORK] < PLACES_CATEGER_MIN_VISITS_SCORE
								&& v.categs[PLACE_CATEG_ID_OTHER] < PLACES_CATEGER_MIN_VISITS_SCORE;
					}),
				visits.end());
	int newSize = visits.size();
	if (size != newSize)
		_D("Visits number from %d to %d (visits min scores checking)", size, newSize);
}

/*
 * Change category if home or work has to few visits
 */
bool ctx::PlaceCateger::__reduceCategory(const PlaceCategId &categId, const ctx::Visits &visits)
{
	return (categId == PLACE_CATEG_ID_HOME && visits.size() < PLACES_CATEGER_MIN_VISITS_PER_HOME)
		|| (categId == PLACE_CATEG_ID_WORK && visits.size() < PLACES_CATEGER_MIN_VISITS_PER_WORK);
}

void ctx::PlaceCateger::categorize(ctx::Visits &visits, ctx::Place &place)
{
	__reduceOutliers(visits);

	place.categId = PLACE_CATEG_ID_NONE;
	place.categConfidence = 0.0;

	if (!visits.empty()) {
		const std::vector<PlaceCategId> categIds = {
			PLACE_CATEG_ID_HOME,
			PLACE_CATEG_ID_WORK,
			PLACE_CATEG_ID_OTHER
		};
		num_t sumScore = 0.0;
		num_t maxScore = 0.0;
		for (PlaceCategId categId : categIds) {
			std::vector<num_t> categVector = __categVectorFromVisits(visits, categId);
			int i, j;
			num_t score = median(categVector, i, j);
			sumScore += score;
			if (score > maxScore) {
				maxScore = score;
				place.categId = categId;
			}
		}
		if (sumScore > 0)
			place.categConfidence = maxScore / sumScore;
		if (__reduceCategory(place.categId, visits)) {
			place.categId = PLACE_CATEG_ID_OTHER;
			place.categConfidence = 0.0;
		}
	}

	place.name = __categId2Name(place.categId);
}

std::vector<ctx::num_t> ctx::PlaceCateger::__categVectorFromVisits(const ctx::Visits &visits, PlaceCategId categId)
{
	std::vector<ctx::num_t> vec;
	for (auto &visit : visits) {
		auto search = visit.categs.find(categId);
		if (search != visit.categs.end())
			vec.push_back(search->second);
	}
	return vec;
}

std::string ctx::PlaceCateger::__categId2Name(PlaceCategId categId) {
	switch (categId) {
	case PLACE_CATEG_ID_HOME:
		return "home";
	case PLACE_CATEG_ID_WORK:
		return "work";
	case PLACE_CATEG_ID_OTHER:
		return "other";
	case PLACE_CATEG_ID_NONE:
		return "none";
	default:
		return "";
	}
}
