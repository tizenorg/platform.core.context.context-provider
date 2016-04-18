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

#ifndef _DEVICE_SYSTEM_STATUS_HEADPHONE_H_
#define _DEVICE_STATUS_HEADPNOHE_H_

#include <glib.h>
#include <runtime_info.h>
#include <bluetooth.h>
#include "../DeviceProviderBase.h"

namespace ctx {

	class DeviceStatusHeadphone : public DeviceProviderBase {
	public:
		DeviceStatusHeadphone();
		~DeviceStatusHeadphone();

		int subscribe();
		int unsubscribe();
		int read();

		bool isSupported();
		void submitTriggerItem();

	private:
		bool __connected;
		int __audioJackState;
		bool __btAudioState;
		bool __btAudioCallbackOn;
		bool __btEventHandlerAdded;
		int __btEventHandlingCount;

		bool __getCurrentStatus();
		void __setBtAudioCallback();
		void __unsetBtAudioCallback();
		void __setBtAudioState(bool state);

		void __generateDataPacket(Json* data);
		bool __handleUpdate();
		void __handleAudioJackEvent();
		static gboolean __handleBtEvent(gpointer data);

		static void __onAudioJackStateChanged(runtime_info_key_e runtimeKey, void* userData);
		static void __onBtConnectionChanged(bool connected, bt_device_connection_info_s *connInfo, void *userData);
		static bool __onBtBond(bt_device_info_s *deviceInfo, void* userData);
	};
}

#endif // _DEVICE_SYSTEM_STATUS_HEADPHONE_H_
