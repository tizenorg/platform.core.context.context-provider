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

#ifndef _DEVICE_STATUS_CHARGER_H_
#define _DEVICE_STATUS_CHARGER_H_

#include "base_rtinfo.h"

namespace ctx {

	class device_status_charger : public device_status_runtime_info {

		GENERATE_PROVIDER_COMMON_DECL(device_status_charger);

	public:
		int read();
		static bool is_supported();
		static void submit_trigger_item();

	private:
		device_status_charger();
		~device_status_charger();
		void handle_update();
	};
}

#endif // _DEVICE_STATUS_CHARGER_H_
