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

#ifndef _DEVICE_STATUS_ACTIVITY_BASE_H_
#define _DEVICE_STATUS_ACTIVITY_BASE_H_

#include <string>
#include <activity_recognition.h>
#include "../device_provider_base.h"

namespace ctx {

	class user_activity_base : public device_provider_base {
	public:
		int subscribe();
		int unsubscribe();

	protected:
		activity_type_e activity_type;
		activity_h activity_handle;
		std::string subject;

		user_activity_base(const char *subj, activity_type_e type);
		virtual ~user_activity_base();

	private:
		void handle_event(activity_type_e activity, const activity_data_h data, double timestamp);
		static void event_cb(activity_type_e activity, const activity_data_h data, double timestamp, activity_error_e error, void* user_data);
	};

}

#endif // _DEVICE_STATUS_ACTIVITY_BASE_H_
