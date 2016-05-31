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

#ifndef _CONTEXT_PROVIDER_TYPES_H_
#define _CONTEXT_PROVIDER_TYPES_H_

#include "MyPlaceTypes.h"

/* Privileges */
#define PRIV_ALARM		"alarm.set"
#define PRIV_NETWORK	"network.get"
#define PRIV_TELEPHONY	"telephony"
#define PRIV_MESSAGE	"message.read"
#define PRIV_CONTACT	"contact.read"
#define PRIV_LOCATION	"location"
#define PRIV_APP_HISTORY	"apphistory.read"
#define PRIV_MEDIA_HISTORY	"mediahistory.read"
#define PRIV_CALL_HISTORY	"callhistory.read"


/* FW-wide Data Logger Parameters */
#define LOG_RETENTION_PERIOD	7776000		/* 90 days in secs */


/* FW-wide Default Values */
#define DEFAULT_TIMESPAN		30
#define DEFAULT_LIMIT			10


/* Subjects */
/* TODO: Cleanup the below namings */
#define SUBJ_STATE_BATTERY		"system/battery"
#define SUBJ_STATE_CHARGER		"system/charger"
#define SUBJ_STATE_HEADPHONE	"system/headphone"
#define SUBJ_STATE_WIFI			"system/wifi"
#define SUBJ_STATE_USB			"system/usb"
#define SUBJ_STATE_GPS			"system/gps"
#define SUBJ_STATE_PSMODE		"system/psmode"
#define SUBJ_STATE_ALARM		"device/alarm"
#define SUBJ_STATE_TIME			"device/time"

#define SUBJ_STATE_CALL			"social/call"
#define SUBJ_STATE_EMAIL		"social/email"
#define SUBJ_STATE_MESSAGE		"social/message"
#define SUBJ_STATE_CONTACTS		"social/contacts"

#define SUBJ_ACTIVITY				"activity/"
#define SUBJ_ACTIVITY_IN_VEHICLE	SUBJ_ACTIVITY "in_vehicle"
#define SUBJ_ACTIVITY_RUNNING		SUBJ_ACTIVITY "running"
#define SUBJ_ACTIVITY_STATIONARY	SUBJ_ACTIVITY "stationary"
#define SUBJ_ACTIVITY_WALKING		SUBJ_ACTIVITY "walking"

#define SUBJ_APP_STATS				"stats/app/"
#define SUBJ_APP_LOGGER				SUBJ_APP_STATS "logger"
#define SUBJ_APP_RECENTLY_USED		SUBJ_APP_STATS "recently"
#define SUBJ_APP_FREQUENTLY_USED	SUBJ_APP_STATS "often"
#define SUBJ_APP_RARELY_USED		SUBJ_APP_STATS "rarely"
#define SUBJ_APP_PEAK_TIME			SUBJ_APP_STATS "peak_time"
#define SUBJ_APP_COMMON_SETTING		SUBJ_APP_STATS "setting"
#define SUBJ_APP_FREQUENCY			SUBJ_APP_STATS "frequency"

#define SUBJ_MEDIA_LOGGER			"stats/media/logger"
#define SUBJ_MUSIC_STATS			"stats/music/"
#define SUBJ_MUSIC_PEAK_TIME		SUBJ_MUSIC_STATS "peak_time"
#define SUBJ_MUSIC_COMMON_SETTING	SUBJ_MUSIC_STATS "setting"
#define SUBJ_MUSIC_FREQUENCY		SUBJ_MUSIC_STATS "frequency"
#define SUBJ_VIDEO_STATS			"stats/video/"
#define SUBJ_VIDEO_PEAK_TIME		SUBJ_VIDEO_STATS "peak_time"
#define SUBJ_VIDEO_COMMON_SETTING	SUBJ_VIDEO_STATS "setting"
#define SUBJ_VIDEO_FREQUENCY		SUBJ_VIDEO_STATS "frequency"

#define SUBJ_SOCIAL_STATS			"stats/contact/"
#define SUBJ_SOCIAL_FREQ_ADDRESS	SUBJ_SOCIAL_STATS "often"
#define SUBJ_SOCIAL_FREQUENCY		SUBJ_SOCIAL_STATS "frequency"

#define SUBJ_PLACE_GEOFENCE		"place/geofence"
#define SUBJ_PLACE_DETECTION	PLACE_DETECTION_SUBJECT

/* Data & Option Keys */
#define KEY_QUERY_RESULT	"QueryResult"
#define KEY_RESULT_SIZE		"ResultSize"
#define KEY_COL_ROW_ID		"rowId"
#define KEY_TIMESPAN		"TimeSpan"
#define KEY_START_TIME		"StartTime"
#define KEY_END_TIME		"EndTime"
#define KEY_LAST_TIME		"LastTime"
#define KEY_TOTAL_COUNT		"TotalCount"
#define KEY_AVERAGE_COUNT	"AvgCount"
#define KEY_DURATION		"Duration"
#define KEY_TOTAL_DURATION	"TotalDuration"
#define KEY_DAY_OF_WEEK		"DayOfWeek"
#define KEY_HOUR_OF_DAY		"HourOfDay"
#define KEY_TIME_OF_DAY		"TimeOfDay"
#define KEY_TOTAL_COUNT		"TotalCount"
#define KEY_APP_ID			"AppId"
#define KEY_PKG_ID			"PkgId"
#define KEY_AUDIO_JACK		"AudioJack"
#define KEY_SYSTEM_VOLUME	"SystemVolume"
#define KEY_MEDIA_VOLUME	"MediaVolume"
#define KEY_BSSID			"BSSID"
#define KEY_UNIV_TIME		"UTC"
#define KEY_LOCAL_TIME		"LocalTime"
#define KEY_RANK			"Rank"

#define KEY_EVENT			"Event"
#define KEY_STATE			"State"
#define KEY_TYPE			"Type"
#define KEY_LEVEL			"Level"
#define KEY_ACCURACY		"Accuracy"
#define KEY_BSSID			"BSSID"
#define KEY_MEDIUM			"Medium"
#define KEY_ADDRESS			"Address"
#define KEY_IS_CONNECTED	"IsConnected"
#define KEY_IS_ENABLED		"IsEnabled"
#define KEY_IS_CHARGING		"IsCharging"
#define KEY_DETECTED		"Detected"
#define KEY_TIME_OF_DAY		"TimeOfDay"
#define KEY_DAY_OF_WEEK		"DayOfWeek"
#define KEY_DAY_OF_MONTH	"DayOfMonth"
#define KEY_PLACE_ID		"PlaceId"


/* Data & Option Values */
#define VAL_TRUE			1
#define VAL_FALSE			0
#define VAL_ENTER			"Enter"
#define VAL_EXIT			"Exit"
#define VAL_DISABLED		"Disabled"
#define VAL_CONNECTED		"Connected"
#define VAL_UNCONNECTED		"Unconnected"
#define VAL_SEARCHING		"Searching"
#define VAL_EMPTY			"Empty"
#define VAL_CRITICAL		"Critical"
#define VAL_LOW				"Low"
#define VAL_NORMAL			"Normal"
#define VAL_HIGH			"High"
#define VAL_FULL			"Full"
#define VAL_HEADSET			"Headset"
#define VAL_BLUETOOTH		"Bluetooth"
#define VAL_IDLE			"Idle"
#define VAL_CONNECTING		"Connecting"
#define VAL_CONNECTED		"Connected"
#define VAL_HELD			"Held"
#define VAL_DIALING			"Dialing"
#define VAL_VOICE			"Voice"
#define VAL_VIDEO			"Video"
#define VAL_SENT			"Sent"
#define VAL_RECEIVED		"Received"
#define VAL_SMS				"SMS"
#define VAL_MMS				"MMS"
#define VAL_MY_PROFILE		"MyProfile"
#define VAL_PERSON			"Person"
#define VAL_CHANGED			"Changed"
#define VAL_DETECTED		"Detected"
#define	VAL_UNCERTAIN		"Uncertain"
#define	VAL_IN				"In"
#define	VAL_OUT				"Out"

#define VAL_ACTIVE		VAL_CONNECTED
#define VAL_ALERTING	VAL_CONNECTING
#define VAL_INCOMING	VAL_CONNECTING


/* Json Formats */
#define TRIG_DEF_RANK			"\"Rank\":{\"type\":\"integer\",\"min\":1}"
#define TRIG_DEF_TOTAL_COUNT	"\"TotalCount\":{\"type\":\"integer\",\"min\":0}"
#define TRIG_DEF_TIME_OF_DAY	"\"TimeOfDay\":{\"type\":\"string\"}"
#define TRIG_DEF_DAY_OF_WEEK	"\"DayOfWeek\":{\"type\":\"string\",\"values\":[\"Mon\",\"Tue\",\"Wed\",\"Thu\",\"Fri\",\"Sat\",\"Sun\",\"Weekday\",\"Weekend\"]}"
#define TRIG_BOOL_ITEM_DEF(sbj)	"\"" sbj "\":{\"type\":\"integer\",\"min\":0,\"max\":1}"


#endif	/* _CONTEXT_PROVIDER_TYPES_H_ */
