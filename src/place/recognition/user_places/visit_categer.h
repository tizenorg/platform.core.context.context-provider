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

#ifndef __CONTEXT_PLACE_STATUS_VISIT_CATEGER_H__
#define __CONTEXT_PLACE_STATUS_VISIT_CATEGER_H__

#include "user_places_types.h"
#include "mahal.h"
#include "piecewise_lin.h"
#include <map>

namespace ctx {

	struct time_features_s {
	    int minutes_since_midnight;
	    int minutes_since_begining_of_the_week;
	    int weekday;
	    bool weekend;
	};

	/*
	 * visit categorizer class
	 */
	class VisitCateger {

	private:
		const static int MINUTES_IN_WEEK = 60 * 24 * 7;
		const static std::map<int, MahalModel> models;
		const static std::vector<num_t> features_mean;
		const static std::vector<num_t> features_std;
		static num_t sum(const std::vector<num_t> model, const size_t &from, const size_t &to);
		static num_t week_model_mean_value(place_categ_id_e categ, const interval_s &interval,
				const time_features_s &start_f,	const time_features_s &end_f);
		static void normalize(std::vector<num_t> &features);

	public:
		static PiecewiseLin chi_approx; // tabled chi function approximator

		/**
		 * Function interpret time in timestamp input argument,
		 *
		 * @param  time             timestamp
		 * @return time_features_s  structure with interpretations of timestamp
		 */
		static time_features_s time_features(const time_t &time);

		static int weeks_scope(const time_features_s &start_f, const interval_s &interval);

		/**
		 * Function interpret time interval input argument and calculates scores
		 * that argument interval is home, work or other based on whole week model.
		 *
		 * @param  interval  time interval
		 * @param  start_f	 start time features
		 * @param  end_f	 end time features
		 * @return categs_t  score that argument interval is home, work or other
		 */
		static categs_t week_model_features(const interval_s &interval,	const time_features_s &start_f,
				const time_features_s &end_f);

		/**
		 * Function interpret time interval input argument,
		 *
		 * @param  interval            time interval
		 * @return std::vector<num_t>  vector with interpretations of input time interval
		 */
		static std::vector<num_t> interval_features(const interval_s &interval);

		/**
		 * Function categorize visit based on visits time interval and fill its categories values.
		 */
		static void categorize(ctx::visit_s &visit);

	};	/* class VisitCateger */

}	/* namespace ctx */

#endif /* __CONTEXT_PLACE_STATUS_VISIT_CATEGER_H__ */
