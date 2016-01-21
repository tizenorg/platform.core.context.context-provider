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
#include <types_internal.h>
#include <context_mgr.h>
#include <provider_iface.h>
#include <custom_context_provider.h>
#include "custom_base.h"

std::map<std::string, ctx::custom_base*> custom_map;

static bool is_valid_fact(std::string subject, ctx::json& fact);
static bool check_value_int(ctx::json& tmpl, std::string key, int value);
static bool check_value_string(ctx::json& tmpl, std::string key, std::string value);

void register_provider(const char *subject, const char *privilege)
{
	ctx::context_provider_info provider_info(ctx::custom_context_provider::create,
											ctx::custom_context_provider::destroy,
											const_cast<char*>(subject), privilege);
	ctx::context_manager::register_provider(subject, provider_info);
	custom_map[subject]->submit_trigger_item();
}

void unregister_provider(const char* subject)
{
	custom_map[subject]->unsubmit_trigger_item();
	ctx::context_manager::unregister_provider(subject);
}

EXTAPI ctx::context_provider_iface* ctx::custom_context_provider::create(void *data)
{
	// Already created in add_item() function. Return corresponding custom provider
	return custom_map[static_cast<const char*>(data)];
}

EXTAPI void ctx::custom_context_provider::destroy(void *data)
{
	std::map<std::string, ctx::custom_base*>::iterator it = custom_map.find(static_cast<char*>(data));
	if (it != custom_map.end()) {
		delete it->second;
		custom_map.erase(it);
	}
}

EXTAPI bool ctx::init_custom_context_provider()
{
	return true;
}

EXTAPI int ctx::custom_context_provider::add_item(std::string subject, std::string name, ctx::json tmpl, const char* owner)
{
	std::map<std::string, ctx::custom_base*>::iterator it;
	it = custom_map.find(subject);

	if (it != custom_map.end()) {
		if ((it->second)->get_template() != tmpl) {	// Same item name, but different template
			return ERR_DATA_EXIST;
		}
		// Same item name with same template
		return ERR_NONE;
	}

	// Create custom base
	ctx::custom_base* custom = new(std::nothrow) custom_base(subject, name, tmpl, owner);
	IF_FAIL_RETURN_TAG(custom, ERR_OUT_OF_MEMORY, _E, "Memory allocation failed");
	custom_map[subject] = custom;

	register_provider(custom->get_subject(), NULL);

	return ERR_NONE;
}

EXTAPI int ctx::custom_context_provider::remove_item(std::string subject)
{
	std::map<std::string, ctx::custom_base*>::iterator it;
	it = custom_map.find(subject);
	IF_FAIL_RETURN_TAG(it != custom_map.end(), ERR_NOT_SUPPORTED, _E, "%s not supported", subject.c_str());

	unregister_provider(subject.c_str());

	return ERR_NONE;
}

EXTAPI int ctx::custom_context_provider::publish_data(std::string subject, ctx::json fact)
{
	std::map<std::string, ctx::custom_base*>::iterator it;
	it = custom_map.find(subject);
	IF_FAIL_RETURN_TAG(it != custom_map.end(), ERR_NOT_SUPPORTED, _E, "%s not supported", subject.c_str());

	bool ret = is_valid_fact(subject, fact);
	IF_FAIL_RETURN_TAG(ret, ERR_INVALID_DATA, _E, "Invalid fact(%s)", subject.c_str());

	custom_map[subject]->handle_update(fact);
	return ERR_NONE;
}

bool is_valid_fact(std::string subject, ctx::json& fact)
{
	ctx::json tmpl = custom_map[subject]->get_template();
	IF_FAIL_RETURN_TAG(tmpl != EMPTY_JSON_OBJECT, false, _E, "Failed to get template");

	bool ret;
	std::list<std::string> keys;
	fact.get_keys(&keys);

	for (std::list<std::string>::iterator it = keys.begin(); it != keys.end(); it++) {
		std::string key = *it;

		std::string type;
		tmpl.get(key.c_str(), "type", &type);
		if (type == "integer") {
			int val;
			ret = fact.get(NULL, key.c_str(), &val);
			IF_FAIL_RETURN_TAG(ret, false, _E, "Custom fact: invalid data type");

			ret = check_value_int(tmpl, key, val);
			IF_FAIL_RETURN_TAG(ret, false, _E, "Custom fact: invalid value");
		} else if (type == "string") {
			std::string val_str;
			ret = fact.get(NULL, key.c_str(), &val_str);
			IF_FAIL_RETURN_TAG(ret, false, _E, "Custom fact: invalid data type");

			ret = check_value_string(tmpl, key, val_str);
			IF_FAIL_RETURN_TAG(ret, false, _E, "Custom fact: invalid value");
		} else {
			_E("Custom fact: invalid data type");
			return false;
		}
	}

	return true;
}

bool check_value_int(ctx::json& tmpl, std::string key, int value)
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

bool check_value_string(ctx::json& tmpl, std::string key, std::string value)
{
	// case1: any value is accepted
	if (tmpl.array_get_size(key.c_str(), "values") <= 0)
		return true;

	// case2: check acceptable value
	std::string t_val;
	for (int i = 0; tmpl.get_array_elem(key.c_str(), "values", i, &t_val); i++) {
		if (t_val == value)
			return true;
	}

	return false;
}


