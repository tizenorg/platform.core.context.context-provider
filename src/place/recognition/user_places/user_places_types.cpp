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

#include <set>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <algorithm>
#include <types_internal.h>
#include "user_places_types.h"
#include "user_places_params.h"
#include "debug_utils.h"

#define MAC_STRING_COMPONENTS_SEPARATOR ':'
#define MAC_SET_STRING_DELIMITER ','

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
		if (i + 1 >= ctx::Mac::MAC_SIZE) {
			break;
		}
		input >> colon;
		if (colon != MAC_STRING_COMPONENTS_SEPARATOR) {
			throw std::runtime_error("Invalid MAC format");
		}
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
		if (i >= Mac::MAC_SIZE) {
			break;
		}
		output << MAC_STRING_COMPONENTS_SEPARATOR;
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
		if (m1.c[i] != m2.c[i]) {
			return false;
		}
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
		if (c1 < c2) {
			return true;
		}
		if (c1 > c2) {
			return false;
		}
	}
	return false; // they are equal
}

std::istream& ctx::operator>>(std::istream &input, ctx::mac_set_t &mac_set)
{
	Mac mac;
	char delimeter;
	while (!input.eof()) {
		try {
			input >> mac;
		} catch (std::runtime_error &e) {
			_E("Cannot read mac_set. Exception: %s", e.what());
			break;
		}
		mac_set.insert(mac);
		if (input.eof()) {
			break;
		}
		delimeter = input.get();
		if (delimeter != MAC_SET_STRING_DELIMITER) {
			input.unget();
			break;
		}
	}
	return input;
}

std::ostream& ctx::operator<<(std::ostream &output, const ctx::mac_set_t &mac_set)
{
	std::vector<Mac> mac_vec(mac_set.size());
	std::copy(mac_set.begin(), mac_set.end(), mac_vec.begin());
	std::sort(mac_vec.begin(), mac_vec.end());

	bool first = true;
	for (auto &mac: mac_vec) {
		if (first) {
			first = false;
		} else {
			output << MAC_SET_STRING_DELIMITER;
		}
		output << mac;
	}
	return output;
}

void ctx::location_event_s::log()
{
	std::string time_str = DebugUtils::human_readable_date_time(timestamp, "%T", 9);
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

void ctx::visit_s::set_location(location_s location_)
{
	location_valid = true;
	location = location_;
}

void ctx::visit_s::print_short_to_stream(std::ostream &out) const
{
	// print only valid visits
	if (interval.end != 0) {
		float duration = ((float) (interval.end - interval.start)) / 3600; // [h]
		out << "__VISIT " << duration << "h: ";
			out << DebugUtils::human_readable_date_time(interval.start, "%m/%d %H:%M", 15) << " ÷ ";
			out << DebugUtils::human_readable_date_time(interval.end, "%m/%d %H:%M", 15) << std::endl;
	}
}

bool ctx::operator==(const ctx::visit_s &v1, const ctx::visit_s &v2)
{
	return v1.interval.start == v2.interval.start
			&& v1.interval.end == v2.interval.end
			&& v1.categs == v2.categs
			&& v1.location.latitude == v2.location.latitude
			&& v1.location.longitude == v2.location.longitude
			&& v1.location.accuracy == v2.location.accuracy
			&& v1.location_valid == v2.location_valid
			&& v1.mac_set == v2.mac_set;
}

ctx::mac_set_t ctx::mac_set_from_string(const std::string &str)
{
	mac_set_t mac_set;
	std::stringstream ss;
	ss << str;
	ss >> mac_set;
	return mac_set;
}

bool ctx::operator>(const Mac &m1, const Mac &m2)
{
	return m2 < m1;
}

std::shared_ptr<ctx::mac_set_t> ctx::mac_set_from_mac_counts(const mac_counts_t &mac_counts)
{
	std::shared_ptr<mac_set_t> mac_set(std::make_shared<mac_set_t>());
	for (auto &c: mac_counts) {
		mac_set->insert(c.first);
	}
	return mac_set;
}

std::shared_ptr<ctx::mac_set_t> ctx::mac_sets_union(const std::vector<std::shared_ptr<mac_set_t>> &mac_sets)
{
	std::shared_ptr<mac_set_t> union_set = std::make_shared<mac_set_t>();
	for (std::shared_ptr<mac_set_t> mac_set : mac_sets) {
		union_set->insert(mac_set->begin(), mac_set->end());
	}
	return union_set;
}

ctx::interval_s::interval_s(time_t start_, time_t end_) : start(start_), end(end_) {
	if (end_ < start_) {
		_E("Negative interval, start=%d, end=%d", start_, end_);
	}
}

void ctx::Place::print_to_stream(std::ostream &out) const
{
	out << "PLACE:" << std::endl;
	out << "__CATEGORY: " << name << std::endl;
	if (location_valid) {
		out << "__LOCATION: lat=" << std::setprecision(GEO_LOCATION_PRECISION + 2) << location.latitude;
		out << ", lon=" << location.longitude << std::setprecision(5) << std::endl;
	}
	out << "__WIFI:" << wifi_aps << std::endl;
	out << "__CREATE_DATE: " << DebugUtils::human_readable_date_time(create_date, "%F %T", 80) << std::endl;
}
