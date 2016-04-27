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

#ifndef _CONTEXT_CONTACTS_CHANGE_PROVIDER_H_
#define _CONTEXT_CONTACTS_CHANGE_PROVIDER_H_

#include <contacts.h>
#include <ProviderTypes.h>
#include <BasicProvider.h>

namespace ctx {

	class ContactsChangeProvider : public BasicProvider {
	public:
		ContactsChangeProvider();
		~ContactsChangeProvider();

		int subscribe();
		int unsubscribe();

		bool isSupported();
		void getPrivilege(std::vector<const char*> &privilege);

	private:
		time_t __latestMyProfile;
		time_t __latestPerson;

		bool __setCallback();
		void __unsetCallback();
		void __handleUpdate(const char* viewUri);
		static void __updateCb(const char* viewUri, void* userData);
		bool __isConsecutiveChange(const char* viewUri);
	};
}

#endif /* _CONTEXT_CONTACTS_CHANGE_PROVIDER_H_ */
