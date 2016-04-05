/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd.
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

#ifndef _CONTEXT_SOCIAL_STATUS_CONTACTS_H_
#define _CONTEXT_SOCIAL_STATUS_CONTACTS_H_

#include <contacts.h>
#include "../DeviceProviderBase.h"

namespace ctx {

	class SocialStatusContacts : public DeviceProviderBase {

		GENERATE_PROVIDER_COMMON_DECL(SocialStatusContacts);

	public:
		int subscribe();
		int unsubscribe();
		static bool isSupported();
		static void submitTriggerItem();

	private:
		time_t __latestMyProfile;
		time_t __latestPerson;

		SocialStatusContacts();
		~SocialStatusContacts();

		bool __setCallback();
		void __unsetCallback();
		void __handleUpdate(const char* viewUri);
		static void __updateCb(const char* viewUri, void* userData);
		bool __isConsecutiveChange(const char* viewUri);
	};
}

#endif // _CONTEXT_SOCIAL_STATUS_CONTACTS_H_
