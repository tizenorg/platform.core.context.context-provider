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

#ifndef _CONTEXT_INACTIVE_DETECTOR_STORAGE_QUERIES_H_
#define _CONTEXT_INACTIVE_DETECTOR_STORAGE_QUERIES_H_

#include <vector>
#include "AppInactiveDetectorTypes.h"

/*

--1 calculate weights
SELECT
	app_info._id,
	app_info.package_name,
	app_info.is_nodisplay,
	app_info.is_enabled,
	app_info.is_atboot,
	app_info.is_preloaded,
	app_info.timestamp,

	app_hist_g.weight as weight
FROM
	context_app_info app_info

INNER JOIN

	(SELECT
		distinct package_name ,
			IFNULL((select count(package_name) from context_app_launch_log
				where package_name == app_hist.package_name  AND timestamp> 8640412900), 0 )
				*
			IFNULL((select sum(duration) from  context_app_launch_log
				where package_name == app_hist.package_name AND timestamp> 8640412900) , 0)
		as weight

	FROM context_app_launch_log app_hist
)app_hist_g

ON app_hist_g.package_name
=
app_info.package_name

ORDER BY app_hist_g.weight

*/

#define GET_APP_INFO_W_WEIGHT_QUERY "SELECT "\
	" " APP_INACTIVE_DETECTOR_APPINFO_TABLE_ALIAS  "." APP_INACTIVE_DETECTOR_APPINFO_COLUMN_ID ", "\
	" " APP_INACTIVE_DETECTOR_APPINFO_TABLE_ALIAS  "." APP_INACTIVE_DETECTOR_APPINFO_COLUMN_PACKAGE_NAME ", "\
	" " APP_INACTIVE_DETECTOR_APPINFO_TABLE_ALIAS  "." APP_INACTIVE_DETECTOR_APPINFO_COLUMN_PACKAGE_TYPE ", "\
	" " APP_INACTIVE_DETECTOR_APPINFO_TABLE_ALIAS  "." APP_INACTIVE_DETECTOR_APPINFO_COLUMN_IS_NODISPLAY ", "\
	" " APP_INACTIVE_DETECTOR_APPINFO_TABLE_ALIAS  "." APP_INACTIVE_DETECTOR_APPINFO_COLUMN_IS_ENABLED ", "\
	" " APP_INACTIVE_DETECTOR_APPINFO_TABLE_ALIAS  "." APP_INACTIVE_DETECTOR_APPINFO_COLUMN_IS_ATBOOT ", "\
	" " APP_INACTIVE_DETECTOR_APPINFO_TABLE_ALIAS  "." APP_INACTIVE_DETECTOR_APPINFO_COLUMN_IS_PRELOADED ", "\
	" " APP_INACTIVE_DETECTOR_APPINFO_TABLE_ALIAS  "." APP_INACTIVE_DETECTOR_APPINFO_COLUMN_TIMESTAMP ", "\
	""\
	" " APP_INACTIVE_DETECTOR_VIRTUAL_TABLE_ALIAS "." APP_INACTIVE_DETECTOR_VIRTUAL_COLUMN_WEIGHT \
	""\
	" FROM " APP_INACTIVE_DETECTOR_APPINFO_TABLE " " APP_INACTIVE_DETECTOR_APPINFO_TABLE_ALIAS \
	""\
	" INNER JOIN " \
	" 	(SELECT "\
		" DISTINCT " APP_INACTIVE_DETECTOR_APPINFO_COLUMN_PACKAGE_NAME "," \
			" IFNULL((SELECT COUNT(" APP_INACTIVE_DETECTOR_APPINFO_COLUMN_PACKAGE_NAME ")" \
				" FROM " APP_INACTIVE_DETECTOR_APPLAUNCH_LOG_TABLE \
					" WHERE " APP_INACTIVE_DETECTOR_APPINFO_COLUMN_PACKAGE_NAME "==" APP_INACTIVE_DETECTOR_APPLAUNCH_LOG_TABLE_ALIAS "." APP_INACTIVE_DETECTOR_APPINFO_COLUMN_PACKAGE_NAME \
					" AND " \
					" " APP_INACTIVE_DETECTOR_APPLAUNCH_LOG_COLUMN_TIMESTAMP " >= " APP_INACTIVE_DETECTOR_VALUE_PLACEHOLDER_TIMESTAMP \
			"), 0) " \
			" * " \
			" IFNULL((SELECT SUM(" APP_INACTIVE_DETECTOR_APPLAUNCH_LOG_COLUMN_DURATION ")" \
				" FROM " APP_INACTIVE_DETECTOR_APPLAUNCH_LOG_TABLE \
					" WHERE " APP_INACTIVE_DETECTOR_APPINFO_COLUMN_PACKAGE_NAME "==" APP_INACTIVE_DETECTOR_APPLAUNCH_LOG_TABLE_ALIAS "." APP_INACTIVE_DETECTOR_APPINFO_COLUMN_PACKAGE_NAME \
					" AND " \
					" " APP_INACTIVE_DETECTOR_APPLAUNCH_LOG_COLUMN_TIMESTAMP " >= " APP_INACTIVE_DETECTOR_VALUE_PLACEHOLDER_TIMESTAMP \
					""\
			"), 0) " \
			" as " APP_INACTIVE_DETECTOR_VIRTUAL_COLUMN_WEIGHT \
		" FROM " APP_INACTIVE_DETECTOR_APPLAUNCH_LOG_TABLE " " APP_INACTIVE_DETECTOR_APPLAUNCH_LOG_TABLE_ALIAS \
		") " APP_INACTIVE_DETECTOR_VIRTUAL_TABLE_ALIAS \
	" ON " APP_INACTIVE_DETECTOR_VIRTUAL_TABLE_ALIAS "." APP_INACTIVE_DETECTOR_APPINFO_COLUMN_PACKAGE_NAME \
	" = " \
	" " APP_INACTIVE_DETECTOR_APPINFO_TABLE_ALIAS "." APP_INACTIVE_DETECTOR_APPINFO_COLUMN_PACKAGE_NAME \
	" ORDER BY " APP_INACTIVE_DETECTOR_VIRTUAL_TABLE_ALIAS "." APP_INACTIVE_DETECTOR_VIRTUAL_COLUMN_WEIGHT

/*
--2 select inner join context_app_info
SELECT
	app_info.package_name,
	app_info.is_nodisplay,
	app_info.is_enabled,
	app_info.is_atboot,
	app_info.is_preloaded,

	activity_classified.activity_rate

FROM
	context_activity_classified activity_classified
INNER JOIN
	context_app_info app_info
ON activity_classified.context_app_info_id = app_info._id

WHERE
	activity_classified.is_active = 1
	AND
	activity_classified.timeframe = 1
*/

#define GET_APP_INFO_INACTIVE_QUERY "SELECT "\
	" " APP_INACTIVE_DETECTOR_APPINFO_TABLE_ALIAS  "." APP_INACTIVE_DETECTOR_APPINFO_COLUMN_ID ", "\
	" " APP_INACTIVE_DETECTOR_APPINFO_TABLE_ALIAS  "." APP_INACTIVE_DETECTOR_APPINFO_COLUMN_PACKAGE_NAME ", "\
	" " APP_INACTIVE_DETECTOR_APPINFO_TABLE_ALIAS  "." APP_INACTIVE_DETECTOR_APPINFO_COLUMN_IS_NODISPLAY ", "\
	" " APP_INACTIVE_DETECTOR_APPINFO_TABLE_ALIAS  "." APP_INACTIVE_DETECTOR_APPINFO_COLUMN_IS_ENABLED ", "\
	" " APP_INACTIVE_DETECTOR_APPINFO_TABLE_ALIAS  "." APP_INACTIVE_DETECTOR_APPINFO_COLUMN_IS_ATBOOT ", "\
	" " APP_INACTIVE_DETECTOR_APPINFO_TABLE_ALIAS  "." APP_INACTIVE_DETECTOR_APPINFO_COLUMN_IS_PRELOADED ", "\
	" "\
	" " APP_INACTIVE_DETECTOR_ACTIVITYCLASSIFIED_TABLE_ALIAS "." APP_INACTIVE_DETECTOR_ACTIVITYCLASSIFIED_COLUMN_ACTIVITY_RATE " "\
	" FROM "\
	" " APP_INACTIVE_DETECTOR_ACTIVITYCLASSIFIED_TABLE " " APP_INACTIVE_DETECTOR_ACTIVITYCLASSIFIED_TABLE_ALIAS " "\
	" INNER JOIN "\
	" " APP_INACTIVE_DETECTOR_APPINFO_TABLE " " APP_INACTIVE_DETECTOR_APPINFO_TABLE_ALIAS " "\
	" ON " APP_INACTIVE_DETECTOR_ACTIVITYCLASSIFIED_TABLE_ALIAS "." APP_INACTIVE_DETECTOR_ACTIVITYCLASSIFIED_COLUMN_CONTEXT_APP_INFO_ID ""\
		" = " APP_INACTIVE_DETECTOR_APPINFO_TABLE_ALIAS "." APP_INACTIVE_DETECTOR_APPINFO_COLUMN_ID " "\
	" WHERE "\
	APP_INACTIVE_DETECTOR_ACTIVITYCLASSIFIED_TABLE_ALIAS "." APP_INACTIVE_DETECTOR_ACTIVITYCLASSIFIED_COLUMN_IS_ACTIVE ""\
		" = " APP_INACTIVE_DETECTOR_VALUE_PLACEHOLDER_CLUSTER ""\
	" AND "\
	" " APP_INACTIVE_DETECTOR_ACTIVITYCLASSIFIED_TABLE_ALIAS "." APP_INACTIVE_DETECTOR_ACTIVITYCLASSIFIED_COLUMN_TIMEFRAME ""\
		" = " APP_INACTIVE_DETECTOR_VALUE_PLACEHOLDER_TIMEFRAME ""

/*
--3 update ranks and clusters for each timeframe: delete all for each time frame, add new calculations for apps
DELETE FROM
	context_activity_classified
WHERE
	timeframe = 1
*/

#define DELETE_APP_ACTIVITY_CLASSIFIED_BY_TIMEFRAME "DELETE FROM "\
	APP_INACTIVE_DETECTOR_ACTIVITYCLASSIFIED_TABLE \
	" WHERE "\
	" timeframe = " APP_INACTIVE_DETECTOR_VALUE_PLACEHOLDER_TIMEFRAME


/*INSERT INTO
	context_activity_classified
	(is_active,
	timeframe,
	context_app_info_id)
SELECT
	is_active,
	timeframe,
	context_app_info_id
FROM  --make loop
	(
	SELECT 1 as is_active, 1 as timeframe,  3964 as context_app_info_id
	UNION
	SELECT 0 as is_active, 1 as timeframe,  3964 as context_app_info_id
	) q
*/

#define INSERT_APP_ACTIVITY_CLASSIFIED " INSERT INTO "\
	APP_INACTIVE_DETECTOR_ACTIVITYCLASSIFIED_TABLE \
	"( " APP_INACTIVE_DETECTOR_ACTIVITYCLASSIFIED_COLUMN_IS_ACTIVE ", "\
	APP_INACTIVE_DETECTOR_ACTIVITYCLASSIFIED_COLUMN_TIMEFRAME ", "\
	APP_INACTIVE_DETECTOR_ACTIVITYCLASSIFIED_COLUMN_CONTEXT_APP_INFO_ID ")" \
"SELECT " \
	APP_INACTIVE_DETECTOR_ACTIVITYCLASSIFIED_COLUMN_IS_ACTIVE "," \
	APP_INACTIVE_DETECTOR_ACTIVITYCLASSIFIED_COLUMN_TIMEFRAME ", "\
	APP_INACTIVE_DETECTOR_ACTIVITYCLASSIFIED_COLUMN_CONTEXT_APP_INFO_ID \
"FROM"\
	"(" \
		APP_INACTIVE_DETECTOR_VALUE_PLACEHOLDER_VALUES \
	") q"



#endif /* _CONTEXT_INACTIVE_DETECTOR_STORAGE_QUERIES_H_ */
