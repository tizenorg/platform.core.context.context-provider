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

#ifndef _CONTEXT_CUSTOM_MANAGER_H_
#define _CONTEXT_CUSTOM_MANAGER_H_

#include <CustomRegister.h>
#include <BasicProvider.h>
#include <ProviderTypes.h>

namespace ctx {

	class CustomManager : public BasicProvider, CustomRegister {
	public:
		CustomManager();
		~CustomManager();

		int subscribe();
		int unsubscribe();
		int write(Json data, Json *requestResult);

		bool isSupported();

		bool unloadable();

		ContextProvider* getProvider(const char* subject);

	private:
		bool __initialize();
		int __addCustomItem(std::string subject, std::string name, ctx::Json tmpl, std::string owner, bool isInit = false);
		int __removeCustomItem(std::string subject);
		int __publishData(std::string, ctx::Json fact);
	};
}

#endif	/* End of _CONTEXT_CUSTOM_MANAGER_H_ */
