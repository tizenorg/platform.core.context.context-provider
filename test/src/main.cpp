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

#include <stdlib.h>
#include <glib.h>
#include <glib-object.h>

#include "utc_visits_and_places/visits_and_places_test.h"

static void signal_handler(int signo)
{
	g_print("\nSIGNAL %d received\n", signo);
	exit(EXIT_SUCCESS);
}

int main(int argc, char** argv)
{
	signal(SIGINT,  signal_handler);
	signal(SIGHUP,  signal_handler);
	signal(SIGTERM, signal_handler);
	signal(SIGQUIT, signal_handler);
	signal(SIGABRT, signal_handler);

#if !defined(GLIB_VERSION_2_36)
	g_type_init();
#endif
	g_test_init(&argc, &argv, NULL);

    // user_places algorithm tests
    g_test_add_func("/my-place/user_places/mac_stream", utc_user_places_mac_stream);
    g_test_add_func("/my-place/user_places/mac_compare", utc_user_places_mac_compare);
    g_test_add_func("/my-place/user_places/mac_set_ostream", utc_user_places_mac_set_ostream);
    g_test_add_func("/my-place/user_places/mac_set_istream", utc_user_places_mac_set_istream);
    g_test_add_func("/my-place/user_places/mac_set_from_string", utc_user_places_mac_set_from_string);
    g_test_add_func("/my-place/user_places/overlap1", utc_user_places_overlap1);
    g_test_add_func("/my-place/user_places/overlap2", utc_user_places_overlap2);
    g_test_add_func("/my-place/user_places/mac_set_insert", utc_user_places_mac_set_insert);
    g_test_add_func("/my-place/user_places/mac_set_find", utc_user_places_mac_set_find);
    g_test_add_func("/my-place/user_places/mac_sets_union", utc_user_places_mac_sets_union);
    g_test_add_func("/my-place/user_places/joint", utc_user_places_joint);
    g_test_add_func("/my-place/user_places/mahal_static_distance1", utc_user_places_mahal_static_distance1);
    g_test_add_func("/my-place/user_places/mahal_static_distance2", utc_user_places_mahal_static_distance2);
    g_test_add_func("/my-place/user_places/mahal_model_distance1", utc_user_places_mahal_model_distance1);
    g_test_add_func("/my-place/user_places/mahal_model_distance2", utc_user_places_mahal_model_distance2);
    g_test_add_func("/my-place/user_places/visit_categer_chi_approx", utc_user_places_visit_categer_chi_approx);
    g_test_add_func("/my-place/user_places/visit_categer_time_features", utc_user_places_visit_categer_time_features);
    g_test_add_func("/my-place/user_places/visit_categer_weeks_scope", utc_user_places_visit_categer_weeks_scope);
    g_test_add_func("/my-place/user_places/visit_categer_time_model_probability_features", utc_user_places_visit_categer_time_model_probability_features);
    g_test_add_func("/my-place/user_places/visit_categer_interval_features", utc_user_places_visit_categer_interval_features);
    g_test_add_func("/my-place/user_places/visit_categer", utc_user_places_visit_categer);
    g_test_add_func("/my-place/user_places/place_categer_median1", utc_user_places_place_categer_median1);
    g_test_add_func("/my-place/user_places/place_categer_median2", utc_user_places_place_categer_median2);
    g_test_add_func("/my-place/user_places/place_categer_categ_id_to_name", utc_user_places_place_categer_categ_id_to_name);
    g_test_add_func("/my-place/user_places/place_categer_visits_outliers_reduction", utc_user_places_place_categer_visits_outliers_reduction);
    g_test_add_func("/my-place/user_places/place_categer", utc_user_places_place_categer);
    g_test_add_func("/my-place/user_places/places_detector_merge_location1", utc_user_places_places_detector_merge_location1);
    g_test_add_func("/my-place/user_places/places_detector_merge_location2", utc_user_places_places_detector_merge_location2);
    g_test_add_func("/my-place/user_places/places_detector_visits_outliers_reduction", utc_user_places_places_detector_visits_outliers_reduction);
    g_test_add_func("/my-place/user_places/places_detector_merged_outliers_reduction", utc_user_places_places_detector_merged_outliers_reduction);

    // TODO: Fix tests
    //g_test_add_func("/my-place/user_places/compose_json_test", utc_user_places_compose_json_test);

    g_test_add_func("/my-place/user_places/automated_test", utc_user_places_automated_test);

	return g_test_run();
}
