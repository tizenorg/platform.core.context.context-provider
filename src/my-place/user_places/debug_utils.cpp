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

#include "debug_utils.h"
#include <Types.h>
#include <iomanip>

/*
 * Number of digits after decimal point used in geo coordinates.
 */
#define GEO_LOCATION_PRECISION 7

std::string ctx::DebugUtils::humanReadableDateTime(time_t timestamp, std::string format, size_t size, bool utc)
{
	struct tm timeinfo;
	struct tm *result;
	tzset();
	if (utc) {
		format += " UTC";
		size += 4;
		result = gmtime_r(&timestamp, &timeinfo);
	} else {
		result = localtime_r(&timestamp, &timeinfo);
	}
	char buffer[size];
	if (result) {
		strftime(buffer, size, format.c_str(), &timeinfo);
	} else {
		snprintf(buffer, size, "NULL");
	}
	return std::string(buffer);
}

void ctx::DebugUtils::printPlace2Stream(const Place &place, std::ostream &out)
{
	out << "PLACE:" << std::endl;
	out << "__CATEGORY: " << place.name << std::endl;
	if (place.locationValid) {
		out << "__LOCATION: lat=" << std::setprecision(GEO_LOCATION_PRECISION + 2) << place.location.latitude;
		out << ", lon=" << place.location.longitude << std::setprecision(5) << std::endl;
	}
	out << "__WIFI:" << place.wifiAps << std::endl;
	out << "__CREATE_DATE: " << humanReadableDateTime(place.createDate, "%F %T", 80) << std::endl;
}
