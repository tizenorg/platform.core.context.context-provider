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

#include <CreateProvider.h>
#include "CustomManager.h"

using namespace ctx;

extern "C" SO_EXPORT ContextProvider* CreateProvider(const char *subject)
{
	static CustomManager *__customMgr = NULL;

	if (!__customMgr) {
		__customMgr = new(std::nothrow) CustomManager;
		IF_FAIL_RETURN_TAG(__customMgr, NULL, _E, "Memory allocation failed");
	}

	/* Request from client */
	if (STR_EQ(SUBJ_CUSTOM, subject)) {
		return __customMgr;
	}

	/* Request from each custom provider */
	return __customMgr->getProvider(subject);
}
