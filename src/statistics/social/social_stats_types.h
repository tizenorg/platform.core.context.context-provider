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

#ifndef __CONTEXT_SOCIAL_STATS_TYPES_H__
#define __CONTEXT_SOCIAL_STATS_TYPES_H__

#include "../shared/common_types.h"

#define SOCIAL_HISTORY_PRIV			"callhistory.read"
#define SOCIAL_SUBJ_FREQ_ADDRESS	"stats/contact/often"
#define SOCIAL_SUBJ_FREQUENCY		"stats/contact/frequency"

#define SOCIAL_TABLE_CONTACT_LOG	"Log_Contact"
#define SOCIAL_TABLE_CONTACT_LOG_COLUMNS \
	"Address TEXT NOT NULL, PLogType INTEGER NOT NULL, " \
	"Duration INTEGER NOT NULL DEFAULT 0, " \
	"UTC TIMESTAMP DEFAULT (strftime('%s', 'now')), " \
	"LocalTime TIMESTAMP DEFAULT (strftime('%s', 'now', 'localtime'))"

#define SOCIAL_TEMP_CONTACT_FREQ	"Temp_ContactFreq"
#define SOCIAL_TEMP_CONTACT_FREQ_SQL \
	"CREATE TABLE IF NOT EXISTS " SOCIAL_TEMP_CONTACT_FREQ \
	" (Address TEXT NOT NULL UNIQUE, TotalCount INTEGER DEFAULT 0);"

#define SOCIAL_COMMUNICATION_TYPE	"CommunicationType"
#define SOCIAL_ADDRESS				"Address"
#define SOCIAL_PHONE_LOG_TYPE		"PLogType"
#define TIME_DIFFERENCE				"TimeDIff"

enum _social_comm_type_e {
	SOCIAL_COMMUNICATION_TYPE_CALL = 1,
	SOCIAL_COMMUNICATION_TYPE_MESSAGE,
	SOCIAL_COMMUNICATION_TYPE_ALL
};

#endif
