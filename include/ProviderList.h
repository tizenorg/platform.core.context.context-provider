/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd.
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

#ifndef _CONTEXT_PROVIDER_LIST_H_
#define _CONTEXT_PROVIDER_LIST_H_

#include <ContextProvider.h>
#include <ProviderTypes.h>

#define LIB_DIRECTORY	"/usr/lib/context-service/"
#define LIB_PREFIX		"libctx-prvd-"
#define LIB_EXTENSION	".so"

const struct {
	const char *subject;
	const char *library;
} subjectLibraryList[] = {
	{SUBJ_ACTIVITY,			"activity"},
	{SUBJ_APP_STATS,		"app-stats"},
	{SUBJ_STATE_CALL,		"call"},
	{SUBJ_STATE_CONTACTS,	"contacts"},
	// {SUBJ_CUSTOM, ""},
	{SUBJ_STATE_EMAIL,		"email"},
	{SUBJ_PLACE_GEOFENCE,	"geofence"},
	{SUBJ_STATE_HEADPHONE,	"headphone"},
	{SUBJ_MEDIA_LOGGER,		"media-stats"},
	{SUBJ_MUSIC_STATS,		"media-stats"},
	{SUBJ_VIDEO_STATS,		"media-stats"},
	{SUBJ_STATE_MESSAGE,	"message"},
	{SUBJ_PLACE_DETECTION,	"my-place"},
	{SUBJ_SOCIAL_STATS,		"social-stats"},
	{SUBJ_STATE_BATTERY,	"system"},
	{SUBJ_STATE_CHARGER,	"system"},
	{SUBJ_STATE_USB,		"system"},
	{SUBJ_STATE_GPS,		"system"},
	{SUBJ_STATE_PSMODE,		"system"},
	{SUBJ_STATE_ALARM,		"time"},
	{SUBJ_STATE_TIME,		"time"},
	{SUBJ_STATE_WIFI,		"wifi"}
};

const struct {
	const char *subject;
	int operation;
	const char *attribute;
	const char *option;
} triggerTemplateList[] = {
	{
		SUBJ_ACTIVITY,
		OPS_SUBSCRIBE,
		"{\"Event\":{\"type\":\"string\", \"values\":[\"Detected\"]}}",
		"{\"Accuracy\":{\"type\":\"string\", \"values\":[\"Low\", \"Normal\", \"High\"]}}"
	},
	{
		SUBJ_APP_FREQUENCY,
		OPS_READ,
		"{" TRIG_DEF_RANK "," TRIG_DEF_TOTAL_COUNT "}",
		"{"
			"\"AppId\":{\"type\":\"string\"},"
			TRIG_DEF_TIME_OF_DAY "," TRIG_DEF_DAY_OF_WEEK
		"}"
	},
	{
		SUBJ_STATE_CALL,
		OPS_SUBSCRIBE | OPS_READ,
		/* TODO remove Connecting, Connected */
		"{"
			"\"Medium\":{\"type\":\"string\",\"values\":[\"Voice\",\"Video\"]},"
			"\"State\":{\"type\":\"string\",\"values\":[\"Idle\",\"Connecting\",\"Connected\"]},"
			"\"Address\":{\"type\":\"string\"}"
		"}",
		NULL
	},
	{
		SUBJ_STATE_CONTACTS,
		OPS_SUBSCRIBE,
		"{"
			"\"Event\":{\"type\":\"string\",\"values\":[\"Changed\"]},"
			"\"Type\":{\"type\":\"string\",\"values\":[\"MyProfile\",\"Person\"]}"
		"}",
		NULL
	},
	{
		SUBJ_STATE_EMAIL,
		OPS_SUBSCRIBE,
		"{"
			"\"Event\":{\"type\":\"string\",\"values\":[\"Received\",\"Sent\"]}"
		"}",
		NULL
	},
	{
		SUBJ_PLACE_GEOFENCE,
		OPS_SUBSCRIBE,
		"{"
			"\"Event\":{\"type\":\"string\",\"values\":[\"In\",\"Out\"]}"
		"}",
		"{"
			"\"PlaceId\":{\"type\":\"integer\",\"min\":1}"
		"}"
	},
	{
		SUBJ_STATE_HEADPHONE,
		OPS_SUBSCRIBE | OPS_READ,
		"{"
			TRIG_BOOL_ITEM_DEF("IsConnected") ","
			"\"Type\":{\"type\":\"string\",\"values\":[\"Normal\",\"Headset\",\"Bluetooth\"]}"
		"}",
		NULL
	},
	{
		SUBJ_MUSIC_FREQUENCY,
		OPS_READ,
		"{" TRIG_DEF_TOTAL_COUNT "}",
		"{" TRIG_DEF_TIME_OF_DAY "," TRIG_DEF_DAY_OF_WEEK "}"
	},
	{
		SUBJ_VIDEO_FREQUENCY,
		OPS_READ,
		"{" TRIG_DEF_TOTAL_COUNT "}",
		"{" TRIG_DEF_TIME_OF_DAY "," TRIG_DEF_DAY_OF_WEEK "}"
	},
	{
		SUBJ_STATE_MESSAGE,
		OPS_SUBSCRIBE,
		"{"
			"\"Event\":{\"type\":\"string\",\"values\":[\"Received\"]},"
			"\"Type\":{\"type\":\"string\",\"values\":[\"SMS\",\"MMS\"]},"
			"\"Address\":{\"type\":\"string\"}"
		"}",
		NULL
	},
	{
		SUBJ_SOCIAL_FREQUENCY,
		OPS_READ,
		"{" TRIG_DEF_RANK "," TRIG_DEF_TOTAL_COUNT "}",
		"{"
			"\"Address\":{\"type\":\"string\"},"
			TRIG_DEF_TIME_OF_DAY "," TRIG_DEF_DAY_OF_WEEK
		"}"
	},
	{
		SUBJ_STATE_BATTERY,
		OPS_SUBSCRIBE | OPS_READ,
		"{"
			"\"Level\":{\"type\":\"string\",\"values\":[\"Empty\",\"Critical\",\"Low\",\"Normal\",\"High\",\"Full\"]},"
			TRIG_BOOL_ITEM_DEF("IsCharging")
		"}",
		NULL
	},
	{
		SUBJ_STATE_CHARGER,
		OPS_SUBSCRIBE | OPS_READ,
		"{" TRIG_BOOL_ITEM_DEF("IsConnected") "}",
		NULL
	},
	{
		SUBJ_STATE_GPS,
		OPS_SUBSCRIBE | OPS_READ,
		"{"
			"\"State\":{\"type\":\"string\",\"values\":[\"Disabled\",\"Searching\",\"Connected\"]}"
		"}",
		NULL
	},
	{
		SUBJ_STATE_PSMODE,
		OPS_SUBSCRIBE | OPS_READ,
		"{" TRIG_BOOL_ITEM_DEF("IsEnabled") "}",
		NULL
	},
	{
		SUBJ_STATE_USB,
		OPS_SUBSCRIBE | OPS_READ,
		"{" TRIG_BOOL_ITEM_DEF("IsConnected") "}",
		NULL
	},
	{
		SUBJ_STATE_ALARM,
		OPS_SUBSCRIBE,
		"{"
			"\"TimeOfDay\":{\"type\":\"integer\",\"min\":0,\"max\":1439},"
			"\"DayOfWeek\":{\"type\":\"string\",\"values\":[\"Mon\",\"Tue\",\"Wed\",\"Thu\",\"Fri\",\"Sat\",\"Sun\",\"Weekday\",\"Weekend\"]}"
		"}",
		NULL
	},
	{
		SUBJ_STATE_TIME,
		OPS_READ,
		"{"
			"\"TimeOfDay\":{\"type\":\"integer\",\"min\":0,\"max\":1439},"
			"\"DayOfWeek\":{\"type\":\"string\",\"values\":[\"Mon\",\"Tue\",\"Wed\",\"Thu\",\"Fri\",\"Sat\",\"Sun\",\"Weekday\",\"Weekend\"]},"
			"\"DayOfMonth\":{\"type\":\"integer\",\"min\":1,\"max\":31}"
		"}",
		NULL
	}
};

#endif	/* _CONTEXT_PROVIDER_TYPES_H_ */
