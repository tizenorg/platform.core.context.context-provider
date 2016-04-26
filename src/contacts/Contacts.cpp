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

ContactsChangeProvider::ContactsChangeProvider() :
	BasicProvider(SUBJ_STATE_CONTACTS),
	__latestMyProfile(0),
	__latestPerson(0)
{
}

ContactsChangeProvider::~ContactsChangeProvider()
{
}

bool ContactsChangeProvider::isSupported()
{
	return true;
}

void ContactsChangeProvider::__updateCb(const char* viewUri, void* userData)
{
	ContactsChangeProvider *instance = static_cast<ContactsChangeProvider*>(userData);
	instance->__handleUpdate(viewUri);
}

void ContactsChangeProvider::__handleUpdate(const char* viewUri)
{
	if (!STR_EQ(viewUri, _contacts_my_profile._uri) && !STR_EQ(viewUri, _contacts_person._uri)) {
		_W("Unknown view uri");
		return;
	}

	std::string view = (STR_EQ(viewUri, _contacts_my_profile._uri)? VAL_MY_PROFILE : VAL_PERSON);
	IF_FAIL_VOID_TAG(!__isConsecutiveChange(viewUri), _D, "Ignore consecutive db change: %s", view.c_str());

	Json data;
	data.set(NULL, KEY_EVENT, VAL_CHANGED);
	data.set(NULL, KEY_TYPE, view);
	publish(NULL, ERR_NONE, data);
}

bool ContactsChangeProvider::__isConsecutiveChange(const char* viewUri)
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

bool ContactsChangeProvider::__setCallback()
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

void ContactsChangeProvider::__unsetCallback()
{
	contacts_db_remove_changed_cb(MY_PROFILE_VIEW, __updateCb, this);
	contacts_db_remove_changed_cb(PERSON_VIEW, __updateCb, this);

	contacts_disconnect();

	__latestMyProfile = 0;
	__latestPerson = 0;
}

int ContactsChangeProvider::subscribe()
{
	bool ret = __setCallback();
	IF_FAIL_RETURN(ret, ERR_OPERATION_FAILED);
	return ERR_NONE;
}

int ContactsChangeProvider::unsubscribe()
{
	__unsetCallback();
	return ERR_NONE;
}
