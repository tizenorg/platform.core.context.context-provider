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

#ifndef _CONTEXT_BASIC_PROVIDER_H_
#define _CONTEXT_BASIC_PROVIDER_H_

#include <ContextProvider.h>

namespace ctx {

	/* BasicProvider implements the very basic form of context providers,
	   which has no controllable options, and does not set the requestResult
	   parameter to reply to clients' requests immediately. */
	class BasicProvider : public ContextProvider {
	public:
		int subscribe(Json option, Json *requestResult);
		int unsubscribe(Json option);
		int read(Json option, Json *requestResult);
		int write(Json data, Json *requestResult);

		virtual bool isSupported();

		/* TODO: This function will be deprecated */
		virtual void submitTriggerItem();

	protected:
		bool __beingSubscribed;

		BasicProvider(const char *subject);
		virtual ~BasicProvider();

		virtual int subscribe();
		virtual int unsubscribe();
		virtual int read();
		virtual int write(Json &data);

		/* TODO: This function needs to be removed from here */
		static bool getSystemInfoBool(const char *key);
	};
}

#endif	/* _CONTEXT_BASIC_PROVIDER_H_ */
