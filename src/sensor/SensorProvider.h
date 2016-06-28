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

#ifndef __CONTEXT_SENSOR_PROVIDER_H__
#define __CONTEXT_SENSOR_PROVIDER_H__

#include <ContextProvider.h>
#include "ClientInfo.h"
#include "SensorLogger.h"
#include "Querier.h"

namespace ctx {

	class SensorProvider : public ContextProvider {
	public:
		SensorProvider(const char *subject);
		virtual ~SensorProvider();

		virtual int subscribe(Json option, Json *requestResult);
		virtual int unsubscribe(Json option);
		virtual int read(Json option, Json *requestResult);
		virtual int write(Json data, Json *requestResult);

	protected:
		virtual Querier* getQuerier(Json option) = 0;

		SensorLogger *sensorLogger;

	private:
		int __addClient(std::string pkgId, int retentionPeriod, Json option);
		int __removeClient(std::string pkgId);

		ClientInfo __clientInfo;
	};
}

#endif /* _CONTEXT_SENSOR_PROVIDER_H_ */
