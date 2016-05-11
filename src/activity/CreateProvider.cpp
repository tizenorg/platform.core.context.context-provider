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

#include <CreateProvider.h>
#include "Activity.h"

using namespace ctx;

extern "C" SO_EXPORT ContextProvider* CreateProvider(const char *subject)
{
	ADD_PROVIDER(SUBJ_ACTIVITY_STATIONARY, StationaryActivityProvider);
	ADD_PROVIDER(SUBJ_ACTIVITY_WALKING, WalkingActivityProvider);
	ADD_PROVIDER(SUBJ_ACTIVITY_RUNNING, RunningActivityProvider);
	ADD_PROVIDER(SUBJ_ACTIVITY_IN_VEHICLE, InVehicleActivityProvider);

	return NULL;
}
