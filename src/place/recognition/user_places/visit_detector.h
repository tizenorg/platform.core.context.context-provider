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

#ifndef __CONTEXT_PLACE_STATUS_VISIT_DETECTOR_H__
#define __CONTEXT_PLACE_STATUS_VISIT_DETECTOR_H__

#include <memory>
#include <vector>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include "user_places_types.h"
#include <Json.h>
#include "visit_listener_iface.h"
#include "location_logger.h"
#include "location_listener_iface.h"
#include "wifi_listener_iface.h"
#include "wifi_logger.h"

namespace ctx {

	class VisitDetector : public IWifiListener, ILocationListener {

	private:
		bool test_mode;
		std::shared_ptr<visits_t> detected_visits;   // only used in test mode
		LocationLogger location_logger;
		WifiLogger wifi_logger;
		std::vector<IVisitListener*> listeners;

		std::shared_ptr<mac_events> current_logger;
		interval_s current_interval;

		std::vector<std::shared_ptr<frame_s>> history_frames;  // python: history_scans  + history_times
		std::vector<std::shared_ptr<frame_s>> buffered_frames; // python: buffered_scans + buffered_times

		int stable_counter;
		int tolerance;
		bool entrance_to_place;
		int period_seconds;

		// fields that  are used only in case of entrance detection
		std::shared_ptr<mac_set_t> rep_macs; // mac that represent the current place
		std::shared_ptr<mac_set_t> stay_macs; // macs that can appear in the current place
		time_t entrance_time;
		time_t departure_time;

		std::vector<location_event_s> locations;

		bool is_valid(const Mac &mac);
		void shift_current_interval();
		void detect_entrance_or_departure(std::shared_ptr<frame_s> frame);
		void detect_entrance(std::shared_ptr<frame_s> frame);
		void detect_departure(std::shared_ptr<frame_s> frame);
		void buffer_processing(std::shared_ptr<frame_s> frame); // python: buffer_anaysing
		std::shared_ptr<frame_s> make_frame(std::shared_ptr<mac_events> mac_events, interval_s interval);  // python: scans2fingerprint
		void history_reset();
		void history_reset(std::shared_ptr<frame_s> frame);
		void visit_start_detected();
		void visit_end_detected();
		void put_visit_location(visit_s &visit);
		std::shared_ptr<mac_set_t> select_representatives(const std::vector<std::shared_ptr<frame_s>> &frames);
		std::shared_ptr<mac_set_t> mac_set_of_greater_or_equal_share(const mac_shares_t &mac_shares, share_t threshold);
		std::shared_ptr<mac_shares_t> mac_shares_from_counts(mac_counts_t const &mac_counts, count_t denominator); // python: response_rate
		share_t calc_max_share(const mac_shares_t &mac_shares);
		bool is_disjoint(const mac_counts_t &mac_counts, const mac_set_t &mac_set);
		bool protrudes_from(const mac_counts_t &mac_counts, const mac_set_t &mac_set);

		void db_create_table();
		void json_put_visit_categ(Json &data, const char* key, const categs_t &categs, int categ_type);
		void json_put_visit_categs(Json &data, const categs_t &categs);
		int db_insert_visit(visit_s visit);
		void set_period(place_recog_mode_e mode);

	public:
		VisitDetector(time_t t_start_scan, place_recog_mode_e energy_mode = PLACE_RECOG_HIGH_ACCURACY_MODE, bool test_mode_ = false);
		~VisitDetector();
		interval_s get_current_interval();
		void on_wifi_scan(mac_event_s event);
		void process_current_logger();
		std::shared_ptr<visits_t> get_visits(); // only used in test mode
		void on_new_location(location_event_s location);
		void set_mode(place_recog_mode_e energy_mode);

	};	/* class VisitDetector */

}	/* namespace ctx */

#endif /* __CONTEXT_PLACE_STATUS_VISIT_DETECTOR_H__ */
