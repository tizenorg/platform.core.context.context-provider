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
#include "inactive_detector.h"
#include "app_inactive_detector_types.h"
#include "inactive_detector_classificator.h"
#include "inactive_detector_classificator_kmeans.h"

int ctx::inactive_detector_classificator::classify(std::vector<app_t> *apps_with_weights)
{
	inactive_detector_classificator_kmeans kmeans;
	int error = kmeans.classify(apps_with_weights);

	return error;
}