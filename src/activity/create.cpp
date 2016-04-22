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

#include <new>
#include <create.h>
#include "Activity.h"

using namespace ctx;

/* TODO: This function will be changed into the following form:
   ContextProvider* create(const char *subject) */

extern "C" SO_EXPORT bool create()
{
	registerProvider<StationaryActivityProvider>(SUBJ_ACTIVITY_STATIONARY, NULL);
	registerProvider<WalkingActivityProvider>(SUBJ_ACTIVITY_WALKING, NULL);
	registerProvider<RunningActivityProvider>(SUBJ_ACTIVITY_RUNNING, NULL);
	registerProvider<InVehicleActivityProvider>(SUBJ_ACTIVITY_IN_VEHICLE, NULL);

	return true;
}
