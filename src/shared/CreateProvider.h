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

#ifndef _CONTEXT_PROVIDER_CREATE_H_
#define _CONTEXT_PROVIDER_CREATE_H_

#include <new>
#include <Types.h>

#define ADD_PROVIDER(subj, prvd) \
	if (STR_EQ((subj), subject)) { \
		ctx::ContextProvider *instance = new(std::nothrow) prvd; \
		if (instance == NULL) { \
			_E("Memoroy allocation failed"); \
			return NULL; \
		} \
		return instance; \
	}

#endif	/* _CONTEXT_PROVIDER_CREATE_H_ */
