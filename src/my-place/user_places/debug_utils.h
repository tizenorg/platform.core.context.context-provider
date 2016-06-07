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

#ifndef _CONTEXT_PLACE_RECOGNITION_DEBUG_UTILS_H_
#define _CONTEXT_PLACE_RECOGNITION_DEBUG_UTILS_H_

#include <string>
#include <ctime>
#include <iostream>
#include <MyPlaceTypes.h>

namespace ctx {

	class DebugUtils {

	public:
		static std::string humanReadableDateTime(time_t timestamp, std::string format, size_t size, bool utc = false);
		static void printPlace2Stream(const Place &place, std::ostream &out);

	};	/* class DebugUtils */

}	/* namespace ctx */

#endif /* End of _CONTEXT_PLACE_RECOGNITION_DEBUG_UTILS_H_ */
