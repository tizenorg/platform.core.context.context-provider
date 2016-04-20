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

#ifndef _CONTEXT_INACTIVE_DETECTOR_CLASSIFICATOR_KMEANS_H_
#define _CONTEXT_INACTIVE_DETECTOR_CLASSIFICATOR_KMEANS_H_

#include <vector>
#include "InactiveDetectorClassificatorKmeansTypes.h"
#include <Json.h>

namespace ctx {

	#define FOR_N for (c = centers, i = 0; i < clusterNumber; i++, c++)
	#define FOR_LEN for (j = 0, p = points; j < length; j++, p++)

	class InactiveDetectorClassificatorKmeans
	{
	private:
		double randomf(double x);
		double __glideFunction(double x);
		Point *__reprojectTo2d(std::vector<AppInfo> *appsWithWeights);
		bool __annotateData(std::vector<AppInfo> *appsWithWeights, Point *c);

		double __distanceTo(Point *pFrom, Point *pTo);
		int __nearest(Point *pt, Point *centers, int clusterNumber, double *distance);
		void __kpp(Point *points, int length, Point *centers, int centersCount);
		Point *__lloyd(Point *points, int length, int clusterNumber);

	public:
		InactiveDetectorClassificatorKmeans();
		~InactiveDetectorClassificatorKmeans();

		int classify(std::vector<AppInfo> *appsWithWeights);
	};	/* class InactiveDetectorClassificatorKmeans */
}	/* namespace ctx */

#endif /* _CONTEXT_INACTIVE_DETECTOR_CLASSIFICATOR_KMEANS_H_ */
