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

#ifndef _DEVICE_ACTIVITY_STATUS_BASE_H_
#define _DEVICE_ACTIVITY_STATUS_BASE_H_

#include <string>
#include <activity_recognition.h>
#include "../shared/BasicProvider.h"

namespace ctx {

	class UserActivityBase : public BasicProvider {
	public:
		UserActivityBase(const char *subject, activity_type_e type);
		virtual ~UserActivityBase();

		int subscribe();
		int unsubscribe();

		bool isSupported();
		void submitTriggerItem();

	protected:
		activity_type_e __activityType;
		activity_h __activityHandle;

	private:
		void __handleUpdate(activity_type_e activity, const activity_data_h data, double timestamp);
		static void __updateCb(activity_type_e activity, const activity_data_h data, double timestamp, activity_error_e error, void* userData);
	};

}

#endif // _DEVICE_ACTIVITY_STATUS_BASE_H_
