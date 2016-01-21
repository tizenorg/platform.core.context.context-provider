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

#ifndef _CUSTOM_BASE_H_
#define _CUSTOM_BASE_H_

#include <json.h>
#include <provider_iface.h>
#include <types_internal.h>

namespace ctx {

	class custom_base : public context_provider_iface {
	public:
		custom_base(std::string subject, std::string name, ctx::json tmpl, std::string owner);
		~custom_base();

		int subscribe(const char *subject, ctx::json option, ctx::json *request_result);
		int unsubscribe(const char *subject, ctx::json option);
		int read(const char *subject, ctx::json option, ctx::json *request_result);
		int write(const char *subject, ctx::json data, ctx::json *request_result);

		static bool is_supported();
		void submit_trigger_item();
		void unsubmit_trigger_item();

		void handle_update(ctx::json data);

		const char* get_subject();
		std::string get_owner();
		ctx::json get_template();

	private:
		std::string _subject;
		std::string _name;
		ctx::json _tmpl;
		std::string _owner;
		ctx::json latest;
	};
}

#endif // _CUSTOM_BASE_H_
