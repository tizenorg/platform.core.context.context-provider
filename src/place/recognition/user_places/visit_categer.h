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

#ifndef _CONTEXT_PLACE_RECOGNITION_VISIT_CATEGER_H_
#define _CONTEXT_PLACE_RECOGNITION_VISIT_CATEGER_H_

#include "user_places_types.h"
#include "mahal.h"
#include "piecewise_lin.h"
#include <map>

namespace ctx {

	struct TimeFeatures {
	    int minutesSinceMidnight;
	    int minutesSinceBeginingOfTheWeek;
	    int weekday;
	    bool weekend;
	};

	/*
	 * visit categorizer class
	 */
	class VisitCateger {

	private:
		const static int MINUTES_IN_WEEK = 60 * 24 * 7;
		const static std::map<int, MahalModel> __models;
		const static std::vector<num_t> __featuresMean;
		const static std::vector<num_t> __featuresStd;
		static num_t __sum(const std::vector<num_t> model, const size_t &from, const size_t &to);
		static num_t __weekModelMeanValue(PlaceCategId categ, const Interval &interval,
				const TimeFeatures &start_f, const TimeFeatures &end_f);
		static void __normalize(std::vector<num_t> &features);
		static PiecewiseLin __chiApprox; // tabled chi function approximator

	public:
		/**
		 * Function interpret time in timestamp input argument,
		 *
		 * @param  time          timestamp
		 * @return TimeFeatures  structure with interpretations of timestamp
		 */
		static TimeFeatures timeFeatures(const time_t &time);

		static int weeksScope(const TimeFeatures &start_f, const Interval &interval);

		/**
		 * Function interpret time interval input argument and calculates scores
		 * that argument interval is home, work or other based on whole week model.
		 *
		 * @param  interval  time interval
		 * @param  start_f	 start time features
		 * @param  end_f	 end time features
		 * @return categs_t  score that argument interval is home, work or other
		 */
		static categs_t weekModelFeatures(const Interval &interval, const TimeFeatures &start_f,
				const TimeFeatures &end_f);

		/**
		 * Function interpret time interval input argument,
		 *
		 * @param  interval            time interval
		 * @return std::vector<num_t>  vector with interpretations of input time interval
		 */
		static std::vector<num_t> intervalFeatures(const Interval &interval);

		/**
		 * Function categorize visit based on visits time interval and fill its categories values.
		 */
		static void categorize(ctx::Visit &visit);

	};	/* class VisitCateger */

}	/* namespace ctx */

#endif /* End of _CONTEXT_PLACE_RECOGNITION_VISIT_CATEGER_H_ */
