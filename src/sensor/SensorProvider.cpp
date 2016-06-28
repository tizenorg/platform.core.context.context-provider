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

#include <ctime>
#include <cmath>
#include <SensorRecorderTypes.h>
#include "TypesInternal.h"
#include "SensorProvider.h"

using namespace ctx;

SensorProvider::SensorProvider(const char *subject) :
	ContextProvider(subject),
	sensorLogger(NULL)
{
}

SensorProvider::~SensorProvider()
{
	delete sensorLogger;
}

int SensorProvider::subscribe(Json option, Json *requestResult)
{
	return ERR_NONE;
}

int SensorProvider::unsubscribe(Json option)
{
	return ERR_NONE;
}

int SensorProvider::read(Json option, Json *requestResult)
{
	int endTime = static_cast<int>(time(NULL)) + 1;
	int startTime = endTime - DEFAULT_QUERY_PERIOD - 1;
	int anchor = -1;
	int interval = -1;

	if (option.get(NULL, KEY_START_TIME, &startTime))
		IF_FAIL_RETURN(startTime >= 0, ERR_INVALID_PARAMETER);

	if (option.get(NULL, KEY_END_TIME, &endTime))
		IF_FAIL_RETURN(endTime >= 0, ERR_INVALID_PARAMETER);

	if (option.get(NULL, KEY_ANCHOR, &anchor))
		IF_FAIL_RETURN(anchor >= 0, ERR_INVALID_PARAMETER);

	if (option.get(NULL, KEY_INTERVAL, &interval))
		IF_FAIL_RETURN(interval >= 0, ERR_INVALID_PARAMETER);

	if (endTime >= 0 && startTime >= endTime)
		return ERR_INVALID_PARAMETER;

	if (interval > 0 && anchor < 0)
		anchor = endTime;

	if (anchor >= 0 && interval < 0)
		interval = static_cast<int>(ceil(static_cast<double>(endTime - startTime) / SECONDS_PER_MINUTE));

	int ret;
	Querier *querier = getQuerier(option);
	IF_FAIL_RETURN(querier, ERR_OPERATION_FAILED);

	if (interval == 0)
		ret = querier->queryRaw(startTime, endTime);
	else if (interval > 0)
		ret = querier->query(startTime, endTime, anchor, interval * SECONDS_PER_MINUTE);
	else
		ret = querier->query(startTime, endTime);

	if (ret != ERR_NONE)
		delete querier;

	return ret;
}

int SensorProvider::write(Json data, Json *requestResult)
{
	IF_FAIL_RETURN(sensorLogger, ERR_OPERATION_FAILED);

	std::string operation;
	std::string pkgId;
	int retentionPeriod = DEFAULT_RETENTION;

	_J("Data", data);

	IF_FAIL_RETURN(data.get(NULL, KEY_OPERATION, &operation), ERR_INVALID_PARAMETER);
	IF_FAIL_RETURN(data.get(NULL, KEY_CLIENT_PKG_ID, &pkgId), ERR_INVALID_PARAMETER);

	if (data.get(NULL, KEY_RETENTION, &retentionPeriod))
		retentionPeriod *= SECONDS_PER_HOUR;

	/* TODO: remove the operation & pkg id from the json */

	if (operation == VAL_START)
		return __addClient(pkgId, retentionPeriod, data);
	else if (operation == VAL_STOP)
		return __removeClient(pkgId);

	return ERR_NOT_SUPPORTED;
}

int SensorProvider::__addClient(std::string pkgId, int retentionPeriod, Json option)
{
	Json tmp;
	int ret;

	/* Check if the app already started Sensor recording */
	ret = __clientInfo.get(getSubject(), pkgId, tmp);
	IF_FAIL_RETURN(ret != ERR_NONE, ERR_ALREADY_STARTED);
	IF_FAIL_RETURN(ret == ERR_NO_DATA, ERR_OPERATION_FAILED);

	/* Store the app's request */
	if (!__clientInfo.set(getSubject(), pkgId, option, retentionPeriod))
		return ERR_OPERATION_FAILED;

	/* If not listening the sensor yet, start */
	sensorLogger->start();

	return ERR_NONE;
}

int SensorProvider::__removeClient(std::string pkgId)
{
	std::vector<Json> options;
	int ret;

	/* Remove the app's request first */
	IF_FAIL_RETURN(__clientInfo.remove(getSubject(), pkgId), ERR_OPERATION_FAILED);

	/* Check if there is no client anymore */
	ret = __clientInfo.get(getSubject(), options);
	IF_FAIL_RETURN(ret != ERR_NONE, ERR_NONE);
	IF_FAIL_RETURN(ret == ERR_NO_DATA, ERR_OPERATION_FAILED);

	/* Stop listening */
	sensorLogger->stop();

	return ERR_NONE;
}
