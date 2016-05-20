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

#ifndef __CONTEXT_PLACE_SENSE_TEST_UTILS_H__
#define __CONTEXT_PLACE_SENSE_TEST_UTILS_H__

#include <iostream>
#include <unordered_map>
#include "my-place/user_places/visit_detector.h"
#include "my-place/user_places/graph.h"
#include <string>
#include <vector>
#include <set>

namespace ctx {

const std::string TEST_FOLDER_PATH = "/tmp/automated_tests/";

const std::string TEST_FILE_INPUT_WIFI_NAME = "wifi_scans.txt";
const std::string TEST_FILE_INPUT_VISITS_NAME = "visits.txt";
const std::string TEST_FILE_INPUT_PLACES_NAME = "places.txt";
const std::string TEST_FILE_INPUT_TIME_ZONE_NAME = "time_zone.txt";
const std::string TEST_FILE_EXPECTED_VISITS_NAME = "visits.txt";
const std::string TEST_FILE_EXPECTED_VISITS_CATEGS_NAME = "visits_categs.txt";
const std::string TEST_FILE_EXPECTED_PLACES_NAME = "places.txt";
const std::string TEST_FILE_EXPECTED_PLACES_CATEGS_NAME = "places_categs.txt";
const std::string TEST_FILE_EXPECTED_PLACES_CATEG_WIFI_NAME = "places_categ_wifi.txt";

enum test_file_type_e {
	TEST_FILE_INPUT_WIFI = 0,
	TEST_FILE_INPUT_VISITS = 1,
	TEST_FILE_INPUT_PLACES = 2,
	TEST_FILE_INPUT_TIME_ZONE = 3,
	TEST_FILE_EXPECTED_VISITS = 4,
	TEST_FILE_EXPECTED_VISITS_CATEGS = 5,
	TEST_FILE_EXPECTED_PLACES = 6,
	TEST_FILE_EXPECTED_PLACES_CATEGS = 7,
	TEST_FILE_EXPECTED_PLACES_CATEG_WIFI = 8,
	TEST_FILE_OTHER = 9
};

enum test_type_e {
	TEST_INPUT_WIFI_EXPECTED_VISITS = 0,
	TEST_INPUT_WIFI_EXPECTED_VISITS_CATEGS = 1,
	TEST_INPUT_WIFI_EXPECTED_PLACES = 2,
	TEST_INPUT_WIFI_EXPECTED_PLACES_CATEGS = 3,
	TEST_INPUT_VISITS_EXPECTED_VISITS_CATEGS = 4,
	TEST_INPUT_VISITS_EXPECTED_PLACES = 5,
	TEST_INPUT_VISITS_EXPECTED_PLACES_CATEGS = 6,
	TEST_INPUT_PLACES_EXPECTED_PLACES_CATEGS = 7,
	TEST_INPUT_WIFI_EXPECTED_VISITS_AND_PLACES = 8,
	TEST_INPUT_WIFI_EXPECTED_PLACES_CATEG_WIFI = 9
};

class UserPlacesTestUtils {
public:
	static const ctx::num_t EPSILON;
	static std::shared_ptr<ctx::MacEvents> events_from_stream(std::istream &input);
	static std::shared_ptr<ctx::MacEvents> events_from_file(std::string filename);
	static void print_mac_events(std::ostream &out, const ctx::MacEvents &logger);
	static void print_mac_counts(std::ostream &out, const ctx::Macs2Counts &counts);
	static void print_mac_shares(std::ostream &out, const ctx::Macs2Shares &shares);
	static void print_mac_set(std::ostream &out, const ctx::MacSet &mac_set);
	static void print_visits(std::ostream &out, std::vector<ctx::Visit> const &visits);
	static void print_visit_sql_insertion(std::ostream &out, ctx::Visit const &visit);
	static void print_visits_sql_insertions(std::ostream &out, ctx::Visits const &visits);
	static void print_graph(std::ostream &out, const ctx::graph::Graph &graph);
	static void print_components(std::ostream &out, const ctx::graph::Components &components);
	static bool equals_tol(const ctx::num_t& v1, const ctx::num_t& v2, const ctx::num_t &tolerance = EPSILON);
	static void find_subfolders_recursively(const std::string path, std::vector<std::string> *folders);
	static std::set<test_type_e> find_tests(const std::string path);
	static std::string read_file(const std::string path);
	static std::shared_ptr<ctx::Visits> visits_from_events(std::shared_ptr<ctx::MacEvents> events);
	static std::vector<std::shared_ptr<ctx::Place>> places_from_visits(std::shared_ptr<ctx::Visits> visits);
	static std::string place_str(const ctx::Place& place);
	static void write_sql_insertions_file(const ctx::Visits &visits);
	static void print_visits(const ctx::Visits& visits);
	static void remove_occurences(std::string& str, char c);
	static std::string read_time_zone(const std::string folder);

private:
	static std::set<test_file_type_e> find_files(const std::string path);
	static void get_file_types(const std::string file_name, std::set<test_file_type_e> *ret);
	static std::string visit_str(const ctx::Visit& v);

};	/* class UserPlacesTestUtils */

}

#endif /* __CONTEXT_PLACE_SENSE_TEST_UTILS_H__ */
