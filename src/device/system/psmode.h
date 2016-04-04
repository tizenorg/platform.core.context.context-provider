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

#ifndef _DEVICE_STATUS_POWER_SAVING_MODE_H_
#define _DEVICE_STATUS_POWER_SAVING_MODE_H_

#include <vconf.h>
#include "../device_provider_base.h"

namespace ctx {

	class device_status_psmode : public device_provider_base {

		GENERATE_PROVIDER_COMMON_DECL(device_status_psmode);

	public:
		int subscribe();
		int unsubscribe();
		int read();
		static bool is_supported();
		static void submit_trigger_item();

	private:
		device_status_psmode();
		~device_status_psmode();
		void handle_update(keynode_t *node);
		static void update_cb(keynode_t *node, void* user_data);
	};
}

#endif // _DEVICE_STATUS_POWER_SAVING_H_
