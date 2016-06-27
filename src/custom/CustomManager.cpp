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
#include <Util.h>
#include <DatabaseManager.h>
#include "CustomManager.h"
#include "CustomProvider.h"
#include "CustomTypes.h"

using namespace ctx;

static std::map<std::string, CustomProvider*> __customMap;
static DatabaseManager __dbManager;

static bool __isValidFact(std::string subject, Json& fact);
static bool __checkValueInt(Json& tmpl, std::string key, int value);
static bool __checkValueString(Json& tmpl, std::string key, std::string value);

CustomManager::CustomManager() :
	BasicProvider(SUBJ_CUSTOM)
{
	__initialize();
}

CustomManager::~CustomManager()
{
	/* Custom provider instances will be deleted by Provider Handler */
	__customMap.clear();
}

bool CustomManager::isSupported()
{
	return true;
}

bool CustomManager::unloadable()
{
	return false;
}

int CustomManager::subscribe()
{
	return ERR_NONE;
}

int CustomManager::unsubscribe()
{
	return ERR_NONE;
}

int CustomManager::write(Json data, Json *requestResult)
{
	int error = ERR_NONE;
	std::string req;
	data.get(NULL, CUSTOM_KEY_REQ, &req);

	std::string packageId;
	std::string name;
	data.get(NULL, KEY_CLIENT_PKG_ID, &packageId);
	data.get(NULL, CUSTOM_KEY_NAME, &name);
	std::string subj = std::string(SUBJ_CUSTOM) + CUSTOM_SEPERATOR + packageId + CUSTOM_SEPERATOR + name;

	if (req == CUSTOM_REQ_TYPE_ADD) {
		Json tmpl;
		data.get(NULL, CUSTOM_KEY_ATTRIBUTES, &tmpl);

		error = __addCustomItem(subj, name, tmpl, packageId);
	} else if (req == CUSTOM_REQ_TYPE_REMOVE) {
		error = __removeCustomItem(subj);
		if (error == ERR_NONE) {
			requestResult->set(NULL, CUSTOM_KEY_SUBJECT, subj);
		}
	} else if (req == CUSTOM_REQ_TYPE_PUBLISH) {
		Json fact;
		data.get(NULL, CUSTOM_KEY_FACT, &fact);

		error = __publishData(subj, fact);
	}

	return error;
}

ContextProvider* CustomManager::getProvider(const char* subject)
{
	auto it = __customMap.find(subject);
	IF_FAIL_RETURN_TAG(it != __customMap.end(), NULL, _E, "'%s' not found", subject);

	CustomProvider* custom = static_cast<CustomProvider*>(it->second);
	return custom;
}

bool CustomManager::__initialize()
{
	/* Create custom template table */
	std::vector<Json> record;
	bool ret = __dbManager.executeSync(CUSTOM_TEMPLATE_TABLE_SCHEMA, &record);
	IF_FAIL_RETURN_TAG(ret, false, _E, "Create template table failed");

	/* Register custom items */
	std::string qSelect = "SELECT * FROM ContextTriggerCustomTemplate";
	ret = __dbManager.executeSync(qSelect.c_str(), &record);
	IF_FAIL_RETURN_TAG(ret, false, _E, "Failed to query custom templates");
	IF_FAIL_RETURN(record.size() > 0, true);

	int error;
	std::vector<Json>::iterator vedEnd = record.end();
	for (auto vecPos = record.begin(); vecPos != vedEnd; ++vecPos) {
		Json elem = *vecPos;
		std::string subject;
		std::string name;
		std::string attributes;
		std::string owner;
		elem.get(NULL, CUSTOM_KEY_SUBJECT, &subject);
		elem.get(NULL, CUSTOM_KEY_NAME, &name);
		elem.get(NULL, CUSTOM_KEY_ATTRIBUTES, &attributes);
		elem.get(NULL, CUSTOM_KEY_OWNER, &owner);

		error = __addCustomItem(subject.c_str(), name, Json(attributes), owner, true);
		if (error != ERR_NONE) {
			_E("Failed to add custom item(%s): %#x", subject.c_str(), error);
		}
	}

	return true;
}

int CustomManager::__addCustomItem(std::string subject, std::string name, Json tmpl, std::string owner, bool isInit)
{
	std::map<std::string, CustomProvider*>::iterator it;
	it = __customMap.find(subject);

	if (it != __customMap.end()) {
		if ((it->second)->getTemplate() != tmpl) {	/* Same item name, different template */
			return ERR_DATA_EXIST;
		}

		return ERR_NONE;	/* Same item */
	}

	/* Create custom provider */
	CustomProvider* custom = new(std::nothrow) CustomProvider(subject.c_str(), name, tmpl, owner);
	IF_FAIL_RETURN_TAG(custom, ERR_OUT_OF_MEMORY, _E, "Memory allocation failed");
	__customMap[subject] = custom;

	/* Register provider handler & template */
	registerCustomProvider(subject.c_str(), OPS_SUBSCRIBE | OPS_READ, tmpl, NULL, owner.c_str());

	/* Insert item to custom template db */
	if (!isInit) {
		std::string q = "INSERT OR IGNORE INTO ContextTriggerCustomTemplate (subject, name, attributes, owner) VALUES ('"
				+ subject + "', '" + name +  "', '" + tmpl.str() + "', '" + owner + "'); ";
		std::vector<Json> record;
		bool ret = __dbManager.executeSync(q.c_str(), &record);
		IF_FAIL_RETURN_TAG(ret, ERR_OPERATION_FAILED, _E, "Failed to query custom templates");
	}

	return ERR_NONE;
}

int CustomManager::__removeCustomItem(std::string subject)
{
	std::map<std::string, CustomProvider*>::iterator it;
	it = __customMap.find(subject);
	IF_FAIL_RETURN_TAG(it != __customMap.end(), ERR_NOT_SUPPORTED, _E, "%s not supported", subject.c_str());

	/* Unregister provider handler & template */
	unregisterCustomProvider(subject.c_str());
	__customMap.erase(it);

	/* Delete item from custom template db */
	std::string q = "DELETE FROM ContextTriggerCustomTemplate WHERE subject = '" + subject + "'";
	std::vector<Json> record;
	bool ret = __dbManager.executeSync(q.c_str(), &record);
	IF_FAIL_RETURN_TAG(ret, ERR_OPERATION_FAILED, _E, "Failed to query custom templates");

	return ERR_NONE;
}

int CustomManager::__publishData(std::string subject, Json fact)
{
	std::map<std::string, CustomProvider*>::iterator it;
	it = __customMap.find(subject);
	IF_FAIL_RETURN_TAG(it != __customMap.end(), ERR_NOT_SUPPORTED, _E, "%s not supported", subject.c_str());

	bool ret = __isValidFact(subject, fact);
	IF_FAIL_RETURN_TAG(ret, ERR_INVALID_DATA, _E, "Invalid fact(%s)", subject.c_str());

	__customMap[subject]->handleUpdate(fact);
	return ERR_NONE;
}

bool __isValidFact(std::string subject, Json& fact)
{
	Json tmpl = __customMap[subject]->getTemplate();
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

bool __checkValueInt(Json& tmpl, std::string key, int value)
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

bool __checkValueString(Json& tmpl, std::string key, std::string value)
{
	/* case1: any value is accepted */
	if (tmpl.getSize(key.c_str(), "values") <= 0)
		return true;

	/* case2: check acceptable value */
	std::string tmplValue;
	for (int i = 0; tmpl.getAt(key.c_str(), "values", i, &tmplValue); i++) {
		if (tmplValue == value)
			return true;
	}

	return false;
}
