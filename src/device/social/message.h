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

#ifndef _CONTEXT_SOCIAL_STATUS_SMS_H_
#define _CONTEXT_SOCIAL_STATUS_SMS_H_

#include <msg.h>
#include <msg_transport.h>
#include "../device_provider_base.h"

namespace ctx {

	class social_status_message : public device_provider_base {

		GENERATE_PROVIDER_COMMON_DECL(social_status_message);

	public:
		int subscribe();
		int unsubscribe();
		static bool is_supported();
		static void submit_trigger_item();

	private:
		msg_handle_t message_handle;
		bool being_subscribed;

		social_status_message();
		~social_status_message();

		bool set_callback();
		void unset_callback();
		void handle_state_change(msg_struct_t msg);
		static void state_change_cb(msg_handle_t handle, msg_struct_t msg, void* user_data);
	};
}

#endif // _CONTEXT_SOCIAL_STATUS_SMS_H_
