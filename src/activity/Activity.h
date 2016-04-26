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

#ifndef _CONTEXT_ACTIVITY_PROVIDER_H_
#define _CONTEXT_ACTIVITY_PROVIDER_H_

#include <string>
#include <activity_recognition.h>
#include <ProviderTypes.h>
#include <BasicProvider.h>

#define GENERATE_ACTIVITY_PROVIDER(actPrvd, actSubj, actType) \
	class actPrvd : public ActivityProvider { \
	public: \
		actPrvd() : ActivityProvider(actSubj, actType) {} \
	}; \

namespace ctx {

	class ActivityProvider : public BasicProvider {
	public:
		ActivityProvider(const char *subject, activity_type_e type);
		virtual ~ActivityProvider();

		int subscribe();
		int unsubscribe();

		bool isSupported();

	protected:
		activity_type_e __activityType;
		activity_h __activityHandle;

	private:
		void __handleUpdate(activity_type_e activity, const activity_data_h data, double timestamp);
		static void __updateCb(activity_type_e activity, const activity_data_h data, double timestamp, activity_error_e error, void* userData);
	};


	GENERATE_ACTIVITY_PROVIDER(StationaryActivityProvider, SUBJ_ACTIVITY_STATIONARY, ACTIVITY_STATIONARY);
	GENERATE_ACTIVITY_PROVIDER(WalkingActivityProvider, SUBJ_ACTIVITY_WALKING, ACTIVITY_WALK);
	GENERATE_ACTIVITY_PROVIDER(RunningActivityProvider, SUBJ_ACTIVITY_RUNNING, ACTIVITY_RUN);
	GENERATE_ACTIVITY_PROVIDER(InVehicleActivityProvider, SUBJ_ACTIVITY_IN_VEHICLE, ACTIVITY_IN_VEHICLE);
}

#endif // _CONTEXT_ACTIVITY_PROVIDER_H_
