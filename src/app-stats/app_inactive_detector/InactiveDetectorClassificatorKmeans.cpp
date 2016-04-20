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
#include <Types.h>
#include "AppInactiveDetectorTypes.h"
#include "InactiveDetectorClassificatorKmeans.h"
#include "InactiveDetectorClassificatorKmeansTypes.h"
#include <math.h>
#include <stdlib.h>

double ctx::InactiveDetectorClassificatorKmeans::randomf(double x)
{
	return x * rand() / (RAND_MAX - 1.);
}

double ctx::InactiveDetectorClassificatorKmeans::__glideFunction(double x)
{
	double value = sqrt(log(x) + 1);
	return (value > 0 ? value : 0);
}

Point *ctx::InactiveDetectorClassificatorKmeans::__reprojectTo2d(std::vector<AppInfo> *appsWithWeights)
{
	int distrSize = appsWithWeights->size();
	Point *p = new Point[distrSize];

	int i = 0;
	for (auto appWithWeights = appsWithWeights->begin(); appWithWeights != appsWithWeights->end(); appWithWeights++) {
		Point thePoint;

		thePoint.x = __glideFunction(appWithWeights->weight); // normalize weight values
		thePoint.y = 0; //remove 3rd dimension of the data
		thePoint.originId = appWithWeights->packageName;

		p[i] = thePoint;
		i++;
	}

	return p;
}

bool ctx::InactiveDetectorClassificatorKmeans::__annotateData(std::vector<AppInfo> *appsWithWeights, Point *c)
{
	int i = 0, maxGroup = -1;

	for (auto appWithWeights = appsWithWeights->begin(); appWithWeights != appsWithWeights->end(); appWithWeights++) {
		appWithWeights->isActive = c[i].group;
		i++;
		maxGroup = c[i].group > maxGroup ? c[i].group : maxGroup;
	}

	return maxGroup == APP_INACTIVE_DETECTOR_KMEANS_CLUSTER_COUNT;
}

double ctx::InactiveDetectorClassificatorKmeans::__distanceTo(Point *pFrom, Point *pTo)
{
	double x = pFrom->x - pTo->x, y = pFrom->y - pTo->y;
	return x*x + y*y;
}

int ctx::InactiveDetectorClassificatorKmeans::__nearest(Point *pt, Point *centers, int clusterNumber, double *distance)
{
	int i;
	int minI = pt->group;
	Point *c;

	double d;
	double minD = HUGE_VAL;

	FOR_N {
		if (minD >(d = __distanceTo(c, pt))) {
			minD = d; minI = i;
		}
	}

	if (distance) *distance = minD;
	return minI;
}

void ctx::InactiveDetectorClassificatorKmeans::__kpp(Point *points, int length, Point *centers, int clusterNumber)
{
	int j;
	int nCluster;
	double sum;
	double *d = new double[length];

	Point *p;
	centers[0] = points[rand() % length];
	for (nCluster = 1; nCluster < clusterNumber; nCluster++) {
		sum = 0;
		FOR_LEN {
			__nearest(p, centers, nCluster, d + j);
			sum += d[j];
		}
		sum = randomf(sum);
		FOR_LEN {
			if ((sum -= d[j]) > 0) continue;
			centers[nCluster] = points[j];
			break;
		}
	}
	FOR_LEN {
		p->group = __nearest(p, centers, nCluster, 0);
	}

	delete d;
}

Point *ctx::InactiveDetectorClassificatorKmeans::__lloyd(Point *points, int length, int clusterNumber)
{
	int i, j, minI;
	int changed;

	Point *centers = new Point;
	Point *p, *c;

	/* k++ init */
	__kpp(points, length, centers, clusterNumber);

	do {
		/* group element for centroids are used as counters */
		FOR_N { c->group = 0; c->x = c->y = 0; }
		FOR_LEN {
			c = centers + p->group;
			c->group++;
			c->x += p->x; c->y += p->y;
		}
		FOR_N { c->x /= c->group; c->y /= c->group; }

		changed = 0;
		/* find closest centroid of each point */
		FOR_LEN {
			minI = __nearest(p, centers, clusterNumber, 0);
			if (minI != p->group) {
				changed++;
				p->group = minI;
			}
		}
	} while (changed > (length >> 10)); /* stop when 99.9% of points are good */

	FOR_N { c->group = i; }

	return centers;
}

int ctx::InactiveDetectorClassificatorKmeans::classify(std::vector<AppInfo> *appsWithWeights)
{
	//array of generated points
	Point *v = __reprojectTo2d(appsWithWeights);
	//mark cluster for each point, output centers
	Point *c = __lloyd(v, appsWithWeights->size(), APP_INACTIVE_DETECTOR_KMEANS_CLUSTER_COUNT);
	//append the result to input data
	bool classificationIsSuccess = __annotateData(appsWithWeights, c);

	return classificationIsSuccess ? ERR_NONE : ERR_OPERATION_FAILED;
}
