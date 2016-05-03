/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd.
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

#include <Util.h>
#include "Message.h"

#define MAX_ADDR_SIZE 20

using namespace ctx;

MessageEventProvider::MessageEventProvider() :
	BasicProvider(SUBJ_STATE_MESSAGE),
	__messageHandle(NULL)
{
}

MessageEventProvider::~MessageEventProvider()
{
}

void MessageEventProvider::getPrivilege(std::vector<const char*> &privilege)
{
	privilege.push_back(PRIV_MESSAGE);
}

bool MessageEventProvider::isSupported()
{
	return util::getSystemInfoBool("tizen.org/feature/network.telephony");
}

bool MessageEventProvider::unloadable()
{
	return false;
}

void MessageEventProvider::__updateCb(msg_handle_t handle, msg_struct_t msg, void* userData)
{
	MessageEventProvider *instance = static_cast<MessageEventProvider*>(userData);
	instance->__handleUpdate(msg);
}

void MessageEventProvider::__handleUpdate(msg_struct_t msg)
{
	int err;
	int type;
	char address[MAX_ADDR_SIZE];
	Json data;

	err = msg_get_int_value(msg, MSG_MESSAGE_TYPE_INT, &type);
	IF_FAIL_VOID_TAG(err == MSG_SUCCESS, _W, "Getting message type failed");

	err = msg_get_str_value(msg, MSG_MESSAGE_REPLY_ADDR_STR, address, MAX_ADDR_SIZE);
	IF_FAIL_VOID_TAG(err == MSG_SUCCESS, _W, "Getting reply address failed");

	switch (type) {
	case MSG_TYPE_SMS_CB :
	case MSG_TYPE_SMS_JAVACB :
	case MSG_TYPE_SMS_WAPPUSH :
	case MSG_TYPE_SMS_MWI :
	case MSG_TYPE_SMS_SYNCML :
	case MSG_TYPE_SMS_REJECT :
	case MSG_TYPE_SMS_ETWS_PRIMARY :
	case MSG_TYPE_SMS :
		data.set(NULL, KEY_TYPE, VAL_SMS);
		break;
	case MSG_TYPE_MMS_NOTI :
	case MSG_TYPE_MMS_JAVA :
	case MSG_TYPE_MMS :
		data.set(NULL, KEY_TYPE, VAL_MMS);
		break;
	default :
		_W("Unknown message type");
		return;
	}

	data.set(NULL, KEY_EVENT, VAL_RECEIVED);
	data.set(NULL, KEY_ADDRESS, address);

	publish(NULL, ERR_NONE, data);
}

bool MessageEventProvider::__setCallback()
{
	int err;

	err = msg_open_msg_handle(&__messageHandle);
	IF_FAIL_RETURN_TAG(err == MSG_SUCCESS, false, _E, "Handle creation failed");

	err = msg_reg_sms_message_callback(__messageHandle, __updateCb, 0, this);
	if (err != MSG_SUCCESS) {
		msg_close_msg_handle(&__messageHandle);
		_E("Setting SMS event callback failed");
		return false;
	}

	msg_reg_mms_conf_message_callback(__messageHandle, __updateCb, NULL, this);
	return true;
}

void MessageEventProvider::__unsetCallback()
{
	if (__messageHandle)
		msg_close_msg_handle(&__messageHandle);

	__messageHandle = NULL;
}

int MessageEventProvider::subscribe()
{
	bool ret = __setCallback();
	IF_FAIL_RETURN(ret, ERR_OPERATION_FAILED);
	return ERR_NONE;
}

int MessageEventProvider::unsubscribe()
{
	__unsetCallback();
	return ERR_NONE;
}
