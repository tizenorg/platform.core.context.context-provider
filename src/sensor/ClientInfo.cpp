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
#include <Types.h>
#include <SensorRecorderTypes.h>
#include "TypesInternal.h"
#include "SensorProvider.h"
#include "ClientInfo.h"

using namespace ctx;

unsigned int ClientInfo::__refCnt = 0;
DatabaseManager *ClientInfo::__dbMgr = NULL;
UninstallMonitor *ClientInfo::__uninstallMonitor = NULL;

ClientInfo::ClientInfo()
{
	if (++__refCnt != 1)
		return;

	__uninstallMonitor = new(std::nothrow) UninstallMonitor();
	IF_FAIL_VOID_TAG(__uninstallMonitor, _E, "Memory allocation failed");

	__dbMgr = new(std::nothrow) DatabaseManager();
	IF_FAIL_VOID_TAG(__dbMgr, _E, "Memory allocation failed");

	bool ret = __dbMgr->executeSync(
			"CREATE TABLE IF NOT EXISTS " CLIENT_INFO " (" \
				KEY_SUBJECT " TEXT NOT NULL," \
				KEY_PKG_ID " TEXT NOT NULL," \
				KEY_OPTION " TEXT NOT NULL," \
				KEY_RETENTION " INTEGER NOT NULL," \
				"PRIMARY KEY (" KEY_SUBJECT "," KEY_PKG_ID ")" \
			")", NULL);

	IF_FAIL_VOID_TAG(ret, _E, "Table creation failed");
}

ClientInfo::~ClientInfo()
{
	if (--__refCnt != 0)
		return;

	delete __dbMgr;
	__dbMgr = NULL;

	delete __uninstallMonitor;
	__uninstallMonitor = NULL;
}

int ClientInfo::get(std::string subject, std::string pkgId, Json& option)
{
	IF_FAIL_RETURN_TAG(__dbMgr, ERR_OPERATION_FAILED, _W, "DB not initialized");

	bool ret;
	std::string optStr;
	std::vector<Json> records;
	char *query = sqlite3_mprintf(
			"SELECT " KEY_OPTION " FROM " CLIENT_INFO " WHERE " \
			KEY_SUBJECT "='%q' AND " KEY_PKG_ID "='%q'",
			subject.c_str(), pkgId.c_str());

	ret = __dbMgr->executeSync(query, &records);
	sqlite3_free(query);

	IF_FAIL_RETURN(ret, ERR_OPERATION_FAILED);
	IF_FAIL_RETURN(!records.empty(), ERR_NO_DATA);
	IF_FAIL_RETURN(records[0].get(NULL, KEY_OPTION, &optStr), ERR_OPERATION_FAILED);

	option = optStr;

	return ERR_NONE;
}

int ClientInfo::get(std::string subject, std::vector<Json>& options)
{
	IF_FAIL_RETURN_TAG(__dbMgr, ERR_OPERATION_FAILED, _W, "DB not initialized");

	bool ret;
	std::string optStr;
	std::vector<Json> records;
	char *query = sqlite3_mprintf(
			"SELECT " KEY_OPTION " FROM " CLIENT_INFO " WHERE " \
			KEY_SUBJECT "='%q'",
			subject.c_str());

	ret = __dbMgr->executeSync(query, &records);
	sqlite3_free(query);

	IF_FAIL_RETURN(ret, ERR_OPERATION_FAILED);
	IF_FAIL_RETURN(!records.empty(), ERR_NO_DATA);

	for (Json& jObj : records) {
		if (!jObj.get(NULL, KEY_OPTION, &optStr))
			continue;
		options.push_back(Json(optStr));
	}

	return ERR_NONE;
}

bool ClientInfo::exist(std::string subject)
{
	IF_FAIL_RETURN_TAG(__dbMgr, ERR_OPERATION_FAILED, _W, "DB not initialized");

	bool ret;
	std::vector<Json> records;
	char *query = sqlite3_mprintf(
			"SELECT " KEY_PKG_ID " FROM " CLIENT_INFO " WHERE " \
			KEY_SUBJECT "='%q' LIMIT 1",
			subject.c_str());

	ret = __dbMgr->executeSync(query, &records);
	sqlite3_free(query);

	IF_FAIL_RETURN(ret, false);
	IF_FAIL_RETURN(!records.empty(), false);

	return true;
}

bool ClientInfo::set(std::string subject, std::string pkgId, Json option, int retentionPeriod)
{
	IF_FAIL_RETURN_TAG(__dbMgr, false, _W, "DB not initialized");

	bool ret;
	char *query = sqlite3_mprintf(
			"INSERT INTO " CLIENT_INFO " VALUES ('%q', '%q', '%q', %d)",
			subject.c_str(), pkgId.c_str(), option.str().c_str(), retentionPeriod);

	ret = __dbMgr->executeSync(query, NULL);
	sqlite3_free(query);

	return ret;
}

bool ClientInfo::remove(std::string subject, std::string pkgId)
{
	IF_FAIL_RETURN_TAG(__dbMgr, false, _W, "DB not initialized");

	bool ret;
	char *query = sqlite3_mprintf(
			"DELETE FROM " CLIENT_INFO " WHERE " \
			KEY_SUBJECT "='%q' AND " KEY_PKG_ID "='%q'",
			subject.c_str(), pkgId.c_str());

	ret = __dbMgr->executeSync(query, NULL);
	sqlite3_free(query);

	return ret;
}

void ClientInfo::getParam(std::vector<Json> &options, const char *key, float *min, float *max)
{
	double val;

	for (Json& opt : options) {
		if (!opt.get(NULL, key, &val))
			continue;
		if (min)
			*min = MIN(*min, static_cast<float>(val));
		if (max)
			*max = MAX(*max, static_cast<float>(val));
	}
}

void ClientInfo::purgeClient(std::string pkgId)
{
	IF_FAIL_VOID_TAG(__dbMgr, _W, "DB not initialized");

	bool ret;
	std::string subject;
	std::vector<Json> records;

	char *query = sqlite3_mprintf(
			"SELECT " KEY_SUBJECT " FROM " CLIENT_INFO " WHERE " KEY_PKG_ID "='%q'",
			pkgId.c_str());

	ret = __dbMgr->executeSync(query, &records);
	sqlite3_free(query);
	IF_FAIL_VOID(ret);

	for (Json& jObj : records) {
		if (!jObj.get(NULL, KEY_SUBJECT, &subject))
			continue;
		_I("Stop recording '%s' for '%s'", subject.c_str(), pkgId.c_str());
		SensorProvider::removeClient(subject, pkgId);
	}
}
