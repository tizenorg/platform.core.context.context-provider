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

#ifndef __CONTEXT_SENSOR_RECORDER_TYPES_INTERNAL_H__
#define __CONTEXT_SENSOR_RECORDER_TYPES_INTERNAL_H__

/* Tables */
#define PEDOMETER_RECORD	"SensorPedometerRecord"
#define PRESSURE_RECORD		"SensorPressureRecord"
#define CLIENT_INFO			"SensorClientInfo"

/* Privileges */
#define PRIV_HEALTHINFO		"healthinfo"

/* Constants */
#define SECONDS_PER_MINUTE	60
#define SECONDS_PER_HOUR	3600
#define SECONDS_PER_DAY		86400

/* Default  Parameters */
#define DEFAULT_RETENTION		SECONDS_PER_HOUR	/* 1 hour */
#define DEFAULT_QUERY_PERIOD	SECONDS_PER_DAY		/* 1 day */

/* Time Conversions */
#define SEC_TO_MS(X)	((X) * 1000)

#endif	/* __CONTEXT_SENSOR_RECORDER_TYPES_INTERNAL_H__ */
