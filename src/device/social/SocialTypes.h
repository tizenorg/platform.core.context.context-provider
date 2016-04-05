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

#ifndef _CONTEXT_SOCIAL_STATUS_TYPES_H_
#define _CONTEXT_SOCIAL_STATUS_TYPES_H_

// Subject
#define SOCIAL_ST_SUBJ_CALL		"social/call"
#define SOCIAL_ST_SUBJ_EMAIL	"social/email"
#define SOCIAL_ST_SUBJ_MESSAGE	"social/message"
#define SOCIAL_ST_SUBJ_CONTACTS	"social/contacts"

// Data Key
#define SOCIAL_ST_STATE			"State"
#define SOCIAL_ST_EVENT			"Event"
#define SOCIAL_ST_TYPE			"Type"
#define SOCIAL_ST_MEDIUM		"Medium"
#define SOCIAL_ST_ADDRESS		"Address"

// Data Values
#define SOCIAL_ST_IDLE			"Idle"
#define SOCIAL_ST_CONNECTING	"Connecting"
#define SOCIAL_ST_CONNECTED		"Connected"
#define SOCIAL_ST_ACTIVE		SOCIAL_ST_CONNECTED
#define SOCIAL_ST_HELD			"Held"
#define SOCIAL_ST_DIALING		"Dialing"
#define SOCIAL_ST_ALERTING		SOCIAL_ST_CONNECTING
#define SOCIAL_ST_INCOMING		SOCIAL_ST_CONNECTING
#define SOCIAL_ST_VOICE			"Voice"
#define SOCIAL_ST_VIDEO			"Video"
#define SOCIAL_ST_SENT			"Sent"
#define SOCIAL_ST_RECEIVED		"Received"
#define SOCIAL_ST_SMS			"SMS"
#define SOCIAL_ST_MMS			"MMS"
#define SOCIAL_ST_MY_PROFILE	"MyProfile"
#define SOCIAL_ST_PERSON		"Person"
#define SOCIAL_ST_CHANGED		"Changed"

#endif	//_CONTEXT_SOCIAL_STATUS_TYPES_H
