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

#ifndef _CONTEXT_RUNTIME_INFO_PROVIDER_H_
#define _CONTEXT_RUNTIME_INFO_PROVIDER_H_

#include <runtime_info.h>
#include <ProviderTypes.h>
#include <BasicProvider.h>

namespace ctx {

	class RuntimeInfoProvider : public BasicProvider {
	public:
		RuntimeInfoProvider(const char *subject, runtime_info_key_e key);

		int subscribe();
		int unsubscribe();
		virtual int read() = 0;

	protected:
		runtime_info_key_e __infoKey;

		virtual ~RuntimeInfoProvider(){}
		static void updateCb(runtime_info_key_e runtimeKey, void* userData);
		virtual void handleUpdate() = 0;

	private:
		runtime_info_key_e __getInfoKey();
	};
}

#endif /* _CONTEXT_RUNTIME_INFO_PROVIDER_H_ */
