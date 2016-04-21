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

#include "Contacts.h"

#define MY_PROFILE_VIEW _contacts_my_profile._uri
#define PERSON_VIEW _contacts_person._uri
#define TIME_INTERVAL 1

using namespace ctx;

SocialStatusContacts::SocialStatusContacts() :
	BasicProvider(SOCIAL_ST_SUBJ_CONTACTS),
	__latestMyProfile(0),
	__latestPerson(0)
{
}

SocialStatusContacts::~SocialStatusContacts()
{
}

bool SocialStatusContacts::isSupported()
{
	return true;
}

void SocialStatusContacts::submitTriggerItem()
{
	registerTriggerItem(OPS_SUBSCRIBE,
			"{"
				"\"Event\":{\"type\":\"string\",\"values\":[\"Changed\"]},"
				"\"Type\":{\"type\":\"string\",\"values\":[\"MyProfile\",\"Person\"]}"
			"}",
			NULL);
}

void SocialStatusContacts::__updateCb(const char* viewUri, void* userData)
{
	SocialStatusContacts *instance = static_cast<SocialStatusContacts*>(userData);
	instance->__handleUpdate(viewUri);
}

void SocialStatusContacts::__handleUpdate(const char* viewUri)
{
	if (!STR_EQ(viewUri, _contacts_my_profile._uri) && !STR_EQ(viewUri, _contacts_person._uri)) {
		_W("Unknown view uri");
		return;
	}

	std::string view = (STR_EQ(viewUri, _contacts_my_profile._uri)? SOCIAL_ST_MY_PROFILE : SOCIAL_ST_PERSON);
	IF_FAIL_VOID_TAG(!__isConsecutiveChange(viewUri), _D, "Ignore consecutive db change: %s", view.c_str());

	Json data;
	data.set(NULL, SOCIAL_ST_EVENT, SOCIAL_ST_CHANGED);
	data.set(NULL, SOCIAL_ST_TYPE, view);
	publish(NULL, ERR_NONE, data);
}

bool SocialStatusContacts::__isConsecutiveChange(const char* viewUri)
{
	time_t now = time(NULL);
	double diff = 0;

	if (STR_EQ(viewUri, MY_PROFILE_VIEW)) {
		diff = difftime(now, __latestMyProfile);
		__latestMyProfile = now;
	} else if (STR_EQ(viewUri, PERSON_VIEW)) {
		diff = difftime(now, __latestPerson);
		__latestPerson = now;
	}

	if (diff < TIME_INTERVAL)
		return true;

	return false;
}

bool SocialStatusContacts::__setCallback()
{
	int err;

	err = contacts_connect();
	IF_FAIL_RETURN_TAG(err == CONTACTS_ERROR_NONE, false, _E, "Connecting contacts failed");

	err = contacts_db_add_changed_cb(MY_PROFILE_VIEW, __updateCb, this);
	IF_FAIL_CATCH_TAG(err == CONTACTS_ERROR_NONE, _E, "Setting my profile view changed callback failed");

	err = contacts_db_add_changed_cb(PERSON_VIEW, __updateCb, this);
	IF_FAIL_CATCH_TAG(err == CONTACTS_ERROR_NONE, _E, "Setting person view changed callback failed");

	return true;

CATCH:
	contacts_disconnect();
	return false;
}

void SocialStatusContacts::__unsetCallback()
{
	contacts_db_remove_changed_cb(MY_PROFILE_VIEW, __updateCb, this);
	contacts_db_remove_changed_cb(PERSON_VIEW, __updateCb, this);

	contacts_disconnect();

	__latestMyProfile = 0;
	__latestPerson = 0;
}

int SocialStatusContacts::subscribe()
{
	bool ret = __setCallback();
	IF_FAIL_RETURN(ret, ERR_OPERATION_FAILED);
	return ERR_NONE;
}

int SocialStatusContacts::unsubscribe()
{
	__unsetCallback();
	return ERR_NONE;
}
