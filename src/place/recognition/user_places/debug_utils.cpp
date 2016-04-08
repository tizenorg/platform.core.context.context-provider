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

#include "debug_utils.h"
#include <types_internal.h>

std::string ctx::DebugUtils::humanReadableDateTime(time_t timestamp, std::string format, size_t size, bool utc)
{
	struct tm * timeinfo;
	if (utc) {
		format += " UTC";
		size += 4;
		timeinfo = gmtime(&timestamp);
	} else {
		timeinfo = localtime(&timestamp);
	}
	char buffer[size];
	if (timeinfo) {
		strftime(buffer, size, format.c_str(), timeinfo);
	} else {
		snprintf(buffer, size, "NULL");
	}
	return std::string(buffer);
}
