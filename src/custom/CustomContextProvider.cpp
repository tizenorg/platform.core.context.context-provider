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

#include <map>
#include <vector>
#include <types_internal.h>
#include <ContextManager.h>
#include <ContextProviderBase.h>
#include <DatabaseManager.h>
#include <CustomContextProvider.h>
#include "CustomBase.h"

static std::map<std::string, ctx::CustomBase*> __customMap;
static ctx::DatabaseManager __dbManager;

static bool __isValidFact(std::string subject, ctx::Json& fact);
static bool __checkValueInt(ctx::Json& tmpl, std::string key, int value);
static bool __checkValueString(ctx::Json& tmpl, std::string key, std::string value);

void registerProvider(const char *subject, const char *privilege)
{
	ctx::ContextProviderInfo providerInfo(ctx::custom_context_provider::create,
											ctx::custom_context_provider::destroy,
											const_cast<char*>(subject), privilege);
	ctx::context_manager::registerProvider(subject, providerInfo);
	__customMap[subject]->submitTriggerItem();
}

void unregisterProvider(const char* subject)
{
	__customMap[subject]->unsubmitTriggerItem();
	ctx::context_manager::unregisterProvider(subject);
}

EXTAPI ctx::ContextProviderBase* ctx::custom_context_provider::create(void *data)
{
	// Already created in addItem() function. Return corresponding custom provider
	return __customMap[static_cast<const char*>(data)];
}

EXTAPI void ctx::custom_context_provider::destroy(void *data)
{
	std::map<std::string, ctx::CustomBase*>::iterator it = __customMap.find(static_cast<char*>(data));
	if (it != __customMap.end()) {
		delete it->second;
		__customMap.erase(it);
	}
}

EXTAPI bool ctx::initCustomContextProvider()
{
	// Create custom template db
	std::string q = std::string("CREATE TABLE IF NOT EXISTS context_trigger_custom_template ")
			+ "(subject TEXT DEFAULT '' NOT NULL PRIMARY KEY, name TEXT DEFAULT '' NOT NULL, operation INTEGER DEFAULT 3 NOT NULL, "
			+ "attributes TEXT DEFAULT '' NOT NULL, owner TEXT DEFAULT '' NOT NULL)";

	std::vector<Json> record;
	bool ret = __dbManager.executeSync(q.c_str(), &record);
	IF_FAIL_RETURN_TAG(ret, false, _E, "Create template table failed");

	// Register custom items
	std::string qSelect = "SELECT * FROM context_trigger_custom_template";
	ret = __dbManager.executeSync(qSelect.c_str(), &record);
	IF_FAIL_RETURN_TAG(ret, false, _E, "Failed to query custom templates");
	IF_FAIL_RETURN(record.size() > 0, true);

	int error;
	std::vector<Json>::iterator vedEnd = record.end();
	for (auto vecPos = record.begin(); vecPos != vedEnd; ++vecPos) {
		ctx::Json elem = *vecPos;
		std::string subject;
		std::string name;
		std::string attributes;
		std::string owner;
		elem.get(NULL, "subject", &subject);
		elem.get(NULL, "name", &name);
		elem.get(NULL, "attributes", &attributes);
		elem.get(NULL, "owner", &owner);

		error = ctx::custom_context_provider::addItem(subject, name, ctx::Json(attributes), owner.c_str(), true);
		if (error != ERR_NONE) {
			_E("Failed to add custom item(%s): %#x", subject.c_str(), error);
		}
	}

	return true;
}

EXTAPI int ctx::custom_context_provider::addItem(std::string subject, std::string name, ctx::Json tmpl, const char* owner, bool isInit)
{
	std::map<std::string, ctx::CustomBase*>::iterator it;
	it = __customMap.find(subject);

	if (it != __customMap.end()) {
		if ((it->second)->getTemplate() != tmpl) {	// Same item name, but different template
			return ERR_DATA_EXIST;
		}
		// Same item name with same template
		return ERR_NONE;
	}

	// Create custom base
	ctx::CustomBase* custom = new(std::nothrow) CustomBase(subject, name, tmpl, owner);
	IF_FAIL_RETURN_TAG(custom, ERR_OUT_OF_MEMORY, _E, "Memory allocation failed");
	__customMap[subject] = custom;

	registerProvider(custom->getSubject(), NULL);

	// Add item to custom template db
	if (!isInit) {
		std::string q = "INSERT OR IGNORE INTO context_trigger_custom_template (subject, name, attributes, owner) VALUES ('"
				+ subject + "', '" + name +  "', '" + tmpl.str() + "', '" + owner + "'); ";
		std::vector<Json> record;
		bool ret = __dbManager.executeSync(q.c_str(), &record);
		IF_FAIL_RETURN_TAG(ret, false, _E, "Failed to query custom templates");
	}

	return ERR_NONE;
}

EXTAPI int ctx::custom_context_provider::removeItem(std::string subject)
{
	std::map<std::string, ctx::CustomBase*>::iterator it;
	it = __customMap.find(subject);
	IF_FAIL_RETURN_TAG(it != __customMap.end(), ERR_NOT_SUPPORTED, _E, "%s not supported", subject.c_str());

	unregisterProvider(subject.c_str());

	// Remove item from custom template db
	std::string q = "DELETE FROM context_trigger_custom_template WHERE subject = '" + subject + "'";
	std::vector<Json> record;
	bool ret = __dbManager.executeSync(q.c_str(), &record);
	IF_FAIL_RETURN_TAG(ret, false, _E, "Failed to query custom templates");

	return ERR_NONE;
}

EXTAPI int ctx::custom_context_provider::publishData(std::string subject, ctx::Json fact)
{
	std::map<std::string, ctx::CustomBase*>::iterator it;
	it = __customMap.find(subject);
	IF_FAIL_RETURN_TAG(it != __customMap.end(), ERR_NOT_SUPPORTED, _E, "%s not supported", subject.c_str());

	bool ret = __isValidFact(subject, fact);
	IF_FAIL_RETURN_TAG(ret, ERR_INVALID_DATA, _E, "Invalid fact(%s)", subject.c_str());

	__customMap[subject]->handleUpdate(fact);
	return ERR_NONE;
}

bool __isValidFact(std::string subject, ctx::Json& fact)
{
	ctx::Json tmpl = __customMap[subject]->getTemplate();
	IF_FAIL_RETURN_TAG(tmpl != EMPTY_JSON_OBJECT, false, _E, "Failed to get template");

	bool ret;
	std::list<std::string> keys;
	fact.getKeys(&keys);

	for (std::list<std::string>::iterator it = keys.begin(); it != keys.end(); it++) {
		std::string key = *it;

		std::string type;
		tmpl.get(key.c_str(), "type", &type);
		if (type == "integer") {
			int val;
			ret = fact.get(NULL, key.c_str(), &val);
			IF_FAIL_RETURN_TAG(ret, false, _E, "Custom fact: invalid data type");

			ret = __checkValueInt(tmpl, key, val);
			IF_FAIL_RETURN_TAG(ret, false, _E, "Custom fact: invalid value");
		} else if (type == "string") {
			std::string val_str;
			ret = fact.get(NULL, key.c_str(), &val_str);
			IF_FAIL_RETURN_TAG(ret, false, _E, "Custom fact: invalid data type");

			ret = __checkValueString(tmpl, key, val_str);
			IF_FAIL_RETURN_TAG(ret, false, _E, "Custom fact: invalid value");
		} else {
			_E("Custom fact: invalid data type");
			return false;
		}
	}

	return true;
}

bool __checkValueInt(ctx::Json& tmpl, std::string key, int value)
{
	int min, max;

	if (tmpl.get(key.c_str(), "min", &min)) {
		IF_FAIL_RETURN(value >= min, false);
	}

	if (tmpl.get(key.c_str(), "max", &max)) {
		IF_FAIL_RETURN(value <= max, false);
	}

	return true;
}

bool __checkValueString(ctx::Json& tmpl, std::string key, std::string value)
{
	// case1: any value is accepted
	if (tmpl.getSize(key.c_str(), "values") <= 0)
		return true;

	// case2: check acceptable value
	std::string tmplValue;
	for (int i = 0; tmpl.getAt(key.c_str(), "values", i, &tmplValue); i++) {
		if (tmplValue == value)
			return true;
	}

	return false;
}


