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

#ifndef _CONTEXT_PLACE_RECOGNITION_VISIT_CATEGER_H_
#define _CONTEXT_PLACE_RECOGNITION_VISIT_CATEGER_H_

#include "../facade/UserPlacesTypes.h"
#include "Mahal.h"
#include "PiecewiseLin.h"
#include <map>
#include <Types.h>

namespace ctx {

	struct TimeFeatures {
	    int minutesSinceMidnight;
	    int minutesSinceBeginingOfTheWeek;
	    int weekday;
	    bool weekend;
	};

	typedef std::vector<num_t> IntervalFeatures;

	/*
	 * visit categorizer class
	 */
	class VisitCateger {

	private:
		static const int __MINUTES_IN_WEEK = 60 * 24 * 7;
		static const std::map<int, MahalModel> __models;
		static const std::vector<num_t> __featuresMean;
		static const std::vector<num_t> __featuresStd;
		static num_t __sum(const std::vector<num_t> model, const size_t &from, const size_t &to);
		static num_t __weekModelMeanValue(PlaceCategId categ, const Interval &interval,
				const TimeFeatures &startF, const TimeFeatures &endF);
		static void __normalize(std::vector<num_t> &features);
		static PiecewiseLin __chiApprox; // tabled chi function approximator

		/**
		 * Function interpret time in timestamp input argument,
		 *
		 * @param  time          timestamp
		 * @return TimeFeatures  structure with interpretations of timestamp
		 */
		static TimeFeatures __timeFeatures(const time_t &time);

		static int __weeksScope(const TimeFeatures &startF, const Interval &interval);

		/**
		 * Function interpret time interval input argument and calculates scores
		 * that argument interval is home, work or other based on whole week model.
		 *
		 * @param  interval  time interval
		 * @param  startF	 start time features
		 * @param  endF	     end time features
		 * @return Categs    score that argument interval is home, work or other
		 */
		static Categs __weekModelFeatures(const Interval &interval, const TimeFeatures &startF,
				const TimeFeatures &endF);

		/**
		 * Function interpret time interval input argument,
		 *
		 * @param  interval          time interval
		 * @return IntervalFeatures  vector with interpretations of input time interval
		 */
		static IntervalFeatures __intervalFeatures(const Interval &interval);

	public:

		/**
		 * Function categorize visit based on visits time interval and fill its categories values.
		 */
		static void categorize(ctx::Visit &visit);

	};	/* class VisitCateger */

}	/* namespace ctx */

#endif /* End of _CONTEXT_PLACE_RECOGNITION_VISIT_CATEGER_H_ */
