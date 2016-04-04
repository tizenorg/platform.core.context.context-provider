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

#include <Json.h>
#include <ContextProviderBase.h>
#include <types_internal.h>

namespace ctx {

	class custom_base : public ContextProviderBase {
	public:
		custom_base(std::string subject, std::string name, ctx::Json tmpl, std::string owner);
		~custom_base();

		int subscribe(const char *subject, ctx::Json option, ctx::Json *requestResult);
		int unsubscribe(const char *subject, ctx::Json option);
		int read(const char *subject, ctx::Json option, ctx::Json *requestResult);
		int write(const char *subject, ctx::Json data, ctx::Json *requestResult);

		static bool is_supported();
		void submit_trigger_item();
		void unsubmit_trigger_item();

		void handle_update(ctx::Json data);

		const char* get_subject();
		std::string get_owner();
		ctx::Json get_template();

	private:
		std::string _subject;
		std::string _name;
		ctx::Json _tmpl;
		std::string _owner;
		ctx::Json latest;
	};
}

#endif // _CUSTOM_BASE_H_
