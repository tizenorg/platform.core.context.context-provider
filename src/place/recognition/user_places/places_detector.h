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

#ifndef __CONTEXT_PLACE_STATUS_PLACES_DETECTOR__
#define __CONTEXT_PLACE_STATUS_PLACES_DETECTOR__

#include "visit_detector.h"
#include <ITimerListener.h>
#include <cstdint>
#include "db_listener_iface.h"
#include "user_places_types.h"
#include <vector>
#include <mutex>

namespace ctx {

	enum {
		PLACES_DETECTOR_QUERY_ID_DELETE_PLACES = 1,
		PLACES_DETECTOR_QUERY_ID_DELETE_OLD_VISITS = 2,
		PLACES_DETECTOR_QUERY_ID_GET_VISITS = 3,
		PLACES_DETECTOR_QUERY_ID_INSERT_VISIT = 4,
		PLACES_DETECTOR_QUERY_ID_INSERT_PLACE = 5,
		PLACES_DETECTOR_QUERY_ID_GET_PLACES = 6
	};

	class PlacesDetector : public ITimerListener, public db_listener_iface {

	private:
		bool test_mode;
		double double_value_from_json(Json &row, const char* key);
		categs_t visit_categs_from_json(Json &row);
		visit_s visit_from_json(Json &row);
		visits_t visits_from_jsons(std::vector<Json>& records);
		std::shared_ptr<ctx::Place> place_from_json(Json &row);
		std::vector<std::shared_ptr<Place>> places_from_jsons(std::vector<Json>& records);
		std::shared_ptr<graph_t> graph_from_visits(const std::vector<visit_s> &visits);
		void db_create_table();
		void db_delete_places();
		void db_delete_old_visits();
		void db_delete_older_visits(time_t threshold);
		void db_get_visits();
		void db_get_places();
		void db_insert_place(const Place &place);
		std::shared_ptr<Place> place_from_merged(visits_t &merged_visits);
		std::vector<std::shared_ptr<Place>> detected_places;
		std::mutex detected_places_access_mutex;
		void detected_places_update(std::vector<std::shared_ptr<Place>> &new_places);

	public:
		static void reduce_outliers(visits_t &visits);
		static void reduce_outliers(std::shared_ptr<components_t> &cc);
		void process_visits(visits_t &visits);
		static void merge_location(const visits_t &merged_visits, Place &place);
		PlacesDetector(bool test_mode_ = false);
		bool onTimerExpired(int timerId);
		void on_creation_result_received(unsigned int query_id, int error) {}
		void on_insertion_result_received(unsigned int query_id, int error, int64_t row_id) {}
		void on_query_result_received(unsigned int query_id, int error, std::vector<Json>& records);
		std::shared_ptr<components_t> merge_visits(const std::vector<visit_s> &visits);
		std::vector<std::shared_ptr<Place>> get_places();

	};  /* class PlacesDetector */

}	/* namespace ctx */

#endif /* __CONTEXT_PLACE_STATUS_PLACES_DETECTOR__ */
