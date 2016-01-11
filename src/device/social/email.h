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

#include <dbus_listener_iface.h>
#include "../provider_base.h"

namespace ctx {

	class social_status_email : public device_provider_base, public dbus_listener_iface {

		GENERATE_PROVIDER_COMMON_DECL(social_status_email);

	public:
		int subscribe();
		int unsubscribe();
		void on_signal_received(const char* sender, const char* path, const char* iface, const char* name, GVariant* param);
		static bool is_supported();
		static void submit_trigger_item();

	private:
		int64_t dbus_signal_id;

		social_status_email();
		~social_status_email();
	};
}

#endif // _CONTEXT_SOCIAL_STATUS_EMAIL_H_
