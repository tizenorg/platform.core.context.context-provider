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

#ifndef _CONTEXT_MESSAGE_EVENT_PROVIDER_H_
#define _CONTEXT_MESSAGE_EVENT_PROVIDER_H_

#include <msg.h>
#include <msg_transport.h>
#include <ProviderTypes.h>
#include <BasicProvider.h>

namespace ctx {

	class MessageEventProvider : public BasicProvider {
	public:
		MessageEventProvider();
		~MessageEventProvider();

		int subscribe();
		int unsubscribe();
		bool isSupported();
		void getPrivilege(std::vector<const char*> &privilege);

		bool unloadable();

	private:
		msg_handle_t __messageHandle;
		bool __beingSubscribed;

		bool __setCallback();
		void __unsetCallback();
		void __handleUpdate(msg_struct_t msg);
		static void __updateCb(msg_handle_t handle, msg_struct_t msg, void* userData);
	};
}

#endif /* _CONTEXT_MESSAGE_EVENT_PROVIDER_H_ */
