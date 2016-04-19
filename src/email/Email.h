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

#ifndef _CONTEXT_SOCIAL_STATUS_EMAIL_H_
#define _CONTEXT_SOCIAL_STATUS_EMAIL_H_

#include <DBusSignalWatcher.h>
#include "../shared/SocialTypes.h"
#include "../shared/DeviceProviderBase.h"

namespace ctx {

	class SocialStatusEmail : public DeviceProviderBase, public IDBusSignalListener {
	public:
		SocialStatusEmail();
		~SocialStatusEmail();

		int subscribe();
		int unsubscribe();

		bool isSupported();
		void submitTriggerItem();

		void onSignal(const char *sender, const char *path, const char *iface, const char *name, GVariant *param);

	private:
		int64_t __dbusSignalId;
		DBusSignalWatcher __dbusWatcher;
	};
}

#endif // _CONTEXT_SOCIAL_STATUS_EMAIL_H_
