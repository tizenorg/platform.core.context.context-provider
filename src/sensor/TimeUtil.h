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
 *
 */

#ifndef __CONTEXT_TIME_UTIL_H__
#define __CONTEXT_TIME_UTIL_H__

#include <stdint.h>

namespace ctx {

	class TimeUtil {
	public:
		static uint64_t getTime();
		static uint64_t getTime(unsigned long long monotonic);

	private:
		TimeUtil();
	};

}

#endif /* __CONTEXT_TIME_UTIL_H__ */
