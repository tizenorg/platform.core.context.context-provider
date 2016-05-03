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

#ifndef _CONTEXT_CUSTOM_BASE_H_
#define _CONTEXT_CUSTOM_BASE_H_

#include <Json.h>
#include <ContextProvider.h>
#include <Types.h>

namespace ctx {

	class CustomBase : public ContextProvider {
	public:
		CustomBase(std::string subject, std::string name, ctx::Json tmpl, std::string owner);
		~CustomBase();

		int subscribe(const char *subject, ctx::Json option, ctx::Json *requestResult);
		int unsubscribe(const char *subject, ctx::Json option);
		int read(const char *subject, ctx::Json option, ctx::Json *requestResult);
		int write(const char *subject, ctx::Json data, ctx::Json *requestResult);

		static bool isSupported();
		void submitTriggerItem();
		void unsubmitTriggerItem();

		void handleUpdate(ctx::Json data);

		const char* getSubject();
		std::string getOwner();
		ctx::Json getTemplate();

	private:
		std::string __subject;
		std::string __name;
		ctx::Json __tmpl;
		std::string __owner;
		ctx::Json __latest;
	};
}

#endif	/* End of _CONTEXT_CUSTOM_BASE_H_ */
