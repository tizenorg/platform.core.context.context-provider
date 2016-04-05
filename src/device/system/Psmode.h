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

#ifndef _DEVICE_SYSTEM_STATUS_POWER_SAVING_MODE_H_
#define _DEVICE_SYSTEM_STATUS_POWER_SAVING_MODE_H_

#include <vconf.h>
#include "../DeviceProviderBase.h"

namespace ctx {

	class DeviceStatusPsmode : public DeviceProviderBase {

		GENERATE_PROVIDER_COMMON_DECL(DeviceStatusPsmode);

	public:
		int subscribe();
		int unsubscribe();
		int read();
		static bool isSupported();
		static void submitTriggerItem();

	private:
		DeviceStatusPsmode();
		~DeviceStatusPsmode();
		void __handleUpdate(keynode_t *node);
		static void __updateCb(keynode_t *node, void* userData);
	};
}

#endif // _DEVICE_SYSTEM_STATUS_POWER_SAVING_MODE_H_
