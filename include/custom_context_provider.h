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

#ifndef __CONTEXT_CUSTOM_CONTEXT_PROVIDER_H__
#define __CONTEXT_CUSTOM_CONTEXT_PROVIDER_H__

namespace ctx {
	bool init_custom_context_provider();

	namespace custom_context_provider {
	int addItem(std::string subject, std::string name, ctx::Json tmpl, const char* owner, bool is_init = false);
	int removeItem(std::string subject);
	int publishData(std::string subject, ctx::Json fact);

	ContextProviderBase* create(void* data);
	void destroy(void* data);
	}
}

#endif //__CONTEXT_CUSTOM_CONTEXT_PROVIDER_H__
