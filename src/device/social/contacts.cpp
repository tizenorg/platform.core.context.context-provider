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

#include <Json.h>
#include <context_mgr.h>
#include "social_types.h"
#include "contacts.h"

#define MY_PROFILE_VIEW _contacts_my_profile._uri
#define PERSON_VIEW _contacts_person._uri
#define TIME_INTERVAL 1

GENERATE_PROVIDER_COMMON_IMPL(social_status_contacts);

ctx::social_status_contacts::social_status_contacts()
	: latest_my_profile(0)
	, latest_person(0)
{
}

ctx::social_status_contacts::~social_status_contacts()
{
}

bool ctx::social_status_contacts::is_supported()
{
	return true;
}

void ctx::social_status_contacts::submit_trigger_item()
{
	context_manager::register_trigger_item(SOCIAL_ST_SUBJ_CONTACTS, OPS_SUBSCRIBE,
			"{"
				"\"View\":{\"type\":\"string\",\"values\":[\"MyProfile\",\"Person\"]}"
			"}",
			NULL);
}

void ctx::social_status_contacts::db_change_cb(const char* view_uri, void* user_data)
{
	social_status_contacts *instance = static_cast<social_status_contacts*>(user_data);
	instance->handle_db_change(view_uri);
}

void ctx::social_status_contacts::handle_db_change(const char* view_uri)
{
	if (!STR_EQ(view_uri, _contacts_my_profile._uri) && !STR_EQ(view_uri, _contacts_person._uri)) {
		_W("Unknown view uri");
		return;
	}

	std::string view = (STR_EQ(view_uri, _contacts_my_profile._uri)? SOCIAL_ST_MY_PROFILE : SOCIAL_ST_PERSON);
	IF_FAIL_VOID_TAG(!is_consecutive_change(view_uri), _D, "Ignore consecutive db change: %s", view.c_str());

	ctx::Json data;
	data.set(NULL, SOCIAL_ST_VIEW, view);
	context_manager::publish(SOCIAL_ST_SUBJ_CONTACTS, NULL, ERR_NONE, data);
}

bool ctx::social_status_contacts::is_consecutive_change(const char* view_uri)
{
	time_t now = time(NULL);
	double diff = 0;

	if (STR_EQ(view_uri, MY_PROFILE_VIEW)) {
		diff = difftime(now, latest_my_profile);
		latest_my_profile = now;
	} else if (STR_EQ(view_uri, PERSON_VIEW)) {
		diff = difftime(now, latest_person);
		latest_person = now;
	}

	if (diff < TIME_INTERVAL)
		return true;

	return false;
}

bool ctx::social_status_contacts::set_callback()
{
	int err;

	err = contacts_connect();
	IF_FAIL_RETURN_TAG(err == CONTACTS_ERROR_NONE, false, _E, "Connecting contacts failed");

	err = contacts_db_add_changed_cb(MY_PROFILE_VIEW, db_change_cb, this);
	IF_FAIL_CATCH_TAG(err == CONTACTS_ERROR_NONE, _E, "Setting my profile view changed callback failed");

	err = contacts_db_add_changed_cb(PERSON_VIEW, db_change_cb, this);
	IF_FAIL_CATCH_TAG(err == CONTACTS_ERROR_NONE, _E, "Setting person view changed callback failed");

	return true;

CATCH:
	contacts_disconnect();
	return false;
}

void ctx::social_status_contacts::unset_callback()
{
	contacts_db_remove_changed_cb(MY_PROFILE_VIEW, db_change_cb, this);
	contacts_db_remove_changed_cb(PERSON_VIEW, db_change_cb, this);

	contacts_disconnect();

	latest_my_profile = 0;
	latest_person = 0;
}

int ctx::social_status_contacts::subscribe()
{
	bool ret = set_callback();
	IF_FAIL_RETURN(ret, ERR_OPERATION_FAILED);
	return ERR_NONE;
}

int ctx::social_status_contacts::unsubscribe()
{
	unset_callback();
	return ERR_NONE;
}
