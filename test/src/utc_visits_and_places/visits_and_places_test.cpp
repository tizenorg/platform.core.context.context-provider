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

#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <numeric>
#include <algorithm>
#include <glib.h>
#include "my-place/user_places/visit_detector.h"
#include "my-place/user_places/visit_categer.h"
#include "my-place/user_places/places_detector.h"
#include "my-place/user_places/place_categer.h"
#include "my-place/user_places/similarity.h"
#include "my-place/user_places/graph.h"
#include "my-place/user_places/wifi_listener_iface.h"
#include "my-place/user_places/user_places_types.h"
#include "my-place/user_places/mahal.h"
#include "my-place/user_places/median.h"
#include "my-place/place_recognition_types.h"
#include <unistd.h>
#include <Types.h>
#include "my-place/user_places/user_places.h"
#include "my-place/user_places/user_places_types.h"
#include "visits_and_places_test.h"

void ctx::Test::utc_user_places_mac_stream()
{
	std::string mac_str = "12:34:56:78:9a:bc";
	ctx::Mac mac;
	std::stringstream ss(mac_str);
	ss >> mac;
	std::string result_str;
	std::stringstream buf;
	buf << mac;
	buf >> result_str;
	g_assert(mac_str == result_str);
}

void ctx::Test::utc_user_places_mac_compare()
{
	ctx::Mac m1("aa:bb:cc:dd:ee:ff");
	ctx::Mac m2("Aa:bb:cc:dd:ee:fF");
	ctx::Mac m3("1a:bb:cc:dd:ee:fA");

	g_assert(m1 == m2);
	g_assert(m1 == m2);
	g_assert(m2 != m3);
	g_assert(m2 > m3);
	g_assert(m3 < m2);
	g_assert(!(m2 < m2));
	g_assert(!(m2 > m2));
}

void ctx::Test::utc_user_places_mac_set_ostream()
{
	ctx::MacSet mac_set;
	mac_set.insert(ctx::Mac("10:34:56:78:9a:bc"));
	mac_set.insert(ctx::Mac("12:34:56:78:9a:ba"));

	std::stringstream ss;
	ss << mac_set;

	g_assert(ss.str() == "10:34:56:78:9a:bc,12:34:56:78:9a:ba");
}

void ctx::Test::utc_user_places_mac_set_istream()
{
	std::stringstream ss;
	ss << "10:34:56:78:9a:bc,12:34:56:78:9a:basomething";

	ctx::MacSet mac_set;
	ss >> mac_set;

	std::string s_remainder;
	ss >> s_remainder;

	g_assert(mac_set.size() == 2);
	g_assert(mac_set.find(ctx::Mac("10:34:56:78:9a:bc")) != mac_set.end());
	g_assert(mac_set.find(ctx::Mac("12:34:56:78:9a:ba")) != mac_set.end());
	g_assert(mac_set.find(ctx::Mac("aa:aa:aa:aa:aa:aa")) == mac_set.end());
	g_assert(s_remainder == "something");
}

void ctx::Test::utc_user_places_mac_set_from_string()
{
	std::string str("10:34:56:78:9a:bc,12:34:56:78:9a:ba");

	ctx::MacSet mac_set = ctx::macSetFromString(str);

	g_assert(mac_set.size() == 2);
	g_assert(mac_set.find(ctx::Mac("10:34:56:78:9a:bc")) != mac_set.end());
	g_assert(mac_set.find(ctx::Mac("12:34:56:78:9a:ba")) != mac_set.end());
	g_assert(mac_set.find(ctx::Mac("aa:aa:aa:aa:aa:aa")) == mac_set.end());
}

void ctx::Test::utc_user_places_overlap1()
{
	ctx::MacSet s1;
	s1.insert(ctx::Mac("10:34:56:78:9a:bc"));
	s1.insert(ctx::Mac("12:34:56:78:9a:ba"));
	s1.insert(ctx::Mac("11:34:53:78:9a:ba"));

	ctx::MacSet s2;
	s2.insert(ctx::Mac("12:34:56:78:9a:ba"));
	s2.insert(ctx::Mac("15:34:56:78:9a:ba"));

	g_assert(ctx::similarity::overlapBiggerOverSmaller(s1,s2) == 0.5);
	g_assert(ctx::similarity::overlapBiggerOverSmaller(s2,s1) == 0.5);
}

void ctx::Test::utc_user_places_overlap2()
{
	ctx::MacSet s1;
	s1.insert(ctx::Mac("10:34:56:78:9a:bc"));
	s1.insert(ctx::Mac("12:34:56:78:9a:ba"));
	s1.insert(ctx::Mac("a2:34:56:78:9a:ba"));
	s1.insert(ctx::Mac("b5:34:56:78:9a:ba"));

	ctx::MacSet s2;
	s2.insert(ctx::Mac("13:34:56:78:9a:ba"));
	s2.insert(ctx::Mac("12:34:56:78:9a:ba"));
	s2.insert(ctx::Mac("13:34:56:78:9a:ba"));
	s2.insert(ctx::Mac("14:34:56:78:9a:ba"));
	s2.insert(ctx::Mac("15:34:56:78:9a:ba"));

	g_assert(ctx::similarity::overlapBiggerOverSmaller(s1,s2) == 0.25);
	g_assert(ctx::similarity::overlapBiggerOverSmaller(s2,s1) == 0.25);
}

void ctx::Test::utc_user_places_mac_set_insert()
{
	ctx::MacSet s1;
	s1.insert(ctx::Mac("10:34:56:78:9a:bc"));
	s1.insert(ctx::Mac("12:34:56:78:9a:ba"));
	s1.insert(ctx::Mac("10:34:56:78:9a:bc")); // the same as the fist inserted
	g_assert(s1.size() == 2);
}

void ctx::Test::utc_user_places_mac_set_find()
{
	ctx::Mac m1("10:34:56:78:9a:bc");
	ctx::Mac m2("10:34:56:78:9a:bc");
	m1 == m2;

	ctx::MacSet s;
	s.insert(ctx::Mac("10:34:56:78:9a:bc"));
	s.insert(ctx::Mac("11:34:56:78:9a:bc"));
	g_assert(s.find(ctx::Mac("11:34:56:78:9a:BC")) != s.end());
	g_assert(s.find(ctx::Mac("12:34:56:78:9a:BC")) == s.end());
}

void ctx::Test::utc_user_places_mac_sets_union()
{
	std::shared_ptr<ctx::MacSet> s1 = std::make_shared<ctx::MacSet>();
	s1->insert(ctx::Mac("10:34:56:78:9a:bc"));
	s1->insert(ctx::Mac("11:34:56:78:9a:bc"));
	s1->insert(ctx::Mac("12:34:56:78:9a:bc"));

	std::shared_ptr<ctx::MacSet> s2 = std::make_shared<ctx::MacSet>();
	s2->insert(ctx::Mac("11:34:56:78:9a:bc"));
	s2->insert(ctx::Mac("12:34:56:78:9a:bc"));
	s2->insert(ctx::Mac("13:34:56:78:9a:bc"));
	s2->insert(ctx::Mac("14:34:56:78:9a:bc"));

	std::vector<std::shared_ptr<ctx::MacSet>> components;
	components.push_back(s1);
	components.push_back(s2);

	std::shared_ptr<ctx::MacSet> union_set = ctx::macSetsUnion(components);

	g_assert(union_set->find(ctx::Mac("13:34:56:78:9a:BC")) != union_set->end());
	g_assert(union_set->find(ctx::Mac("14:34:56:78:9a:BC")) != union_set->end());
	g_assert(union_set->size() == 5);
}

void ctx::Test::utc_user_places_joint()
{
	ctx::MacSet s1;
	s1.insert(ctx::Mac("10:34:56:78:9a:bc"));
	s1.insert(ctx::Mac("12:34:56:78:9a:ba"));
	s1.insert(ctx::Mac("a2:34:56:78:9a:ba"));
	s1.insert(ctx::Mac("b5:34:56:78:9a:ba"));

	ctx::MacSet s2;
	s2.insert(ctx::Mac("13:34:56:78:9a:ba"));
	s2.insert(ctx::Mac("12:34:56:78:9a:ba"));
	s2.insert(ctx::Mac("13:34:56:78:9a:ba"));
	s2.insert(ctx::Mac("14:34:56:78:9a:ba"));
	s2.insert(ctx::Mac("15:34:56:78:9a:ba"));

	ctx::MacSet s3;
	s2.insert(ctx::Mac("73:34:56:78:9a:ba"));
	s2.insert(ctx::Mac("72:34:56:78:9a:ba"));
	s2.insert(ctx::Mac("74:34:56:78:9a:ba"));

	g_assert(ctx::similarity::isJoint(s1,s2));
	g_assert(ctx::similarity::isJoint(s2,s1));
	g_assert(!ctx::similarity::isJoint(s1,s3));
	g_assert(!ctx::similarity::isJoint(s2,s3));
}

void ctx::Test::utc_user_places_mahal_static_distance1()
{
	std::vector<ctx::num_t> v1 = {1, 5, 3};
	std::vector<ctx::num_t> v2 = {1, 2, -1};
	std::vector<ctx::num_t> m = {1, 0, 0, 0, 1, 0, 0, 0, 1}; // identity matrix

	int dist = ctx::MahalModel::distance(v1, v2, m);

	g_assert(UserPlacesTestUtils::equals_tol(dist, 5));
}

void ctx::Test::utc_user_places_mahal_static_distance2()
{
	std::vector<ctx::num_t> v1 = {410.0, 400.0};
	std::vector<ctx::num_t> v2 = {500.0, 500.0};
	std::vector<ctx::num_t> m = {0.00025, -0.00015, -0.00015, 0.00025};

	double dist = ctx::MahalModel::distance(v1, v2, m);

	g_assert(UserPlacesTestUtils::equals_tol(dist, 1.3509256));
}

void ctx::Test::utc_user_places_mahal_model_distance1()
{
	ctx::MahalModel model({1, 5, 3}, {1, 0, 0, 0, 1, 0, 0, 0, 1});
	int dist = model.distance({1, 2, -1});

	g_assert(UserPlacesTestUtils::equals_tol(dist, 5));
}

void ctx::Test::utc_user_places_mahal_model_distance2()
{
	ctx::MahalModel model({500.0, 500.0}, {0.00025, -0.00015, -0.00015, 0.00025});
	double dist = model.distance({410.0, 400.0});

	//g_assert(dist == 1.825); // fails because of round-off error, so:
	//g_assert(equals_tol(dist, 1.825));
	g_assert(UserPlacesTestUtils::equals_tol(dist, 1.3509256));
}

void ctx::Test::utc_user_places_visit_categer_chi_approx()
{
	const ctx::num_t tolerance = 0.00001;
	g_assert(UserPlacesTestUtils::equals_tol(ctx::VisitCateger::__chiApprox.value(0), 0.0));
	g_assert(UserPlacesTestUtils::equals_tol(ctx::VisitCateger::__chiApprox.value(0.3333), 0.000003, tolerance));
	g_assert(UserPlacesTestUtils::equals_tol(ctx::VisitCateger::__chiApprox.value(0.8888), 0.002455, tolerance));
	g_assert(UserPlacesTestUtils::equals_tol(ctx::VisitCateger::__chiApprox.value(1.5555), 0.06697, tolerance));
	g_assert(UserPlacesTestUtils::equals_tol(ctx::VisitCateger::__chiApprox.value(2.9999), 0.747301, tolerance));
	g_assert(UserPlacesTestUtils::equals_tol(ctx::VisitCateger::__chiApprox.value(3.1111), 0.792491, tolerance));
	g_assert(UserPlacesTestUtils::equals_tol(ctx::VisitCateger::__chiApprox.value(4.6666), 0.997225, tolerance));
	g_assert(UserPlacesTestUtils::equals_tol(ctx::VisitCateger::__chiApprox.value(5.7777), 0.999978, tolerance));
}

void ctx::Test::utc_user_places_visit_categer_time_features()
{
	const char* tz_env = getenv("TZ");
	setenv("TZ", "Europe/Warsaw", 1);

	auto tf1 = ctx::VisitCateger::timeFeatures(1424445450);
	auto tf2 = ctx::VisitCateger::timeFeatures(1424517450);
	//  Mon, 14 Sep 2015 00:00:00
	auto tf3 = ctx::VisitCateger::timeFeatures(1442181600);

	g_assert(tf1.minutesSinceMidnight == 977);
	g_assert(tf1.minutesSinceBeginingOfTheWeek == 6737);
	g_assert(tf1.weekday == 4);
	g_assert(tf1.weekend == false);
	g_assert(tf2.minutesSinceMidnight == 737);
	g_assert(tf2.minutesSinceBeginingOfTheWeek == 7937);
	g_assert(tf2.weekday == 5);
	g_assert(tf2.weekend == true);
	g_assert(tf3.minutesSinceMidnight == 0);
	g_assert(tf3.minutesSinceBeginingOfTheWeek == 0);
	g_assert(tf3.weekday == 0);
	g_assert(tf3.weekend == false);

	if (tz_env != NULL) {
		setenv("TZ", tz_env, 1);
	} else {
		unsetenv("TZ");
	}
}

void ctx::Test::utc_user_places_visit_categer_weeks_scope()
{
	const char* tz_env = getenv("TZ");
	setenv("TZ", "GMT", 1);

	// [Thu, 01 Jan 1970 00:00:00 GMT -> Thu, 01 Jan 1970 00:00:00 GMT]
	ctx::Interval visit0 = ctx::Interval(0, 0);
	auto tfs0 = ctx::VisitCateger::timeFeatures(0);
	int ws0 = ctx::VisitCateger::weeksScope(tfs0, visit0);

	// [Mon, 14 Sep 2015 00:00:00 GMT -> Mon, 14 Sep 2015 08:00:00 GMT]
	ctx::Interval visit1 = ctx::Interval(1442188800, 1442217600);
	auto tfs1 = ctx::VisitCateger::timeFeatures(1442188800);
	int ws1 = ctx::VisitCateger::weeksScope(tfs1, visit1);

	// [Sun, 13 Sep 2015 08:00:00 GMT -> Mon, 14 Sep 2015 00:00:00 GMT]
	ctx::Interval visit2 = ctx::Interval(1442131200, 1442188800);
	auto tfs2 = ctx::VisitCateger::timeFeatures(1442131200);
	int ws2 = ctx::VisitCateger::weeksScope(tfs2, visit2);

	// [Sun, 13 Sep 2015 08:00:00 GMT -> Mon, 14 Sep 2015 08:00:00 GMT]
	ctx::Interval visit3 = ctx::Interval(1442131200, 1442217600);
	auto tfs3 = ctx::VisitCateger::timeFeatures(1442131200);
	int ws3 = ctx::VisitCateger::weeksScope(tfs3, visit3);

	// [Sun, 13 Sep 2015 08:00:00 GMT -> Sun, 13 Sep 2015 23:59:30 GMT]
	ctx::Interval visit4 = ctx::Interval(1442131200, 1442188770);
	auto tfs4 = ctx::VisitCateger::timeFeatures(1442131200);
	int ws4 = ctx::VisitCateger::weeksScope(tfs4, visit4);

	// [Thu, 03 Sep 2015 12:00:00 GMT -> Thu, 17 Sep 2015 12:00:00 GMT]
	ctx::Interval visit5 = ctx::Interval(1441281600, 1442491200);
	auto tfs5 = ctx::VisitCateger::timeFeatures(1441281600);
	int ws5 = ctx::VisitCateger::weeksScope(tfs5, visit5);

	// [Mon, 07 Sep 2015 00:00:00 GMT -> Mon, 14 Sep 2015 00:00:00 GMT]
	ctx::Interval visit6 = ctx::Interval(1441584000, 1442188800);
	auto tfs6 = ctx::VisitCateger::timeFeatures(1441584000);
	int ws6 = ctx::VisitCateger::weeksScope(tfs6, visit6);

	g_assert(ws0 == 1);
	g_assert(ws1 == 1);
	g_assert(ws2 == 1);
	g_assert(ws3 == 2);
	g_assert(ws4 == 1);
	g_assert(ws5 == 3);
	g_assert(ws6 == 1);

	if (tz_env != NULL) {
		setenv("TZ", tz_env, 1);
	} else {
		unsetenv("TZ");
	}
}

void ctx::Test::utc_user_places_visit_categer_time_model_probability_features()
{
	const char* tz_env = getenv("TZ");
	setenv("TZ", "Europe/Warsaw", 1);

	// Visit 1 - short visit
	// [Mon, 01 Jun 2015 10:00:00 GMT -> Mon, 01 Jun 2015 11:00:00 GMT]
	ctx::Interval visit1 = ctx::Interval(1433152800, 1433156400);
	auto start_f1 = ctx::VisitCateger::timeFeatures(visit1.start);
	auto end_f1 = ctx::VisitCateger::timeFeatures(visit1.end);
	auto week_f1 = ctx::VisitCateger::weekModelFeatures(visit1, start_f1, end_f1);

	// Visit 2 - long visit (over one day)
	// [Tue, 02 Jun 2015 07:00:00 GMT -> Wed, 03 Jun 2015 12:00:00 GMT]
	ctx::Interval visit2 = ctx::Interval(1433228400, 1433332800);
	auto start_f2 = ctx::VisitCateger::timeFeatures(visit2.start);
	auto end_f2 = ctx::VisitCateger::timeFeatures(visit2.end);
	auto week_f2 = ctx::VisitCateger::weekModelFeatures(visit2, start_f2, end_f2);

	// Visit 3 - very long visit -> 4 days
	// [Wed, 03 Jun 2015 07:00:00 GMT -> Sun, 07 Jun 2015 12:00:00 GMT]
	ctx::Interval visit3 = ctx::Interval(1433314800, 1433678400);
	auto start_f3 = ctx::VisitCateger::timeFeatures(visit3.start);
	auto end_f3 = ctx::VisitCateger::timeFeatures(visit3.end);
	auto week_f3 = ctx::VisitCateger::weekModelFeatures(visit3, start_f3, end_f3);

	// Visit 4 - standard work visit
	// [Mon, 08 Jun 2015 07:00:00 GMT -> Mon, 08 Jun 2015 15:00:00 GMT]
	ctx::Interval visit4 = ctx::Interval(1433746800, 1433775600);
	auto start_f4 = ctx::VisitCateger::timeFeatures(visit4.start);
	auto end_f4 = ctx::VisitCateger::timeFeatures(visit4.end);
	auto week_f4 = ctx::VisitCateger::weekModelFeatures(visit4, start_f4, end_f4);

	// Visit 5 - standardowy home visit
	// [Mon, 08 Jun 2015 17:00:00 GMT -> Tue, 09 Jun 2015 07:00:00 GMT]
	ctx::Interval visit5 = ctx::Interval(1433782800, 1433833200);
	auto start_f5 = ctx::VisitCateger::timeFeatures(visit5.start);
	auto end_f5 = ctx::VisitCateger::timeFeatures(visit5.end);
	auto week_f5 = ctx::VisitCateger::weekModelFeatures(visit5, start_f5, end_f5);

	// Visit 6 - standard weekend (friday -> sunday)
	// [Fri, 12 Jun 2015 17:00:00 GMT -> Sun, 14 Jun 2015 10:00:00 GMT]
	ctx::Interval visit6 = ctx::Interval(1434128400, 1434276000);
	auto start_f6 = ctx::VisitCateger::timeFeatures(visit6.start);
	auto end_f6 = ctx::VisitCateger::timeFeatures(visit6.end);
	auto week_f6 = ctx::VisitCateger::weekModelFeatures(visit6, start_f6, end_f6);

	// Visit 7 - 1 week long (2 weeks scope)
	// [Fri, 12 Jun 2015 17:00:00 GMT -> Fri, 19 Jun 2015 10:00:00 GMT]
	ctx::Interval visit7 = ctx::Interval(1434128400, 1434708000);
	auto start_f7 = ctx::VisitCateger::timeFeatures(visit7.start);
	auto end_f7 = ctx::VisitCateger::timeFeatures(visit7.end);
	auto week_f7 = ctx::VisitCateger::weekModelFeatures(visit7, start_f7, end_f7);

	/*
	g_print("\n week_f1[HOME](%.12f) == %.12f", week_f1[PLACE_CATEG_ID_HOME], 0.073803994609);
	g_print("\n week_f1[WORK](%.12f) == %.12f", week_f1[PLACE_CATEG_ID_WORK], 0.867184674804);
	g_print("\n week_f1[OTHER](%.12f) == %.12f", week_f1[PLACE_CATEG_ID_OTHER], 0.059011330587);
	g_print("\n week_f2[HOME](%.12f) == %.12f", week_f2[PLACE_CATEG_ID_HOME], 0.498090672846);
	g_print("\n week_f2[WORK](%.12f) == %.12f", week_f2[PLACE_CATEG_ID_WORK], 0.453803101069);
	g_print("\n week_f2[OTHER](%.12f) == %.12f", week_f2[PLACE_CATEG_ID_OTHER], 0.048106226085);
	g_print("\n week_f3[HOME](%.12f) == %.12f", week_f3[PLACE_CATEG_ID_HOME], 0.605894740106);
	g_print("\n week_f3[WORK](%.12f) == %.12f", week_f3[PLACE_CATEG_ID_WORK], 0.245746540362);
	g_print("\n week_f3[OTHER](%.12f) == %.12f", week_f3[PLACE_CATEG_ID_OTHER], 0.148358719531);
	g_print("\n week_f4[HOME](%.12f) == %.12f", week_f4[PLACE_CATEG_ID_HOME], 0.118911740500);
	g_print("\n week_f4[WORK](%.12f) == %.12f", week_f4[PLACE_CATEG_ID_WORK], 0.819522091278);
	g_print("\n week_f4[OTHER](%.12f) == %.12f", week_f4[PLACE_CATEG_ID_OTHER], 0.061566168222);
	g_print("\n week_f5[HOME](%.12f) == %.12f", week_f5[PLACE_CATEG_ID_HOME], 0.893970559507);
	g_print("\n week_f5[WORK](%.12f) == %.12f", week_f5[PLACE_CATEG_ID_WORK], 0.068939539807);
	g_print("\n week_f5[OTHER](%.12f) == %.12f", week_f5[PLACE_CATEG_ID_OTHER], 0.037089900685);
	g_print("\n week_f6[HOME](%.12f) == %.12f", week_f6[PLACE_CATEG_ID_HOME], 0.772451164060);
	g_print("\n week_f6[WORK](%.12f) == %.12f", week_f6[PLACE_CATEG_ID_WORK], 0.006292949013);
	g_print("\n week_f6[OTHER](%.12f) == %.12f", week_f6[PLACE_CATEG_ID_OTHER], 0.221255886926);
	g_print("\n week_f7[HOME](%.12f) == %.12f", week_f7[PLACE_CATEG_ID_HOME], 0.647201987285);
	g_print("\n week_f7[WORK](%.12f) == %.12f", week_f7[PLACE_CATEG_ID_WORK], 0.231020030179);
	g_print("\n week_f7[OTHER](%.12f) == %.12f", week_f7[PLACE_CATEG_ID_OTHER], 0.121777982536);
	g_print("\n");
	*/

	const ctx::num_t tolerance = 0.000000000001;
	g_assert(UserPlacesTestUtils::equals_tol(week_f1[PLACE_CATEG_ID_HOME], 0.073803994609, tolerance));
	g_assert(UserPlacesTestUtils::equals_tol(week_f1[PLACE_CATEG_ID_WORK], 0.867184674804, tolerance));
	g_assert(UserPlacesTestUtils::equals_tol(week_f1[PLACE_CATEG_ID_OTHER], 0.059011330587, tolerance));
	g_assert(UserPlacesTestUtils::equals_tol(week_f2[PLACE_CATEG_ID_HOME], 0.498090672846, tolerance));
	g_assert(UserPlacesTestUtils::equals_tol(week_f2[PLACE_CATEG_ID_WORK], 0.453803101069, tolerance));
	g_assert(UserPlacesTestUtils::equals_tol(week_f2[PLACE_CATEG_ID_OTHER], 0.048106226085, tolerance));
	g_assert(UserPlacesTestUtils::equals_tol(week_f3[PLACE_CATEG_ID_HOME], 0.605894740106, tolerance));
	g_assert(UserPlacesTestUtils::equals_tol(week_f3[PLACE_CATEG_ID_WORK], 0.245746540362, tolerance));
	g_assert(UserPlacesTestUtils::equals_tol(week_f3[PLACE_CATEG_ID_OTHER], 0.148358719531, tolerance));
	g_assert(UserPlacesTestUtils::equals_tol(week_f4[PLACE_CATEG_ID_HOME], 0.118911740500, tolerance));
	g_assert(UserPlacesTestUtils::equals_tol(week_f4[PLACE_CATEG_ID_WORK], 0.819522091278, tolerance));
	g_assert(UserPlacesTestUtils::equals_tol(week_f4[PLACE_CATEG_ID_OTHER], 0.061566168222, tolerance));
	g_assert(UserPlacesTestUtils::equals_tol(week_f5[PLACE_CATEG_ID_HOME], 0.893970559507, tolerance));
	g_assert(UserPlacesTestUtils::equals_tol(week_f5[PLACE_CATEG_ID_WORK], 0.068939539807, tolerance));
	g_assert(UserPlacesTestUtils::equals_tol(week_f5[PLACE_CATEG_ID_OTHER], 0.037089900685, tolerance));
	g_assert(UserPlacesTestUtils::equals_tol(week_f6[PLACE_CATEG_ID_HOME], 0.772451164060, tolerance));
	g_assert(UserPlacesTestUtils::equals_tol(week_f6[PLACE_CATEG_ID_WORK], 0.006292949013, tolerance));
	g_assert(UserPlacesTestUtils::equals_tol(week_f6[PLACE_CATEG_ID_OTHER], 0.221255886926, tolerance));
	g_assert(UserPlacesTestUtils::equals_tol(week_f7[PLACE_CATEG_ID_HOME], 0.647201987285, tolerance));
	g_assert(UserPlacesTestUtils::equals_tol(week_f7[PLACE_CATEG_ID_WORK], 0.231020030179, tolerance));
	g_assert(UserPlacesTestUtils::equals_tol(week_f7[PLACE_CATEG_ID_OTHER], 0.121777982536, tolerance));

	if (tz_env != NULL) {
		setenv("TZ", tz_env, 1);
	} else {
		unsetenv("TZ");
	}
}

void ctx::Test::utc_user_places_visit_categer_interval_features()
{
	const char* tz_env = getenv("TZ");
	setenv("TZ", "Europe/Warsaw", 1);
	auto v = ctx::VisitCateger::intervalFeatures(ctx::Interval(1424445450, 1424448264));
	g_assert(UserPlacesTestUtils::equals_tol(v[0], 46.9));
	g_assert(UserPlacesTestUtils::equals_tol(v[1], 977));
	g_assert(UserPlacesTestUtils::equals_tol(v[2], 1024));
	g_assert(UserPlacesTestUtils::equals_tol(v[3], 4));
	const ctx::num_t tolerance = 0.02;
	g_assert(UserPlacesTestUtils::equals_tol(v[4], 0.185766275162, tolerance));
	g_assert(UserPlacesTestUtils::equals_tol(v[5], 0.727713045493, tolerance));
	g_assert(UserPlacesTestUtils::equals_tol(v[6], 0.086520679346, tolerance));

	if (tz_env != NULL) {
		setenv("TZ", tz_env, 1);
	} else {
		unsetenv("TZ");
	}
}

void ctx::Test::utc_user_places_visit_categer()
{
	const char* tz_env = getenv("TZ");
	setenv("TZ", "Europe/Warsaw", 1);

	ctx::Interval interval(1424445433, 1424448313);
	ctx::Visit visit(interval);
	ctx::VisitCateger::categorize(visit);
	g_assert(visit.categs.size() == 3);

	g_assert(visit.categs.find(PLACE_CATEG_ID_HOME) != visit.categs.end());
	g_assert(visit.categs.find(PLACE_CATEG_ID_WORK) != visit.categs.end());
	g_assert(visit.categs.find(PLACE_CATEG_ID_OTHER) != visit.categs.end());
	g_assert(visit.categs.find(PLACE_CATEG_ID_NONE) == visit.categs.end());

	ctx::num_t p_home = visit.categs.find(PLACE_CATEG_ID_HOME)->second;
	ctx::num_t p_work = visit.categs.find(PLACE_CATEG_ID_WORK)->second;
	ctx::num_t p_other = visit.categs.find(PLACE_CATEG_ID_OTHER)->second;

	/*
	g_print("\n p_home(%.12f) == %.12f", p_home, 0.097576751286);
	g_print("\n p_work(%.12f) == %.12f", p_work, 0.490992039238);
	g_print("\n p_other(%.12f) == %.12f", p_other, 0.478752308325);
	*/
	const ctx::num_t tolerance = 0.00001;
	g_assert(UserPlacesTestUtils::equals_tol(p_home, 0.097576751286, tolerance));
	g_assert(UserPlacesTestUtils::equals_tol(p_work, 0.490992039238, tolerance));
	g_assert(UserPlacesTestUtils::equals_tol(p_other, 0.478752308325, tolerance));


	if (tz_env != NULL) {
		setenv("TZ", tz_env, 1);
	} else {
		unsetenv("TZ");
	}
}

void ctx::Test::utc_user_places_place_categer_median1()
{
	std::vector<ctx::num_t> v = {6.6, 2.3, 7.7, 1.1, 8.88, 9.99, 5.5, 4.43, 3.3};
	g_assert(ctx::median(v) == 5.5);
}

void ctx::Test::utc_user_places_place_categer_median2()
{
	std::vector<ctx::num_t> v = {6.6, 10.1, 2.3, 7.7, 1.1, 8.88, 9.99, 5.5, 4.43, 3.3};
	g_assert(ctx::median(v) == 6.05);
}

void ctx::Test::utc_user_places_place_categer_categ_id_to_name()
{
	g_assert(ctx::PlaceCateger::categId2Name(PLACE_CATEG_ID_HOME) == "home");
	g_assert(ctx::PlaceCateger::categId2Name(PLACE_CATEG_ID_WORK) == "work");
	g_assert(ctx::PlaceCateger::categId2Name(PLACE_CATEG_ID_OTHER) == "other");
}

void ctx::Test::utc_user_places_place_categer_visits_outliers_reduction()
{
	ctx::Categs categs_correct1 = {
		{PLACE_CATEG_ID_HOME, 0.5},
		{PLACE_CATEG_ID_WORK, 0.4},
		{PLACE_CATEG_ID_OTHER, 0.2}
	};
	ctx::Categs categs_correct2 = {
		{PLACE_CATEG_ID_HOME, -1},
		{PLACE_CATEG_ID_WORK, 0.04},
		{PLACE_CATEG_ID_OTHER, 0.1}
	};
	ctx::Categs categs_incorrect1 = {
		{PLACE_CATEG_ID_HOME, 0},
		{PLACE_CATEG_ID_WORK, 0},
		{PLACE_CATEG_ID_OTHER, 0}
	};
	ctx::Categs categs_incorrect2 = {
		{PLACE_CATEG_ID_HOME, 0.05},
		{PLACE_CATEG_ID_WORK, 0.04},
		{PLACE_CATEG_ID_OTHER, 0.02}
	};

	ctx::Interval interval(0, 2*60*60); // 2h

	ctx::Visit visit1(interval, std::make_shared<ctx::MacSet>(), categs_correct1);
	ctx::Visit visit2(interval, std::make_shared<ctx::MacSet>(), categs_incorrect1);
	ctx::Visit visit3(interval, std::make_shared<ctx::MacSet>(), categs_correct2);
	ctx::Visit visit4(interval, std::make_shared<ctx::MacSet>(), categs_incorrect2);

	ctx::Visits visits = {visit1, visit2, visit3, visit4};

	ctx::PlaceCateger::reduceOutliers(visits);

	ctx::Visits expected = {visit1, visit3};

	g_assert(visits == expected);
}

void ctx::Test::utc_user_places_place_categer()
{
	ctx::Categs categs1 = {
		{PLACE_CATEG_ID_HOME, 0.5},
		{PLACE_CATEG_ID_WORK, 0.4},
		{PLACE_CATEG_ID_OTHER, 0.2}
	};
	ctx::Visit visit1(ctx::Interval(0, 0), std::make_shared<ctx::MacSet>(), categs1);

	ctx::Categs categs2 = {
		{PLACE_CATEG_ID_HOME, 0.2},
		{PLACE_CATEG_ID_WORK, 0.8},
		{PLACE_CATEG_ID_OTHER, 0.1}
	};
	ctx::Visit visit2(ctx::Interval(0, 0), std::make_shared<ctx::MacSet>(), categs2);

	ctx::Categs categs3 = {
		{PLACE_CATEG_ID_HOME, 0.4},
		{PLACE_CATEG_ID_WORK, 0.9},
	};
	ctx::Visit visit3(ctx::Interval(0, 0), std::make_shared<ctx::MacSet>(), categs3);

	ctx::Visits visits = {visit1, visit2, visit3};

	auto vec_home = ctx::PlaceCateger::categVectorFromVisits(visits, PLACE_CATEG_ID_HOME);
	auto vec_work = ctx::PlaceCateger::categVectorFromVisits(visits, PLACE_CATEG_ID_WORK);
	auto vec_other = ctx::PlaceCateger::categVectorFromVisits(visits, PLACE_CATEG_ID_OTHER);

	g_assert(vec_home == std::vector<ctx::num_t>({0.5, 0.2, 0.4}));
	g_assert(vec_work == std::vector<ctx::num_t>({0.4, 0.8, 0.9}));
	g_assert(vec_other == std::vector<ctx::num_t>({0.2, 0.1}));

	ctx::Place place;
	ctx::PlaceCateger::categorize(visits, place);

	g_assert(place.categId == PLACE_CATEG_ID_WORK);
	g_assert(UserPlacesTestUtils::equals_tol(place.categConfidence, 0.592593));
}

void ctx::Test::utc_user_places_places_detector_merge_location1()
{
	ctx::Visits visits;
	ctx::Interval interval(0,0);
	auto mac_set = std::make_shared<ctx::MacSet>();
	ctx::Visit visit1(interval, mac_set); visit1.setLocation(ctx::Location(10,10)); visits.push_back(visit1);
	ctx::Visit visit2(interval, mac_set); visit2.setLocation(ctx::Location(50,10)); visits.push_back(visit2);
	ctx::Visit visit3(interval, mac_set); visit3.setLocation(ctx::Location(25,20)); visits.push_back(visit3);
	ctx::Visit visit4(interval, mac_set); visit4.setLocation(ctx::Location(25,20)); visits.push_back(visit4);
	ctx::Visit visit5(interval, mac_set); visits.push_back(visit5);

	ctx::Place place;
	ctx::PlacesDetector::__mergeLocation(visits, place);

	g_assert(place.locationValid == true);
	g_assert(place.location.latitude == 25);
	g_assert(place.location.longitude == 15);
}

void ctx::Test::utc_user_places_places_detector_merge_location2()
{
	ctx::Visits visits;
	ctx::Interval interval(0,0);
	auto mac_set = std::make_shared<ctx::MacSet>();
	ctx::Visit visit1(interval, mac_set); visits.push_back(visit1);
	ctx::Visit visit2(interval, mac_set); visits.push_back(visit2);

	ctx::Place place;
	ctx::PlacesDetector::__mergeLocation(visits, place);

	g_assert(place.locationValid == false);
}

void ctx::Test::utc_user_places_places_detector_visits_outliers_reduction()
{
	ctx::Categs categs = {
		{PLACE_CATEG_ID_HOME, 0.5},
		{PLACE_CATEG_ID_WORK, 0.4},
		{PLACE_CATEG_ID_OTHER, 0.2}
	};

	ctx::Interval interval_to_short(0, 14*60); // <15m
	ctx::Interval interval_to_long(0, 6*24*60*60); // >5d
	ctx::Interval interval_ok1(0, 15*60); // 15m
	ctx::Interval interval_ok2(0, 5*24*60*60); // 5d
	ctx::Interval interval_ok3(0, 2*60*60); // 2h

	ctx::Visit visit1(interval_to_short, std::make_shared<ctx::MacSet>(), categs);
	ctx::Visit visit2(interval_to_long, std::make_shared<ctx::MacSet>(), categs);
	ctx::Visit visit3(interval_ok1, std::make_shared<ctx::MacSet>(), categs); // OK
	ctx::Visit visit4(interval_ok2, std::make_shared<ctx::MacSet>(), categs); // OK
	ctx::Visit visit5(interval_ok3, std::make_shared<ctx::MacSet>(), categs); // OK
	ctx::Visit visit6(interval_to_short, std::make_shared<ctx::MacSet>(), categs);
	ctx::Visit visit7(interval_to_long, std::make_shared<ctx::MacSet>(), categs);
	ctx::Visit visit8(interval_ok2, std::make_shared<ctx::MacSet>(), categs); // OK
	ctx::Visit visit9(interval_ok3, std::make_shared<ctx::MacSet>(), categs); // OK

	ctx::Visits visits = {visit1, visit2, visit3, visit4, visit5, visit6, visit7, visit8, visit9};

	ctx::PlacesDetector::reduceOutliers(visits);

	ctx::Visits expected = {visit3, visit4, visit5, visit8, visit9};
	g_assert(visits == expected);
}

void ctx::Test::utc_user_places_places_detector_merged_outliers_reduction()
{
	auto c1 = std::make_shared<ctx::graph::Component>(ctx::graph::Component({}));
	auto c2 = std::make_shared<ctx::graph::Component>(ctx::graph::Component({7})); // OK
	auto c3 = std::make_shared<ctx::graph::Component>(ctx::graph::Component({2, 3})); // OK
	auto c4 = std::make_shared<ctx::graph::Component>(ctx::graph::Component({4, 5, 6})); // OK
	auto c5 = std::make_shared<ctx::graph::Component>(ctx::graph::Component({8})); // OK
	auto c6 = std::make_shared<ctx::graph::Component>(ctx::graph::Component({9, 5, 6, 7})); // OK
	auto c7 = std::make_shared<ctx::graph::Component>(ctx::graph::Component({4, 5})); // OK
	auto ccs = std::make_shared<ctx::graph::Components>(ctx::graph::Components({c1, c2, c3, c4, c5, c6, c7}));

	ctx::PlacesDetector::reduceOutliers(ccs);

	auto expected_ccs = std::make_shared<ctx::graph::Components>(ctx::graph::Components({c2, c3, c4, c5, c6, c7}));

	g_assert(*ccs == *expected_ccs);
}

void ctx::Test::utc_user_places_compose_json_test()
{
	std::vector<std::shared_ptr<ctx::Place>> places;

	auto place1 = std::make_shared<ctx::Place>();
	place1->categId = PLACE_CATEG_ID_HOME;
	place1->categConfidence = .555;
	place1->name = "Home";
	place1->locationValid = true;
	place1->location.latitude = 10.9443311;
	place1->location.longitude = 50.8550422;
	place1->wifiAps = "00:1f:f3:5b:2b:1f,15:34:56:78:9a:ba,13:34:56:78:9a:ba";
	place1->createDate = time_t(12132567);
	places.push_back(place1);

	auto place2 = std::make_shared<ctx::Place>();
	place2->categId = PLACE_CATEG_ID_WORK;
	place2->categConfidence = 0.666;
	place2->name = "Work";
	place2->locationValid = true;
	place2->location.latitude = 10.9623333;
	place2->location.longitude = 50.8430444;
	place2->wifiAps = "12:34:56:78:9a:ba";
	place2->createDate = time_t(12132889);
	places.push_back(place2);

	ctx::Json output_json = ctx::UserPlaces::composeJson(places);
	std::string output = output_json.str();

	std::string desired_output = ""
	"{"
		"\"PlacesList\":["
			"{"
				"\"TypeId\":" + std::to_string(PLACE_CATEG_ID_HOME) + ","
				"\"TypeConfidence\":\"0.555\","
				"\"Name\":\"Home\","
				"\"GeoLatitude\":\"10.9443311\","
				"\"GeoLongitude\":\"50.8550422\","
				"\"WifiAPs\":\"00:1f:f3:5b:2b:1f,15:34:56:78:9a:ba,13:34:56:78:9a:ba\","
				"\"CreateDate\":12132567"
			"},"
			"{"
				"\"TypeId\":" + std::to_string(PLACE_CATEG_ID_WORK) + ","
				"\"TypeConfidence\":\"0.666\","
				"\"Name\":\"Work\","
				"\"GeoLatitude\":\"10.9623333\","
				"\"GeoLongitude\":\"50.8430444\","
				"\"WifiAPs\":\"12:34:56:78:9a:ba\","
				"\"CreateDate\":12132889"
			"}"
		"]"
	"}";

	/*
	g_print("\n----------------------------------------------------------------------\n");
	g_print("JSON string comparison:\n");
	g_print("-> output:\n");
	g_print("----------------------------------------------------------------------\n");
	g_print("%s\n", output.c_str());
	g_print("----------------------------------------------------------------------\n");
	g_print("-> desired_output:\n");
	g_print("----------------------------------------------------------------------\n");
	g_print("%s\n", desired_output.c_str());
	g_print("----------------------------------------------------------------------\n");
	*/

	g_assert(output == desired_output);
}

void ctx::Test::test_input_wifi_expected_visits_and_places(std::string path)
{
	g_print("\n\t\ttests_input_wifi_expected_visits_and_places");
	// testing visit detector
	auto events = UserPlacesTestUtils::events_from_file(path + TEST_FILE_INPUT_WIFI_NAME);
	auto visits = UserPlacesTestUtils::visits_from_events(events);
	std::stringstream ss_output_visits;
	UserPlacesTestUtils::print_visits(ss_output_visits, *visits);
	std::string expected_visits_from_file = UserPlacesTestUtils::read_file(path + TEST_FILE_EXPECTED_VISITS_NAME);
	g_assert(ss_output_visits.str().compare(expected_visits_from_file) == 0);

	// testing places detector
	ctx::PlacesDetector places_detector(true);
	std::stringstream ss_output_user_places;
	auto ccs = places_detector.__mergeVisits(*visits);
	UserPlacesTestUtils::print_components(ss_output_user_places, *ccs);
	std::string expected_places_from_file = UserPlacesTestUtils::read_file(path + TEST_FILE_EXPECTED_PLACES_NAME);

	g_assert(ss_output_user_places.str().compare(expected_places_from_file) == 0);
}

void ctx::Test::test_input_wifi_expected_places_categ_wifi(std::string path)
{
	g_print("\n\t\ttests_input_wifi_expected_places_categ_wifi");
	auto events = UserPlacesTestUtils::events_from_file(path + TEST_FILE_INPUT_WIFI_NAME);
	auto visits = UserPlacesTestUtils::visits_from_events(events);
	ctx::PlacesDetector::reduceOutliers(*visits);
//	UserPlacesTestUtils::print_visits(*visits); // TODO remove
	auto places = UserPlacesTestUtils::places_from_visits(visits);
	std::vector<std::string> places_str;
	for (auto& place : places) {
		places_str.push_back(UserPlacesTestUtils::place_str(*place) + "\n");
	}
	std::sort(places_str.begin(), places_str.end());
	std::string comb_places = std::accumulate(places_str.begin(), places_str.end(), std::string(""));
	std::string exp_comb_places = UserPlacesTestUtils::read_file(path + TEST_FILE_EXPECTED_PLACES_CATEG_WIFI_NAME);
//	std::cout << std::endl << "places:" << std::endl << comb_places << std::endl << std::endl;  // TODO remove

	// Remove unnecessary characters from strings
	UserPlacesTestUtils::remove_occurences(comb_places, '\r');
	UserPlacesTestUtils::remove_occurences(exp_comb_places, '\r');
	UserPlacesTestUtils::remove_occurences(comb_places, '\n');
	UserPlacesTestUtils::remove_occurences(exp_comb_places, '\n');

	g_assert(comb_places == exp_comb_places);
}

void ctx::Test::run_automated_test(std::string folder_path, test_type_e test)
{
	switch (test) {
	case TEST_INPUT_WIFI_EXPECTED_VISITS:
		// XXX: Implement if needed
		break;
	case TEST_INPUT_WIFI_EXPECTED_VISITS_CATEGS:
		// XXX: Implement if needed
		break;
	case TEST_INPUT_WIFI_EXPECTED_PLACES:
		// XXX: Implement if needed
		break;
	case TEST_INPUT_WIFI_EXPECTED_PLACES_CATEGS:
		// XXX: Implement if needed
		break;
	case TEST_INPUT_VISITS_EXPECTED_VISITS_CATEGS:
		// XXX: Implement if needed
		break;
	case TEST_INPUT_VISITS_EXPECTED_PLACES:
		// XXX: Implement if needed
		break;
	case TEST_INPUT_VISITS_EXPECTED_PLACES_CATEGS:
		// XXX: Implement if needed
		break;
	case TEST_INPUT_PLACES_EXPECTED_PLACES_CATEGS:
		// XXX: Implement if needed
		break;
	case TEST_INPUT_WIFI_EXPECTED_VISITS_AND_PLACES:
		test_input_wifi_expected_visits_and_places(folder_path);
		break;
	case TEST_INPUT_WIFI_EXPECTED_PLACES_CATEG_WIFI:
		test_input_wifi_expected_places_categ_wifi(folder_path);
		break;
	default:
		break;
	}
}

void ctx::Test::utc_user_places_automated_test()
{
	std::vector<std::string> folders;
	UserPlacesTestUtils::find_subfolders_recursively(TEST_FOLDER_PATH, &folders);
	for (std::string folder : folders) {
		std::set<test_type_e> tests = UserPlacesTestUtils::find_tests(folder);
		g_print("\n\t%d tests in folder %s", tests.size(), folder.c_str());
		if (!tests.empty()) {
			const char* tz_env = getenv("TZ");
			std::string time_zone = UserPlacesTestUtils::read_time_zone(folder);
			setenv("TZ", time_zone.c_str(), 1);
			g_print(", time_zone = %s", time_zone.c_str());

			for (test_type_e test : tests) {
				run_automated_test(folder, test);
			}

			if (tz_env != NULL) {
				setenv("TZ", tz_env, 1);
			} else {
				unsetenv("TZ");
			}
		}
	}
	if (folders.empty()) {
		g_print("No folders found");
	}
	g_print("\n");
}

extern "C" void utc_user_places_mac_stream() {
	ctx::Test::utc_user_places_mac_stream();
}

extern "C" void utc_user_places_mac_compare() {
	ctx::Test::utc_user_places_mac_compare();
}

extern "C" void utc_user_places_mac_set_ostream() {
	ctx::Test::utc_user_places_mac_set_ostream();
}

extern "C" void utc_user_places_mac_set_istream() {
	ctx::Test::utc_user_places_mac_set_istream();
}

extern "C" void utc_user_places_mac_set_from_string() {
	ctx::Test::utc_user_places_mac_set_from_string();
}

extern "C" void utc_user_places_overlap1() {
	ctx::Test::utc_user_places_overlap1();
}

extern "C" void utc_user_places_overlap2() {
	ctx::Test::utc_user_places_overlap2();
}

extern "C" void utc_user_places_mac_set_insert() {
	ctx::Test::utc_user_places_mac_set_insert();
}

extern "C" void utc_user_places_mac_set_find() {
	ctx::Test::utc_user_places_mac_set_find();
}

extern "C" void utc_user_places_mac_sets_union() {
	ctx::Test::utc_user_places_mac_sets_union();
}

extern "C" void utc_user_places_joint() {
	ctx::Test::utc_user_places_joint();
}

extern "C" void utc_user_places_mahal_static_distance1() {
	ctx::Test::utc_user_places_mahal_static_distance1();
}

extern "C" void utc_user_places_mahal_static_distance2() {
	ctx::Test::utc_user_places_mahal_static_distance2();
}

extern "C" void utc_user_places_mahal_model_distance1() {
	ctx::Test::utc_user_places_mahal_model_distance1();
}

extern "C" void utc_user_places_mahal_model_distance2() {
	ctx::Test::utc_user_places_mahal_model_distance2();
}

extern "C" void utc_user_places_visit_categer_chi_approx() {
	ctx::Test::utc_user_places_visit_categer_chi_approx();
}

extern "C" void utc_user_places_visit_categer_time_features() {
	ctx::Test::utc_user_places_visit_categer_time_features();
}

extern "C" void utc_user_places_visit_categer_weeks_scope() {
	ctx::Test::utc_user_places_visit_categer_weeks_scope();
}

extern "C" void utc_user_places_visit_categer_time_model_probability_features() {
	ctx::Test::utc_user_places_visit_categer_time_model_probability_features();
}

extern "C" void utc_user_places_visit_categer_interval_features() {
	ctx::Test::utc_user_places_visit_categer_interval_features();
}

extern "C" void utc_user_places_visit_categer() {
	ctx::Test::utc_user_places_visit_categer();
}

extern "C" void utc_user_places_place_categer_median1() {
	ctx::Test::utc_user_places_place_categer_median1();
}

extern "C" void utc_user_places_place_categer_median2() {
	ctx::Test::utc_user_places_place_categer_median2();
}

extern "C" void utc_user_places_place_categer_categ_id_to_name() {
	ctx::Test::utc_user_places_place_categer_categ_id_to_name();
}

extern "C" void utc_user_places_place_categer_visits_outliers_reduction() {
	ctx::Test::utc_user_places_place_categer_visits_outliers_reduction();
}

extern "C" void utc_user_places_place_categer() {
	ctx::Test::utc_user_places_place_categer();
}

extern "C" void utc_user_places_places_detector_merge_location1() {
	ctx::Test::utc_user_places_places_detector_merge_location1();
}

extern "C" void utc_user_places_places_detector_merge_location2() {
	ctx::Test::utc_user_places_places_detector_merge_location2();
}

extern "C" void utc_user_places_places_detector_visits_outliers_reduction() {
	ctx::Test::utc_user_places_places_detector_visits_outliers_reduction();
}

extern "C" void utc_user_places_places_detector_merged_outliers_reduction() {
	ctx::Test::utc_user_places_places_detector_merged_outliers_reduction();
}

extern "C" void utc_user_places_compose_json_test() {
	ctx::Test::utc_user_places_compose_json_test();
}

extern "C" void utc_user_places_automated_test() {
	ctx::Test::utc_user_places_automated_test();
}
