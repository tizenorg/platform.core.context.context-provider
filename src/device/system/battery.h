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

#ifndef _DEVICE_STATUS_BATTERY_LEVEL_H_
#define _DEVICE_STATUS_BATTERY_LEVEL_H_

#include <device/callback.h>
#include <device/battery.h>
#include "../provider_base.h"

namespace ctx {

	class device_status_battery : public device_provider_base {

		GENERATE_PROVIDER_COMMON_DECL(device_status_battery);

	public:
		int subscribe();
		int unsubscribe();
		int read();
		static bool is_supported();
		static void submit_trigger_item();

	private:
		device_status_battery();
		~device_status_battery();
		const char* trans_to_string(intptr_t level);
		void handle_update(device_callback_e device_type, void* value);
		static void update_cb(device_callback_e device_type, void* value, void* user_data);
	};
}

#endif // _DEVICE_STATUS_BATTERY_LEVEL_H_
