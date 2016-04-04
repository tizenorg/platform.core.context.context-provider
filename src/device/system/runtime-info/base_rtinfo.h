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

#ifndef __CONTEXT_DEVICE_STATUS_RUNTIME_INFO_H__
#define __CONTEXT_DEVICE_STATUS_RUNTIME_INFO_H__

#include <runtime_info.h>
#include "../../device_provider_base.h"

namespace ctx {

	class device_status_runtime_info : public device_provider_base {
	public:
		device_status_runtime_info(runtime_info_key_e key);

		int subscribe();
		int unsubscribe();
		virtual int read() = 0;

	protected:
		runtime_info_key_e info_key;

		virtual ~device_status_runtime_info(){}
		static void update_cb(runtime_info_key_e runtime_key, void* user_data);
		virtual void handle_update() = 0;

	private:
		runtime_info_key_e get_info_key();
	};
}

#endif
