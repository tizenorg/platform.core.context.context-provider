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

#include <sqlite3.h>
#include <SensorRecorderTypes.h>
#include "../TypesInternal.h"
#include "PressureQuerier.h"

#define PROJECTION \
	"AVG(" KEY_PRESSURE ") AS " KEY_PRESSURE ", " \
	"MIN(" KEY_PRESSURE ") AS " KEY_MIN_PRESSURE ", " \
	"MAX(" KEY_PRESSURE ") AS " KEY_MAX_PRESSURE ", " \
	"AVG(" KEY_PRESSURE ") AS " KEY_AVG_PRESSURE ", " \
	"MIN(" KEY_UNIV_TIME ") AS " KEY_START_TIME ", " \
	"MAX(" KEY_UNIV_TIME ") AS " KEY_END_TIME

#define PROJECTION_RAW \
	KEY_PRESSURE ", " \
	KEY_PRESSURE " AS " KEY_MIN_PRESSURE ", " \
	KEY_PRESSURE " AS " KEY_MAX_PRESSURE ", " \
	KEY_PRESSURE " AS " KEY_AVG_PRESSURE ", " \
	KEY_UNIV_TIME " AS " KEY_START_TIME ", " \
	KEY_UNIV_TIME " AS " KEY_END_TIME

using namespace ctx;

PressureQuerier::PressureQuerier(ContextProvider *provider, Json option) :
	Querier(provider, option)
{
}

PressureQuerier::~PressureQuerier()
{
}

int PressureQuerier::queryRaw(int startTime, int endTime)
{
	char *sql = sqlite3_mprintf(
			"SELECT " PROJECTION_RAW \
			" FROM " PRESSURE_RECORD \
			" WHERE " KEY_UNIV_TIME " > %llu AND " KEY_UNIV_TIME " <= %llu",
			SEC_TO_MS(static_cast<uint64_t>(startTime)), SEC_TO_MS(static_cast<uint64_t>(endTime)));

	int ret = Querier::query(sql);
	sqlite3_free(sql);

	return ret;
}

int PressureQuerier::query(int startTime, int endTime)
{
	char *sql = sqlite3_mprintf(
			"SELECT " PROJECTION \
			" FROM " PRESSURE_RECORD \
			" WHERE " KEY_UNIV_TIME " > %llu AND " KEY_UNIV_TIME " <= %llu",
			SEC_TO_MS(static_cast<uint64_t>(startTime)), SEC_TO_MS(static_cast<uint64_t>(endTime)));

	int ret = Querier::query(sql);
	sqlite3_free(sql);

	return ret;
}

int PressureQuerier::query(int startTime, int endTime, int anchor, int interval)
{
	char *sql = sqlite3_mprintf(
			"SELECT " PROJECTION \
			" FROM " PRESSURE_RECORD \
			" WHERE " KEY_UNIV_TIME " > %llu AND " KEY_UNIV_TIME " <= %llu" \
			" GROUP BY ROUND((" KEY_UNIV_TIME " - %llu) / %llu + 0.5)" \
			" ORDER BY " KEY_END_TIME " ASC",
			SEC_TO_MS(static_cast<uint64_t>(startTime)), SEC_TO_MS(static_cast<uint64_t>(endTime)),
			SEC_TO_MS(static_cast<uint64_t>(anchor)), SEC_TO_MS(static_cast<uint64_t>(interval)));

	int ret = Querier::query(sql);
	sqlite3_free(sql);

	return ret;
}
