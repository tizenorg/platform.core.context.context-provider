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
#include "../device_provider_base.h"

namespace ctx {

	class social_status_contacts : public device_provider_base {

		GENERATE_PROVIDER_COMMON_DECL(social_status_contacts);

	public:
		int subscribe();
		int unsubscribe();
		static bool is_supported();
		static void submit_trigger_item();

	private:
		time_t latest_my_profile;
		time_t latest_person;

		social_status_contacts();
		~social_status_contacts();

		bool set_callback();
		void unset_callback();
		void handle_db_change(const char* view_uri);
		static void db_change_cb(const char* view_uri, void* user_data);
		bool is_consecutive_change(const char* view_uri);
	};
}

#endif // _CONTEXT_SOCIAL_STATUS_CONTACTS_H_
