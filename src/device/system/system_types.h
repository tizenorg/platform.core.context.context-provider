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

#ifndef	__CONTEXT_DEVICESTATUS_TYPES_H__
#define	__CONTEXT_DEVICESTATUS_TYPES_H__

// Subject
#define DEVICE_ST_SUBJ_BATTERY			"system/battery"
#define DEVICE_ST_SUBJ_CHARGER			"system/charger"
#define DEVICE_ST_SUBJ_HEADPHONE		"system/headphone"
#define DEVICE_ST_SUBJ_WIFI				"system/wifi"
#define DEVICE_ST_SUBJ_USB				"system/usb"
#define DEVICE_ST_SUBJ_GPS				"system/gps"
#define DEVICE_ST_SUBJ_PSMODE			"system/psmode"
#define DEVICE_ST_SUBJ_ALARM			"device/alarm"
#define DEVICE_ST_SUBJ_TIME				"device/time"

// Data Key
#define DEVICE_ST_EVENT			"Event"
#define DEVICE_ST_STATE			"State"
#define DEVICE_ST_TYPE			"Type"
#define DEVICE_ST_LEVEL			"Level"
#define DEVICE_ST_BSSID			"BSSID"
#define DEVICE_ST_IS_CONNECTED	"IsConnected"
#define DEVICE_ST_IS_ENABLED	"IsEnabled"
#define DEVICE_ST_IS_CHARGING	"IsCharging"
#define DEVICE_ST_DETECTED		"Detected"
#define DEVICE_ST_TIME_OF_DAY	"TimeOfDay"
#define DEVICE_ST_DAY_OF_WEEK	"DayOfWeek"
#define DEVICE_ST_DAY_OF_MONTH	"DayOfMonth"

// Data Value
#define DEVICE_ST_TRUE			1
#define DEVICE_ST_FALSE			0
#define DEVICE_ST_ENTER			"Enter"
#define DEVICE_ST_EXIT			"Exit"
#define DEVICE_ST_DISABLED		"Disabled"
#define DEVICE_ST_CONNECTED		"Connected"
#define DEVICE_ST_UNCONNECTED	"Unconnected"
#define DEVICE_ST_SEARCHING		"Searching"
#define DEVICE_ST_EMPTY			"Empty"
#define DEVICE_ST_CRITICAL		"Critical"
#define DEVICE_ST_LOW			"Low"
#define DEVICE_ST_NORMAL		"Normal"
#define DEVICE_ST_HIGH			"High"
#define DEVICE_ST_FULL			"Full"
#define DEVICE_ST_HEADSET		"Headset"
#define DEVICE_ST_BLUETOOTH		"Bluetooth"

#define TRIG_BOOL_ITEM_DEF(sbj)	"\"" sbj "\":{\"type\":\"integer\",\"min\":0,\"max\":1}"

#endif	//__CONTEXT_DEVICESTATUS_TYPES_H__
