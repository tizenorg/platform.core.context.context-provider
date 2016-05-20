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

#ifndef __CONTEXT_PLACE_STATUS_USER_PLACES_PLACE_SENSE_TEST_H__
#define __CONTEXT_PLACE_STATUS_USER_PLACES_PLACE_SENSE_TEST_H__

#include "test_utils.h"

namespace ctx {

class Test {
public:
	static void utc_user_places_mac_stream();
	static void utc_user_places_mac_compare();
	static void utc_user_places_mac_set_ostream();
	static void utc_user_places_mac_set_istream();
	static void utc_user_places_mac_set_from_string();
	static void utc_user_places_overlap1();
	static void utc_user_places_overlap2();
	static void utc_user_places_mac_set_insert();
	static void utc_user_places_mac_set_find();
	static void utc_user_places_mac_sets_union();
	static void utc_user_places_joint();
	static void utc_user_places_mahal_static_distance1();
	static void utc_user_places_mahal_static_distance2();
	static void utc_user_places_mahal_model_distance1();
	static void utc_user_places_mahal_model_distance2();
	static void utc_user_places_visit_categer_chi_approx();
	static void utc_user_places_visit_categer_time_features();
	static void utc_user_places_visit_categer_weeks_scope();
	static void utc_user_places_visit_categer_time_model_probability_features();
	static void utc_user_places_visit_categer_interval_features();
	static void utc_user_places_visit_categer();
	static void utc_user_places_place_categer_median1();
	static void utc_user_places_place_categer_median2();
	static void utc_user_places_place_categer_categ_id_to_name();
	static void utc_user_places_place_categer_visits_outliers_reduction();
	static void utc_user_places_place_categer();
	static void utc_user_places_places_detector_merge_location1();
	static void utc_user_places_places_detector_merge_location2();
	static void utc_user_places_places_detector_visits_outliers_reduction();
	static void utc_user_places_places_detector_merged_outliers_reduction();
	static void utc_user_places_compose_json_test();
	static void test_input_wifi_expected_visits_and_places(std::string path);
	static void test_input_wifi_expected_places_categ_wifi(std::string path);
	static void run_automated_test(std::string folder_path, test_type_e test);
	static void utc_user_places_automated_test();
};

}

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

	void utc_user_places_mac_stream();
	void utc_user_places_mac_compare();
	void utc_user_places_mac_set_ostream();
	void utc_user_places_mac_set_istream();
	void utc_user_places_mac_set_from_string();
	void utc_user_places_overlap1();
	void utc_user_places_overlap2();
	void utc_user_places_mac_set_insert();
	void utc_user_places_mac_set_find();
	void utc_user_places_mac_sets_union();
	void utc_user_places_joint();
	void utc_user_places_mahal_static_distance1();
	void utc_user_places_mahal_static_distance2();
	void utc_user_places_mahal_model_distance1();
	void utc_user_places_mahal_model_distance2();
	void utc_user_places_visit_categer_chi_approx();
	void utc_user_places_visit_categer_time_features();
	void utc_user_places_visit_categer_weeks_scope();
	void utc_user_places_visit_categer_time_model_probability_features();
	void utc_user_places_visit_categer_interval_features();
	void utc_user_places_visit_categer();
	void utc_user_places_place_categer_median1();
	void utc_user_places_place_categer_median2();
	void utc_user_places_place_categer_categ_id_to_name();
	void utc_user_places_place_categer_visits_outliers_reduction();
	void utc_user_places_place_categer();
	void utc_user_places_places_detector_merge_location1();
	void utc_user_places_places_detector_merge_location2();
	void utc_user_places_places_detector_visits_outliers_reduction();
	void utc_user_places_places_detector_merged_outliers_reduction();
	void utc_user_places_compose_json_test();
	void utc_user_places_automated_test();

#ifdef __cplusplus
}
#endif // __cplusplus

#endif /* __CONTEXT_PLACE_STATUS_USER_PLACES_PLACE_SENSE_TEST_H__ */
