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

#include <set>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <algorithm>
#include <Types.h>
#include "UserPlacesTypes.h"
#include "UserPlacesParams.h"
#include "../utils/DebugUtils.h"

#define __MAC_STRING_COMPONENTS_SEPARATOR ':'
#define __MAC_SET_STRING_DELIMITER ','

ctx::Mac::Mac(const std::string& str)
{
	std::stringstream ss(str);
	try {
		ss >> *this;
	} catch (std::runtime_error &e) {
		_E("%s", e.what());
	}
}

ctx::Mac::Mac(const char *str)
{
	std::stringstream ss(str);
	try {
		ss >> *this;
	} catch (std::runtime_error &e) {
		_E("%s", e.what());
	}
}

std::istream& ctx::operator>>(std::istream &input, ctx::Mac &mac)
{
	int h;
	char colon;
	for (size_t i = 0; i < ctx::Mac::MAC_SIZE; i++) {
		input >> std::hex;
		input >> h;
		mac.c[i] = h;
		if (i + 1 >= ctx::Mac::MAC_SIZE)
			break;
		input >> colon;
		if (colon != __MAC_STRING_COMPONENTS_SEPARATOR)
			throw std::runtime_error("Invalid MAC format");
	}
	input >> std::dec;
	return input;
}

std::ostream& ctx::operator<<(std::ostream &output, const ctx::Mac &mac)
{
	size_t i = 0;
	while (true) {
		output << std::hex << std::setfill('0') << std::setw(2);
		output << static_cast<int>(mac.c[i]);
		i++;
		if (i >= Mac::MAC_SIZE)
			break;
		output << __MAC_STRING_COMPONENTS_SEPARATOR;
	}
	output << std::dec;
	return output;
}

ctx::Mac::operator std::string() const
{
	std::stringstream ss;
	ss << *this;
	return ss.str();
}

bool ctx::operator==(const Mac &m1, const Mac &m2)
{
	for (size_t i = 0; i < Mac::MAC_SIZE; i++) {
		if (m1.c[i] != m2.c[i])
			return false;
	}
	return true;
}

bool ctx::operator!=(const Mac &m1, const Mac &m2)
{
	return !(m1 == m2);
}

bool ctx::operator<(const Mac &m1, const Mac &m2)
{
	unsigned char c1, c2;
	for (size_t i = 0; i < Mac::MAC_SIZE; i++) {
		c1 = m1.c[i];
		c2 = m2.c[i];
		if (c1 < c2)
			return true;
		if (c1 > c2)
			return false;
	}
	return false; // they are equal
}

std::istream& ctx::operator>>(std::istream &input, ctx::MacSet &macSet)
{
	Mac mac;
	char delimeter;
	while (!input.eof()) {
		try {
			input >> mac;
		} catch (std::runtime_error &e) {
			_E("Cannot read macSet. Exception: %s", e.what());
			break;
		}
		macSet.insert(mac);
		if (input.eof())
			break;
		delimeter = input.get();
		if (delimeter != __MAC_SET_STRING_DELIMITER) {
			input.unget();
			break;
		}
	}
	return input;
}

std::ostream& ctx::operator<<(std::ostream &output, const ctx::MacSet &macSet)
{
	std::vector<Mac> macVec(macSet.size());
	std::copy(macSet.begin(), macSet.end(), macVec.begin());
	std::sort(macVec.begin(), macVec.end());

	bool first = true;
	for (auto &mac: macVec) {
		if (first) {
			first = false;
		} else {
			output << __MAC_SET_STRING_DELIMITER;
		}
		output << mac;
	}
	return output;
}

void ctx::LocationEvent::log()
{
	std::string time_str = DebugUtils::humanReadableDateTime(timestamp, "%T", 9);
#ifdef TIZEN_ENGINEER_MODE
	_D("location lat=%.8f, lon=%.8f, acc=%.2f[m], time=%s, method=%d",
			coordinates.latitude,
			coordinates.longitude,
			coordinates.accuracy,
			time_str.c_str(),
			method);
#else /* TIZEN_ENGINEER_MODE */
	_D("location lat=%.8f, lon=%.8f, acc=%.2f[m], time=%s",
			coordinates.latitude,
			coordinates.longitude,
			coordinates.accuracy,
			time_str.c_str());
#endif /* TIZEN_ENGINEER_MODE */
}

void ctx::Visit::setLocation(Location location_)
{
	locationValid = true;
	location = location_;
}

void ctx::Visit::printShort2Stream(std::ostream &out) const
{
	// print only valid visits
	if (interval.end != 0) {
		float duration = ((float) (interval.end - interval.start)) / 3600; // [h]
		out << "__VISIT " << duration << "h: ";
			out << DebugUtils::humanReadableDateTime(interval.start, "%m/%d %H:%M", 15) << " ÷ ";
			out << DebugUtils::humanReadableDateTime(interval.end, "%m/%d %H:%M", 15) << std::endl;
	}
}

bool ctx::operator==(const ctx::Visit &v1, const ctx::Visit &v2)
{
	return v1.interval.start == v2.interval.start
			&& v1.interval.end == v2.interval.end
			&& v1.categs == v2.categs
			&& v1.location.latitude == v2.location.latitude
			&& v1.location.longitude == v2.location.longitude
			&& v1.location.accuracy == v2.location.accuracy
			&& v1.locationValid == v2.locationValid
			&& v1.macSet == v2.macSet;
}

ctx::MacSet ctx::macSetFromString(const std::string &str)
{
	MacSet macSet;
	std::stringstream ss;
	ss << str;
	ss >> macSet;
	return macSet;
}

bool ctx::operator>(const Mac &m1, const Mac &m2)
{
	return m2 < m1;
}

std::shared_ptr<ctx::MacSet> ctx::macSetFromMacs2Counts(const Macs2Counts &macs2Counts)
{
	std::shared_ptr<MacSet> macSet(std::make_shared<MacSet>());
	for (auto &macCount: macs2Counts) {
		macSet->insert(macCount.first);
	}
	return macSet;
}

std::shared_ptr<ctx::MacSet> ctx::macSetsUnion(const std::vector<std::shared_ptr<MacSet>> &macSets)
{
	std::shared_ptr<MacSet> unionSet = std::make_shared<MacSet>();
	for (std::shared_ptr<MacSet> macSet : macSets) {
		unionSet->insert(macSet->begin(), macSet->end());
	}
	return unionSet;
}

ctx::Interval::Interval(time_t start_, time_t end_) : start(start_), end(end_) {
	if (end_ < start_)
		_E("Negative interval, start=%d, end=%d", start_, end_);
}
