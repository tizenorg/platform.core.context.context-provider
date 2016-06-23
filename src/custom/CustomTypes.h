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

#ifndef _CONTEXT_CUSTOM_TYPES_H_
#define _CONTEXT_CUSTOM_TYPES_H_

#include <ProviderTypes.h>

#define CUSTOM_KEY_REQ				"req"
#define CUSTOM_KEY_PACKAGE_ID		"packageId"
#define CUSTOM_KEY_NAME				"name"
#define CUSTOM_KEY_ATTRIBUTES		"attributes"
#define CUSTOM_KEY_FACT				"fact"
#define CUSTOM_KEY_SUBJECT			"subject"
#define CUSTOM_KEY_OWNER			"owner"

#define CUSTOM_REQ_TYPE_ADD			"add"
#define CUSTOM_REQ_TYPE_REMOVE		"remove"
#define CUSTOM_REQ_TYPE_PUBLISH		"publish"

#define CUSTOM_SEPERATOR			"/"

#define CUSTOM_TEMPLATE_TABLE_SCHEMA \
	"CREATE TABLE IF NOT EXISTS ContextTriggerCustomTemplate " \
	"(subject TEXT DEFAULT '' NOT NULL PRIMARY KEY, name TEXT DEFAULT '' NOT NULL," \
	" operation INTEGER DEFAULT 3 NOT NULL, attributes TEXT DEFAULT '' NOT NULL, " \
	" owner TEXT DEFAULT '' NOT NULL)"


#endif	/* End of _CONTEXT_CUSTOM_TYPES_H_ */
