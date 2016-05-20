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

#include "test_utils.h"
#include <math.h>
#include <fstream>
#include "my-place/user_places/debug_utils.h"
#include "my-place/user_places/visit_detector.h"
#include "my-place/user_places/places_detector.h"
#include "my-place/user_places/place_categer.h"
#include "my-place/place_recognition_types.h"
#include <dirent.h>
#include <sys/stat.h>
#include <sstream>
#include <fstream>
#include <algorithm>

std::shared_ptr<ctx::MacEvents> ctx::UserPlacesTestUtils::events_from_stream(std::istream &input)
{
	std::shared_ptr<ctx::MacEvents> logger = std::make_shared<ctx::MacEvents>();
	time_t timestamp;
	ctx::Mac mac;

	while (true) {
		std::string dev_str;
		if (!(input >> timestamp)) break;
		if (!(input >> mac)) break;
		ctx::MacEvent e(timestamp, mac);
		logger->push_back(e);
	}
	return logger;
}

std::shared_ptr<ctx::MacEvents> ctx::UserPlacesTestUtils::events_from_file(std::string filename)
{
	std::ifstream stream(filename);
	return events_from_stream(stream);
}

void ctx::UserPlacesTestUtils::print_mac_events(std::ostream &out, const ctx::MacEvents &logger)
{
	for (auto &e : logger) {
		out << e.timestamp << " " << e.mac << std::endl;
	}
}

void ctx::UserPlacesTestUtils::print_mac_counts(std::ostream &out, const ctx::Macs2Counts &counts)
{
	for (auto &c : counts) {
		out << c.first << " " << c.second << std::endl;
	}
}

void ctx::UserPlacesTestUtils::print_mac_shares(std::ostream &out, const ctx::Macs2Shares &shares)
{
	for (auto &s : shares) {
		out << s.first << " " << s.second << std::endl;
	}
}

void ctx::UserPlacesTestUtils::print_mac_set(std::ostream &out, const ctx::MacSet &mac_set)
{
	for (auto &e : mac_set) {
		out << e << std::endl;
	}
}

void ctx::UserPlacesTestUtils::print_visits(std::ostream &out, std::vector<ctx::Visit> const &visits)
{
	out <<  "Visits: " << visits.size() << std::endl;

	int i = 0;
	for (auto &visit : visits) {
		time_t start = visit.interval.start;
		time_t end = visit.interval.end;
		float duration = ((float) (end - start)) / 3600; // [h]

		out << "[" << i << "] = [" << ctx::DebugUtils::humanReadableDateTime(start, "%F %H:%M", 80, true);
		out << "  -->  " << ctx::DebugUtils::humanReadableDateTime(end, "%F %H:%M", 80, true) << "]";
		out  << "  :  " << duration << " h" << std::endl;
		i++;
	}
}

void ctx::UserPlacesTestUtils::print_graph(std::ostream &out, ctx::graph::Graph const &graph)
{
	for (size_t i = 0; i < graph.size(); i++) {
		out << "[" <<  i <<  "] ->";
		for (ctx::graph::Node n : *graph[i]) {
			out << " " << n;
		}
		out << std::endl;
	}
}

void ctx::UserPlacesTestUtils::print_components(std::ostream &out, const ctx::graph::Components &components)
{
	out << "Distinct user places: " << components.size() << std::endl;
	for (std::shared_ptr<ctx::graph::Component> component : components) {
		out << "[";
		for (ctx::graph::Node node : *component) {
			out << node << ", ";
		}
		out << "]" << std::endl;
	}
}

void ctx::UserPlacesTestUtils::print_visit_sql_insertion(std::ostream &out, ctx::Visit const &visit)
{
	ctx::Interval const &interval = visit.interval;
	ctx::Categs const &categs = visit.categs;

	g_assert (categs.find(PLACE_CATEG_ID_HOME) != categs.end());
	g_assert (categs.find(PLACE_CATEG_ID_WORK) != categs.end());
	g_assert (categs.find(PLACE_CATEG_ID_OTHER) != categs.end());

	out << "INSERT INTO " << VISIT_TABLE;
	out << "(";
	out << VISIT_COLUMN_START_TIME << ",";
	out << VISIT_COLUMN_END_TIME << ",";
#ifdef TIZEN_ENGINEER_MODE
	out << VISIT_COLUMN_START_TIME_HUMAN << ",";
	out << VISIT_COLUMN_END_TIME_HUMAN << ",";
#endif /* TIZEN_ENGINEER_MODE */
	out << VISIT_COLUMN_WIFI_APS << ",";
	out << VISIT_COLUMN_CATEG_HOME << ",";
	out << VISIT_COLUMN_CATEG_WORK << ",";
	out << VISIT_COLUMN_CATEG_OTHER;
	out << ") VALUES (";
	out << interval.start << ",";
	out << interval.end << ",";
#ifdef TIZEN_ENGINEER_MODE
	std::string start_time_human = ctx::DebugUtils::humanReadableDateTime(interval.start, "%F %T", 80);
	std::string end_time_human = ctx::DebugUtils::humanReadableDateTime(interval.end, "%F %T", 80);
	out << "'" << start_time_human << "'" << ",";
	out << "'" << end_time_human << "'" << ",";
#endif /* TIZEN_ENGINEER_MODE */
	out << "'" << *visit.macSet << "'" << ",";
	out << categs.find(PLACE_CATEG_ID_HOME)->second << ",";
	out << categs.find(PLACE_CATEG_ID_WORK)->second << ",";
	out << categs.find(PLACE_CATEG_ID_OTHER)->second;
	out << ");";
}

void ctx::UserPlacesTestUtils::print_visits_sql_insertions(std::ostream &out, ctx::Visits const &visits)
{
	for (ctx::Visit const &visit : visits) {
		print_visit_sql_insertion(out, visit);
		out << std::endl;
	}
}

const ctx::num_t ctx::UserPlacesTestUtils::EPSILON = .0001;

bool ctx::UserPlacesTestUtils::equals_tol(const ctx::num_t& v1, const ctx::num_t& v2, const ctx::num_t &tolerance)
{
	return fabs(v1 - v2) < tolerance;
}

void ctx::UserPlacesTestUtils::find_subfolders_recursively(const std::string path, std::vector<std::string> *folders)
{
	DIR *dir;
	if ((dir = opendir(path.c_str())) != NULL) {
		struct dirent *ent;
		while ((ent = readdir(dir)) != NULL) {
			if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0) {
				continue;
			}
			struct stat buf;
			lstat((path + ent->d_name).c_str(), &buf);
			if (S_ISDIR(buf.st_mode)) {
				find_subfolders_recursively(path + ent->d_name + "/", folders);
			}
		}
		closedir(dir);
		folders->push_back(path);
	} else {
		//g_print("\n ERROR open dir = %s \n", path.c_str());
	}
}

std::set<ctx::test_file_type_e> ctx::UserPlacesTestUtils::find_files(const std::string path)
{
	std::set<test_file_type_e> ret;
	DIR *dir;
	if ((dir = opendir(path.c_str())) != NULL) {
		struct dirent *ent;
		while ((ent = readdir(dir)) != NULL) {
			if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0) {
				continue;
			}
			struct stat buf;
			lstat((path + ent->d_name).c_str(), &buf);
			if (S_ISREG(buf.st_mode)) {
				get_file_types(ent->d_name, &ret);
			}
		}
		closedir(dir);
	} else {
		g_print("\n ERROR open dir = %s \n", path.c_str());
	}
	return ret;
}

std::set<ctx::test_type_e> ctx::UserPlacesTestUtils::find_tests(const std::string path)
{
	std::set<test_type_e> ret;
	std::set<test_file_type_e> files = UserPlacesTestUtils::find_files(path);
	if (files.find(TEST_FILE_INPUT_TIME_ZONE) != files.end()) {
		if (files.find(TEST_FILE_INPUT_WIFI) != files.end()
				&& files.find(TEST_FILE_EXPECTED_VISITS) != files.end()
				&& files.find(TEST_FILE_EXPECTED_PLACES) != files.end()) {
			ret.insert(TEST_INPUT_WIFI_EXPECTED_VISITS_AND_PLACES);
		}
		if (files.find(TEST_FILE_INPUT_WIFI) != files.end()) {
			if (files.find(TEST_FILE_EXPECTED_VISITS) != files.end()) {
				ret.insert(TEST_INPUT_WIFI_EXPECTED_VISITS);
			}
			if (files.find(TEST_FILE_EXPECTED_VISITS_CATEGS) != files.end()) {
				ret.insert(TEST_INPUT_WIFI_EXPECTED_VISITS_CATEGS);
			}
			if (files.find(TEST_FILE_EXPECTED_PLACES) != files.end()) {
				ret.insert(TEST_INPUT_WIFI_EXPECTED_PLACES);
			}
			if (files.find(TEST_FILE_EXPECTED_PLACES_CATEGS) != files.end()) {
				ret.insert(TEST_INPUT_WIFI_EXPECTED_PLACES_CATEGS);
			}
			if (files.find(TEST_FILE_EXPECTED_PLACES_CATEG_WIFI) != files.end()) {
				ret.insert(TEST_INPUT_WIFI_EXPECTED_PLACES_CATEG_WIFI);
			}
		}
		if (files.find(TEST_FILE_INPUT_VISITS) != files.end()) {
			if (files.find(TEST_FILE_EXPECTED_VISITS_CATEGS) != files.end()) {
				ret.insert(TEST_INPUT_WIFI_EXPECTED_VISITS_CATEGS);
			}
			if (files.find(TEST_FILE_EXPECTED_PLACES) != files.end()) {
				ret.insert(TEST_INPUT_WIFI_EXPECTED_PLACES);
			}
			if (files.find(TEST_FILE_EXPECTED_PLACES_CATEGS) != files.end()) {
				ret.insert(TEST_INPUT_WIFI_EXPECTED_PLACES_CATEGS);
			}
		}
		if (files.find(TEST_FILE_INPUT_PLACES) != files.end()) {
			if (files.find(TEST_FILE_EXPECTED_PLACES_CATEGS) != files.end()) {
				ret.insert(TEST_INPUT_PLACES_EXPECTED_PLACES_CATEGS);
			}
		}
	}
	return ret;
}

std::string ctx::UserPlacesTestUtils::read_file(const std::string path)
{
	std::ifstream infile(path, std::ifstream::in);
	if (!infile.good()) {
		return "";
	}
	std::stringstream ss;
	std::string temp;
	while(getline(infile, temp)) {
		ss << temp << '\n';
	}
	infile.close();
	return ss.str();
}

void ctx::UserPlacesTestUtils::get_file_types(const std::string file_name, std::set<test_file_type_e> *ret)
{
	if (file_name == TEST_FILE_INPUT_WIFI_NAME) {
		ret->insert(TEST_FILE_INPUT_WIFI);
	}
	if (file_name == TEST_FILE_INPUT_VISITS_NAME) {
		ret->insert(TEST_FILE_INPUT_VISITS);
	}
	if (file_name == TEST_FILE_INPUT_PLACES_NAME) {
		ret->insert(TEST_FILE_INPUT_PLACES);
	}
	if (file_name == TEST_FILE_INPUT_TIME_ZONE_NAME) {
		ret->insert(TEST_FILE_INPUT_TIME_ZONE);
	}
	if (file_name == TEST_FILE_EXPECTED_VISITS_NAME) {
		ret->insert(TEST_FILE_EXPECTED_VISITS);
	}
	if (file_name == TEST_FILE_EXPECTED_VISITS_CATEGS_NAME) {
		ret->insert(TEST_FILE_EXPECTED_VISITS_CATEGS);
	}
	if (file_name == TEST_FILE_EXPECTED_PLACES_NAME) {
		ret->insert(TEST_FILE_EXPECTED_PLACES);
	}
	if (file_name == TEST_FILE_EXPECTED_PLACES_CATEGS_NAME) {
		ret->insert(TEST_FILE_EXPECTED_PLACES_CATEGS);
	}
	if (file_name == TEST_FILE_EXPECTED_PLACES_CATEG_WIFI_NAME) {
		ret->insert(TEST_FILE_EXPECTED_PLACES_CATEG_WIFI);
	}
}

std::shared_ptr<ctx::Visits> ctx::UserPlacesTestUtils::visits_from_events(std::shared_ptr<ctx::MacEvents> events)
{
	time_t start_scan = (*events)[0].timestamp;
	ctx::VisitDetector visit_detector(start_scan, PLACE_RECOG_HIGH_ACCURACY_MODE, true);
	for (auto &event : *events) {
		visit_detector.onWifiScan(event);
	}
	return visit_detector.getVisits();
}

std::vector<std::shared_ptr<ctx::Place>> ctx::UserPlacesTestUtils::places_from_visits(std::shared_ptr<ctx::Visits> visits)
{
	ctx::PlacesDetector places_detector(true);
	places_detector.__processVisits(*visits);
	return places_detector.getPlaces();
}

std::string ctx::UserPlacesTestUtils::place_str(const ctx::Place &place)
{
	return ctx::PlaceCateger::categId2Name(place.categId) + " " + place.wifiAps;
}

/*
 * for place_detection tests during development
 * */
void ctx::UserPlacesTestUtils::write_sql_insertions_file(const ctx::Visits &visits)
{
	std::ofstream out;
	out.open("/tmp/visits_insert.sql");
	UserPlacesTestUtils::print_visits_sql_insertions(out, visits);
}

std::string ctx::UserPlacesTestUtils::visit_str(const ctx::Visit& v)
{
	std::stringstream ss;
	ss << v.categs.at(PLACE_CATEG_ID_HOME) << ", ";
	ss << v.categs.at(PLACE_CATEG_ID_WORK) << ", ";
	ss << v.categs.at(PLACE_CATEG_ID_OTHER) << " ";
	ss << v.interval.start << " " << v.interval.end << " ";
	ss << *v.macSet;
	return ss.str();
}

void ctx::UserPlacesTestUtils::print_visits(const ctx::Visits& visits)
{
	std::cout << std::endl << "vists:" << std::endl;
	for (const ctx::Visit& visit : visits) {
		std::cout << visit_str(visit) << std::endl;
	}
}

void ctx::UserPlacesTestUtils::remove_occurences(std::string& str, char c)
{
	str.erase(std::remove(str.begin(), str.end(), c), str.end());
}

std::string ctx::UserPlacesTestUtils::read_time_zone(const std::string folder)
{
	std::string time_zone = read_file(folder + TEST_FILE_INPUT_TIME_ZONE_NAME);
	remove_occurences(time_zone, '\n');
	remove_occurences(time_zone, '\r');
	return time_zone;
}
