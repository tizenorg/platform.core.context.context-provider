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
#include "../provider_base.h"

namespace ctx {

	class social_status_call : public device_provider_base {

		GENERATE_PROVIDER_COMMON_DECL(social_status_call);

	public:
		int subscribe();
		int unsubscribe();
		int read();
		static bool is_supported();
		static void submit_trigger_item();

	private:
		telephony_handle_list_s handle_list;

		social_status_call();
		~social_status_call();

		bool init_telephony();
		void release_telephony();
		bool set_callback();
		void unset_callback();
		bool read_current_status(telephony_h& handle, ctx::Json& data);

		bool get_call_state(telephony_call_h& handle, std::string& state);
		bool get_call_type(telephony_call_h& handle, std::string& type);
		bool get_call_address(telephony_call_h& handle, std::string& address);
		bool get_call_handle_id(telephony_call_h& handle, unsigned int& id);

		void handle_call_event(telephony_h handle, telephony_noti_e noti_id, void* id);
		static void call_event_cb(telephony_h handle, telephony_noti_e noti_id, void *data, void *user_data);
	};
}

#endif // _CONTEXT_SOCIAL_STATUS_CALL_H_
