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

#ifndef __CONTEXT_APP_INACTIVE_DETECTOR_CLASSIFICATOR_KMEANS_TYPES__
#define __CONTEXT_APP_INACTIVE_DETECTOR_CLASSIFICATOR_KMEANS_TYPES__

#define APP_INACTIVE_DETECTOR_KMEANS_CLUSTER_COUNT 2

typedef struct {
	std::string origin_id;
	double x;
	double y;
	int group;
} point_s;

#endif