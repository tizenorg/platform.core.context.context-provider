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

#ifndef __CONTEXT_PLACE_STATUS_USER_PLACES_TYPES_H__
#define __CONTEXT_PLACE_STATUS_USER_PLACES_TYPES_H__

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

	std::istream &operator>>(std::istream &input, ctx::mac_set_t &mac_set);
	std::ostream &operator<<(std::ostream &output, const ctx::mac_set_t &mac_set);
	ctx::mac_set_t mac_set_from_string(const std::string &str);

	std::shared_ptr<mac_set_t> mac_sets_union(const std::vector<std::shared_ptr<mac_set_t>> &mac_sets);

	struct interval_s {
		time_t start;
		time_t end;

		interval_s(time_t start_, time_t end_);
	};

}	/* namespace ctx */

namespace std {

	template <> struct hash<ctx::interval_s> {
		size_t operator()(const ctx::interval_s & interval) const {
			return interval.end * interval.start;
		}
	};

}	/* namespace std */

namespace ctx {

	/*
	 * fully describes interval data after the interval is finished
	 */
	struct frame_s {
		interval_s interval;
		count_t no_timestamps;
		mac_counts_t mac_counts;

		frame_s(interval_s interval_) : interval(interval_), no_timestamps(0) {};
	};

	/*
	 * mac address + its timestamp
	 */
	struct mac_event_s {
		time_t timestamp;
		Mac mac;

		mac_event_s(time_t timestamp_, Mac mac_) : timestamp(timestamp_), mac(mac_) {}
	};

	typedef std::map<int, num_t> categs_t; // scores of categories

	struct location_s {
		double latitude;
		double longitude;
		double accuracy; // [m]

		location_s(double latitude_ = 0.0, double longitude_ = 0.0, double accuracy_ = -1.0)
			: latitude(latitude_), longitude(longitude_), accuracy(accuracy_) {}

	};	/* struct location_s */

#ifdef TIZEN_ENGINEER_MODE
	typedef enum {
		LOCATION_METHOD_REQUEST = 0,
		LOCATION_METHOD_GET_LOCATION = 1,
		LOCATION_METHOD_GET_LAST_LOCATION = 2
	} location_source_e;
#endif /* TIZEN_ENGINEER_MODE */

	/*
	 * location + timestamp + method
	 */
	struct location_event_s {
		location_s coordinates;
		time_t timestamp;

#ifdef TIZEN_ENGINEER_MODE
		location_source_e method;

		location_event_s(double latitude_, double longitude_, double accuracy_, time_t timestamp_, location_source_e method_)
			: coordinates(latitude_, longitude_, accuracy_), timestamp(timestamp_), method(method_) {}
#else /* TIZEN_ENGINEER_MODE */
		location_event_s(double latitude_, double longitude_, double accuracy_, time_t timestamp_)
			: coordinates(latitude_, longitude_, accuracy_), timestamp(timestamp_) {}
#endif /* TIZEN_ENGINEER_MODE */

		void log();

	};	/* struct location_event_s */

	struct visit_s {
		interval_s interval;
		std::shared_ptr<mac_set_t> mac_set;
		categs_t categs;
		bool location_valid;
		location_s location; // makes sense if location_valid == true;

		visit_s(interval_s interval_, std::shared_ptr<mac_set_t> mac_set_ = std::make_shared<mac_set_t>(), categs_t categs_ = categs_t()) :
			interval(interval_),
			mac_set(mac_set_),
			categs(categs_),
			location_valid(false) {}
		void set_location(location_s location);
		void print_short_to_stream(std::ostream &out) const;

	};	/* struct visit_s */

	bool operator==(const visit_s &v1, const visit_s &v2);
	typedef std::vector<visit_s> visits_t;
	typedef std::vector<mac_event_s> mac_events; // used to store current interval logs

	std::shared_ptr<mac_set_t> mac_set_from_mac_counts(const mac_counts_t &mac_counts);

	typedef float confidence_t;

	class Place {

	public:
		place_categ_id_e categ_id; // category of a place (work/home/other)
		confidence_t categ_confidence; // confidence of the above category - between [0,1]
		std::string name; // for now: "work"/"home"/"other"
		bool location_valid;
		location_s location; // makes sense if location_valid == true;
		std::string wifi_aps; // WiFi APs MAC addresses separated by ","
		time_t create_date; // The last update time of this place

		void print_to_stream(std::ostream &out) const;

	};	/* class Place */

}	/* namespace ctx */

#endif /*__CONTEXT_PLACE_STATUS_USER_PLACES_TYPES_H__*/
