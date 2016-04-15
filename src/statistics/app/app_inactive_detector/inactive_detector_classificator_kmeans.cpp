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
#include <Types.h>
#include "app_inactive_detector_types.h"
#include "inactive_detector_classificator_kmeans.h"
#include "inactive_detector_classificator_kmeans_types.h"
#include <math.h>
#include <stdlib.h>

double ctx::inactive_detector_classificator_kmeans::randomf(double x)
{
	return x * rand() / (RAND_MAX - 1.);
}

double ctx::inactive_detector_classificator_kmeans::glide_function(double x)
{
	double value = sqrt(log(x) + 1);
	return (value > 0 ? value : 0);
}

point_s *ctx::inactive_detector_classificator_kmeans::reproject_to_2d(std::vector<app_t> *apps_with_weights)
{
	int distr_size = apps_with_weights->size();
	point_s *p = new point_s[distr_size];

	int i=0;
	for(std::vector<app_t>::iterator apps_with_weight = apps_with_weights->begin();
		apps_with_weight != apps_with_weights->end(); apps_with_weight++)
	{
		point_s the_point;

		the_point.x = glide_function(apps_with_weight->weight); // normalize weight values
		the_point.y = 0; //remove 3rd dimension of the data
		the_point.origin_id = apps_with_weight->package_name;

		p[i] = the_point;
		i++;
	}

	return p;
}

bool ctx::inactive_detector_classificator_kmeans::annotate_data(std::vector<app_t> *apps_with_weights, point_s *c)
{
	int i=0, max_group=-1;

	for(std::vector<app_t>::iterator apps_with_weight = apps_with_weights->begin();
		apps_with_weight != apps_with_weights->end(); apps_with_weight++)
	{
		apps_with_weight->is_active = c[i].group;
		i++;
		max_group = c[i].group > max_group ? c[i].group : max_group;
	}

	return max_group == APP_INACTIVE_DETECTOR_KMEANS_CLUSTER_COUNT;
}

double ctx::inactive_detector_classificator_kmeans::distance_to(point_s *p_from,
	point_s *p_to)
{
	double x = p_from->x - p_to->x, y = p_from->y - p_to->y;
	return x*x + y*y;
}

int ctx::inactive_detector_classificator_kmeans::nearest(point_s *pt,
	point_s *centers,
	int cluster_number,
	double *distance)
{
	int i;
	int min_i = pt->group;
	point_s *c;

	double d;
	double min_d = HUGE_VAL;

	for_n {
		if (min_d >(d = distance_to(c, pt))) {
			min_d = d; min_i = i;
		}
	}

	if (distance) *distance = min_d;
	return min_i;
}

void ctx::inactive_detector_classificator_kmeans::kpp(point_s *points, int length, point_s *centers, int cluster_number)
{
	int j;
	int n_cluster;
	double sum;
	double *d = new double[length];

	point_s *p;
	centers[0] = points[rand() % length];
	for (n_cluster = 1; n_cluster < cluster_number; n_cluster++) {
		sum = 0;
		for_len{
			nearest(p, centers, n_cluster, d + j);
			sum += d[j];
		}
		sum = randomf(sum);
		for_len{
			if ((sum -= d[j]) > 0) continue;
			centers[n_cluster] = points[j];
			break;
		}
	}
	for_len p->group = nearest(p, centers, n_cluster, 0);

	delete d;
}

point_s *ctx::inactive_detector_classificator_kmeans::lloyd(point_s *points, int length, int cluster_number)
{
	int i, j, min_i;
	int changed;

	point_s *centers = new point_s;
	point_s *p, *c;

	/* k++ init */
	kpp(points, length, centers, cluster_number);

	do {
		/* group element for centroids are used as counters */
		for_n{ c->group = 0; c->x = c->y = 0; }
			for_len{
				c = centers + p->group;
				c->group++;
				c->x += p->x; c->y += p->y;
			}
		for_n{ c->x /= c->group; c->y /= c->group; }

		changed = 0;
		/* find closest centroid of each point */
		for_len{
			min_i = nearest(p, centers, cluster_number, 0);
			if (min_i != p->group) {
				changed++;
				p->group = min_i;
			}
		}
	} while (changed > (length >> 10)); /* stop when 99.9% of points are good */

	for_n{ c->group = i; }

	return centers;
}

int ctx::inactive_detector_classificator_kmeans::classify(std::vector<app_t> *apps_with_weights)
{
	//array of generated points
	point_s *v = reproject_to_2d(apps_with_weights);
	//mark cluster for each point, output centers
	point_s *c = lloyd(v, apps_with_weights->size(), APP_INACTIVE_DETECTOR_KMEANS_CLUSTER_COUNT);
	//append the result to input data
	bool classification_is_success = annotate_data(apps_with_weights, c);

	return classification_is_success ? ERR_NONE : ERR_OPERATION_FAILED;
}
