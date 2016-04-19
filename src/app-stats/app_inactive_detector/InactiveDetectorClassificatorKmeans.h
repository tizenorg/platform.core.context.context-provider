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

#ifndef __CONTEXT_INACTIVE_DETECTOR_CLASSIFICATOR_KMEANS_H__
#define __CONTEXT_INACTIVE_DETECTOR_CLASSIFICATOR_KMEANS_H__

#include <vector>
#include "InactiveDetectorClassificatorKmeansTypes.h"
#include <Json.h>

namespace ctx {

	#define for_n for (c = centers, i = 0; i < cluster_number; i++, c++)
	#define for_len for (j = 0, p = points; j < length; j++, p++)

	class inactive_detector_classificator_kmeans
	{
		private:
			double randomf(double x);
			double glide_function(double x);
			point_s *reproject_to_2d(std::vector<app_t> *apps_with_weights);
			bool annotate_data(std::vector<app_t> *apps_with_weights, point_s *c);

			double distance_to(point_s *p_from, point_s *p_to);
			int nearest(point_s *pt, point_s *centers, int cluster_number, double *distance);
			void kpp(point_s *points, int length, point_s *centers, int centers_count);
			point_s *lloyd(point_s *points, int length, int cluster_number);
		public:
			inactive_detector_classificator_kmeans();
			~inactive_detector_classificator_kmeans();

			int classify(std::vector<app_t> *apps_with_weights);
	};	/* class inactive_detector_classificator_kmeans */
}	/* namespace ctx */

#endif /* __CONTEXT_INACTIVE_DETECTOR_CLASSIFICATOR_KMEANS_H__ */
