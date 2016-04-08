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

#ifndef _DEVICE_ACTIVITY_STATUS_H_
#define _DEVICE_ACTIVITY_STATUS_H_

#include <system_info.h>
#include "ActivityBase.h"

#define GENERATE_ACTIVITY_PROVIDER(actPrvd, actSubj, actType) \
	class actPrvd : public UserActivityBase { \
	public: \
		static ContextProviderBase *create(void *data) \
		{ \
			CREATE_INSTANCE(ctx::actPrvd); \
		} \
		static void destroy(void *data) \
		{ \
			DESTROY_INSTANCE(); \
		} \
		static bool isSupported() \
		{ \
			return getSystemInfoBool("tizen.org/feature/sensor.activity_recognition"); \
		} \
		static void submitTriggerItem() \
		{ \
			context_manager::registerTriggerItem((actSubj), OPS_SUBSCRIBE, \
					"{\"Event\":{\"type\":\"string\", \"values\":[\"Detected\"]}}", \
					"{\"Accuracy\":{\"type\":\"string\", \"values\":[\"Low\", \"Normal\", \"High\"]}}" \
					); \
		} \
	protected: \
		void destroySelf() \
		{ \
			destroy(NULL); \
		} \
	private: \
		static actPrvd *__instance; \
		actPrvd() : UserActivityBase((actSubj), (actType)) {} \
	}; \
	ctx::actPrvd *ctx::actPrvd::__instance = NULL; \

namespace ctx {
	GENERATE_ACTIVITY_PROVIDER(UserActivityStationary, USER_ACT_SUBJ_STATIONARY, ACTIVITY_STATIONARY);
	GENERATE_ACTIVITY_PROVIDER(UserActivityWalking, USER_ACT_SUBJ_WALKING, ACTIVITY_WALK);
	GENERATE_ACTIVITY_PROVIDER(UserActivityRunning, USER_ACT_SUBJ_RUNNING, ACTIVITY_RUN);
	GENERATE_ACTIVITY_PROVIDER(UserActivityInVehicle, USER_ACT_SUBJ_IN_VEHICLE, ACTIVITY_IN_VEHICLE);
}

#endif // _DEVICE_ACTIVITY_STATUS_H_
