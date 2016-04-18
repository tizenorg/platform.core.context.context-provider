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

#ifndef _CONTEXT_DEVICE_PROVIDER_BASE_H_
#define _CONTEXT_DEVICE_PROVIDER_BASE_H_

#include <ContextProvider.h>

namespace ctx {

	class DeviceProviderBase : public ContextProvider {
	public:
		virtual bool isSupported();
		virtual void submitTriggerItem();

		int subscribe(Json option, Json *requestResult);
		int unsubscribe(Json option);
		int read(Json option, Json *requestResult);
		int write(Json data, Json *requestResult);

	protected:
		bool __beingSubscribed;

		DeviceProviderBase(const char *subject);
		virtual ~DeviceProviderBase() {}

		virtual int subscribe();
		virtual int unsubscribe();
		virtual int read();
		virtual int write();

		static bool getSystemInfoBool(const char *key);
	};
}

#endif	// _CONTEXT_DEVICE_PROVIDER_BASE_H_
