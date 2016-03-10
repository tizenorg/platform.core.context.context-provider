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

#include <types_internal.h>
#include "inactive_detector_weight.h"
#include "inactive_detector_storage.h"


int ctx::inactive_detector_weight::request_weights(
			double timestamp_from)
{
	inactive_detector_storage ids;
	// query the database for the result
	int error = ids.get_apps_info_w_weights(timestamp_from);

	return error;
}