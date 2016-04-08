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

#ifndef _CONTEXT_PLACE_RECOGNITION_USER_PLACES_TYPES_H_
#define _CONTEXT_PLACE_RECOGNITION_USER_PLACES_TYPES_H_

#include <memory>
#include <vector>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include "graph.h"
#include "../place_recognition_types.h"
#include <string>
#include <ctime>

namespace ctx {

	/*
	 * type for numerical computations
	 */
	typedef double num_t;

	/*
	 * mac address
	 */
	class Mac {

	public:
		const static size_t MAC_SIZE = 6;  // number of bytes for mac address.
		unsigned char c[MAC_SIZE];

		Mac() {};
		Mac(const std::string &str);
		Mac(const char *str);
		operator std::string() const;

	};	/* class Mac */

	std::istream &operator>>(std::istream &input, ctx::Mac &mac);
	std::ostream &operator<<(std::ostream &output, const ctx::Mac &mac);
	bool operator==(const ctx::Mac &m1, const ctx::Mac &m2);
	bool operator!=(const ctx::Mac &m1, const ctx::Mac &m2);
	bool operator<(const ctx::Mac &m1, const ctx::Mac &m2);
	bool operator>(const ctx::Mac &m1, const ctx::Mac &m2);

}	/* namespace ctx */

namespace std {

	template <> struct hash<ctx::Mac> {
		size_t operator()(const ctx::Mac & m) const {
			size_t h = 1;
			for (size_t i = 0; i < ctx::Mac::MAC_SIZE; i++) {
				h = h * 37 + m.c[i];
			}
			return h;
		}
	};

}	/* namespace std */

namespace ctx {

	typedef float share_t;
	typedef int count_t;

	typedef std::unordered_map<ctx::Mac, ctx::count_t> mac_counts_t;
	typedef std::unordered_map<ctx::Mac, ctx::share_t> mac_shares_t;

	typedef std::unordered_set<ctx::Mac> mac_set_t;

	std::istream &operator>>(std::istream &input, ctx::mac_set_t &macSet);
	std::ostream &operator<<(std::ostream &output, const ctx::mac_set_t &macSet);
	ctx::mac_set_t mac_set_from_string(const std::string &str);

	std::shared_ptr<mac_set_t> mac_sets_union(const std::vector<std::shared_ptr<mac_set_t>> &macSets);

	struct Interval {
		time_t start;
		time_t end;

		Interval(time_t start_, time_t end_);
	};

}	/* namespace ctx */

namespace std {

	template <> struct hash<ctx::Interval> {
		size_t operator()(const ctx::Interval & interval) const {
			return interval.end * interval.start;
		}
	};

}	/* namespace std */

namespace ctx {

	/*
	 * fully describes interval data after the interval is finished
	 */
	struct Frame {
		Interval interval;
		count_t numberOfTimestamps;
		mac_counts_t macCountsMap;

		Frame(Interval interval_) : interval(interval_), numberOfTimestamps(0) {};
	};

	/*
	 * mac address + its timestamp
	 */
	struct MacEvent {
		time_t timestamp;
		Mac mac;

		MacEvent(time_t timestamp_, Mac mac_) : timestamp(timestamp_), mac(mac_) {}
	};

	typedef std::map<int, num_t> categs_t; // scores of categories

	struct Location {
		double latitude;
		double longitude;
		double accuracy; // [m]

		Location(double latitude_ = 0.0, double longitude_ = 0.0, double accuracy_ = -1.0)
			: latitude(latitude_), longitude(longitude_), accuracy(accuracy_) {}

	};	/* struct Location */

#ifdef TIZEN_ENGINEER_MODE
	enum LocationSource {
		LOCATION_METHOD_REQUEST = 0,
		LOCATION_METHOD_GET_LOCATION = 1,
		LOCATION_METHOD_GET_LAST_LOCATION = 2
	};
#endif /* TIZEN_ENGINEER_MODE */

	/*
	 * location + timestamp + method
	 */
	struct LocationEvent {
		Location coordinates;
		time_t timestamp;

#ifdef TIZEN_ENGINEER_MODE
		LocationSource method;

		LocationEvent(double latitude_, double longitude_, double accuracy_, time_t timestamp_, LocationSource method_)
			: coordinates(latitude_, longitude_, accuracy_), timestamp(timestamp_), method(method_) {}
#else /* TIZEN_ENGINEER_MODE */
		LocationEvent(double latitude_, double longitude_, double accuracy_, time_t timestamp_)
			: coordinates(latitude_, longitude_, accuracy_), timestamp(timestamp_) {}
#endif /* TIZEN_ENGINEER_MODE */

		void log();

	};	/* struct LocationEvent */

	struct Visit {
		Interval interval;
		std::shared_ptr<mac_set_t> macSet;
		categs_t categs;
		bool location_valid;
		Location location; // makes sense if location_valid == true;

		Visit(Interval interval_, std::shared_ptr<mac_set_t> macSet_ = std::make_shared<mac_set_t>(), categs_t categs_ = categs_t()) :
			interval(interval_),
			macSet(macSet_),
			categs(categs_),
			location_valid(false) {}
		void set_location(Location location);
		void print_short_to_stream(std::ostream &out) const;

	};	/* struct Visit */

	bool operator==(const Visit &v1, const Visit &v2);
	typedef std::vector<Visit> visits_t;
	typedef std::vector<MacEvent> mac_events; // used to store current interval logs

	std::shared_ptr<mac_set_t> mac_set_from_mac_counts(const mac_counts_t &macCountsMap);

	typedef float confidence_t;

	class Place {

	public:
		PlaceCategId categ_id; // category of a place (work/home/other)
		confidence_t categ_confidence; // confidence of the above category - between [0,1]
		std::string name; // for now: "work"/"home"/"other"
		bool location_valid;
		Location location; // makes sense if location_valid == true;
		std::string wifi_aps; // WiFi APs MAC addresses separated by ","
		time_t create_date; // The last update time of this place

		void print_to_stream(std::ostream &out) const;

	};	/* class Place */

}	/* namespace ctx */

#endif /* End of _CONTEXT_PLACE_RECOGNITION_USER_PLACES_TYPES_H_ */
