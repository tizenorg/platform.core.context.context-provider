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

#ifndef _DEVICE_STATUS_ACTIVITY_H_
#define _DEVICE_STATUS_ACTIVITY_H_

#include <system_info.h>
#include "activity_base.h"

#define GENERATE_ACTIVITY_PROVIDER(act_prvd, act_subj, act_type) \
	class act_prvd : public user_activity_base { \
	public: \
		static ContextProviderBase *create(void *data) \
		{ \
			CREATE_INSTANCE(ctx::act_prvd); \
		} \
		static void destroy(void *data) \
		{ \
			DESTROY_INSTANCE(); \
		} \
		static bool is_supported() \
		{ \
			return get_system_info_bool("tizen.org/feature/sensor.activity_recognition"); \
		} \
		static void submit_trigger_item() \
		{ \
			context_manager::registerTriggerItem((act_subj), OPS_SUBSCRIBE, \
					"{\"Event\":{\"type\":\"string\", \"values\":[\"Detected\"]}}", \
					"{\"Accuracy\":{\"type\":\"string\", \"values\":[\"Low\", \"Normal\", \"High\"]}}" \
					); \
		} \
	protected: \
		void destroy_self() \
		{ \
			destroy(NULL); \
		} \
	private: \
		static act_prvd *__instance; \
		act_prvd() : user_activity_base((act_subj), (act_type)) {} \
	}; \
	ctx::act_prvd *ctx::act_prvd::__instance = NULL; \

namespace ctx {
	GENERATE_ACTIVITY_PROVIDER(user_activity_stationary, USER_ACT_SUBJ_STATIONARY, ACTIVITY_STATIONARY);
	GENERATE_ACTIVITY_PROVIDER(user_activity_walking, USER_ACT_SUBJ_WALKING, ACTIVITY_WALK);
	GENERATE_ACTIVITY_PROVIDER(user_activity_running, USER_ACT_SUBJ_RUNNING, ACTIVITY_RUN);
	GENERATE_ACTIVITY_PROVIDER(user_activity_in_vehicle, USER_ACT_SUBJ_IN_VEHICLE, ACTIVITY_IN_VEHICLE);
}

#endif // _DEVICE_STATUS_ACTIVITY_H_
