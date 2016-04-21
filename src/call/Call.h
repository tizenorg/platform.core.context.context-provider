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

#ifndef _CONTEXT_SOCIAL_STATUS_CALL_H_
#define _CONTEXT_SOCIAL_STATUS_CALL_H_

#include <telephony.h>
#include "../shared/SocialTypes.h"
#include "../shared/BasicProvider.h"

namespace ctx {

	class SocialStatusCall : public BasicProvider {
	public:
		SocialStatusCall();
		~SocialStatusCall();

		int subscribe();
		int unsubscribe();
		int read();

		bool isSupported();
		void submitTriggerItem();

	private:
		telephony_handle_list_s __handleList;

		bool __initTelephony();
		void __releaseTelephony();
		bool __setCallback();
		void __unsetCallback();
		bool __readCurrentStatus(telephony_h& handle, Json* data);

		bool __getCallState(telephony_call_h& handle, std::string& state);
		bool __getCallType(telephony_call_h& handle, std::string& type);
		bool __getCallAddress(telephony_call_h& handle, std::string& address);
		bool __getCallHandleId(telephony_call_h& handle, unsigned int& id);

		void __handleUpdate(telephony_h handle, telephony_noti_e notiId, void* id);
		static void __updateCb(telephony_h handle, telephony_noti_e notiId, void *data, void *userData);
	};
}

#endif // _CONTEXT_SOCIAL_STATUS_CALL_H_
