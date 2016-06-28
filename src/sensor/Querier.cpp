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

#include <Types.h>
#include <SensorRecorderTypes.h>
#include "Querier.h"

using namespace ctx;

Querier::Querier(ContextProvider *provider, Json option) :
	__provider(provider),
	__option(option)
{
}

Querier::~Querier()
{
}

int Querier::query(const char *sql)
{
	return __dbMgr.execute(0, sql, this) ? ERR_NONE : ERR_OPERATION_FAILED;
}

int Querier::queryRaw(int startTime, int endTime)
{
	return ERR_INVALID_PARAMETER;
}

int Querier::query(int startTime, int endTime)
{
	return ERR_INVALID_PARAMETER;
}

int Querier::query(int startTime, int endTime, int anchor, int interval)
{
	return ERR_INVALID_PARAMETER;
}

void Querier::onTableCreated(unsigned int queryId, int error)
{
}

void Querier::onInserted(unsigned int queryId, int error, int64_t rowId)
{
}

void Querier::onExecuted(unsigned int queryId, int error, std::vector<Json>& records)
{
	Json response;
	__convertToResponse(records, response);
	__provider->replyToRead(__option, error, response);
	delete this;
}

void Querier::__convertToResponse(std::vector<Json> &sqlResult, Json &response)
{
	std::list<std::string> keys;
	std::string val;

	response.set(NULL, KEY_RESULT_SIZE, static_cast<int>(sqlResult.size()));

	for (Json &tuple : sqlResult) {
		tuple.getKeys(&keys);
		for (std::string &key : keys) {
			tuple.get(NULL, key.c_str(), &val);
			response.append(NULL, key.c_str(), val);
		}
		keys.clear();
	}
}
