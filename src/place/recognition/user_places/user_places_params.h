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

#ifndef __CONTEXT_PLACE_STATUS_USER_PLACES_PARAMS_H__
#define __CONTEXT_PLACE_STATUS_USER_PLACES_PARAMS_H__

/*
 * WiFi scanning frequency (in minutes) in PLACE_RECOG_HIGH_ACCURACY_MODE.
 */
#define WIFI_LOGGER_INTERVAL_MINUTES_HIGH_ACCURACY 3

/*
 * WiFi scanning frequency (in minutes) in PLACE_RECOG_LOW_POWER_MODE.
 */
#define WIFI_LOGGER_INTERVAL_MINUTES_LOW_POWER 60

/*
 * Time window taken into consideration (in seconds) in PLACE_RECOG_HIGH_ACCURACY_MODE.
 */
#define VISIT_DETECTOR_PERIOD_SECONDS_HIGH_ACCURACY 360

/*
 * Time window taken into consideration (in seconds) in PLACE_RECOG_LOW_POWER_MODE.
 */
#define VISIT_DETECTOR_PERIOD_SECONDS_LOW_POWER 3600

/*
 * Overlap threshold between two sets of mac addresses (overlap
 * coefficient for two sets should be higher than this threshold
 * in order to detect stable radio environment); =< 1.0
 * New parameter in algorithm compared to original version of PlaceSense!
 */
#define VISIT_DETECTOR_OVERLAP 0.8f

/*
 * Specifies how many stable intervals must be seen to
 * indicate an entrance to a place; >= 1
 */
#define VISIT_DETECTOR_STABLE_DEPTH 1

/*
 * Representatives threshold (representatnive beacon
 * response rate should be higher than this threshold); =< 1.0
 */
#define VISIT_DETECTOR_REP_THRESHOLD 0.9f

/*
 * Specifies how long scans must be unstable to indicate a leave form a place; >= 1
 */
#define VISIT_DETECTOR_TOLERANCE_DEPTH 3

#define PLACES_DETECTOR_TASK_START_HOUR 3
#define PLACES_DETECTOR_TASK_START_MINUTE 11
#define PLACES_DETECTOR_RETENTION_DAYS 30
#define PLACES_DETECTOR_RETENTION_SECONDS 24 * 60 * 60 * PLACES_DETECTOR_RETENTION_DAYS

/*
 * Number of digits after decimal point used in geo coordinates.
 */
#define GEO_LOCATION_PRECISION 7

/*
 * Minimal duration of visit (in minutes) taking into account for place detection
 */
#define PLACES_DETECTOR_MIN_VISIT_DURATION_MINUTES 15

/*
 * Maximum duration of visit (in minutes) taking into account for place detection
 */
#define PLACES_DETECTOR_MAX_VISIT_DURATION_MINUTES 5 * 24 * 60

/*
 * Minimum visits number per place
 */
#define PLACES_DETECTOR_MIN_VISITS_PER_PLACE 1

/*
 * Minimum visits number per big place
 */
#define PLACES_DETECTOR_MIN_VISITS_PER_BIG_PLACE 4

/*
 * Minimal visit category score for taking this visit into consideration during
 * place categorization
 */
#define PLACES_CATEGER_MIN_VISITS_SCORE 0.1

/*
 * Minimum visits number per home
 */
#define PLACES_CATEGER_MIN_VISITS_PER_HOME 3

/*
 * Minimum visits number per work
 */
#define PLACES_CATEGER_MIN_VISITS_PER_WORK 2

#endif /* __CONTEXT_PLACE_STATUS_USER_PLACES_PARAMS_H__ */
