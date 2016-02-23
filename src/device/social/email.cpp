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

#include <gio/gio.h>
#include <email-api-etc.h>

#include <context_mgr.h>
#include "social_types.h"
#include "email.h"

GENERATE_PROVIDER_COMMON_IMPL(social_status_email);

ctx::social_status_email::social_status_email()
	: dbus_signal_id(-1)
	, __dbusWatcher(DBusType::SESSION)
{
}

ctx::social_status_email::~social_status_email()
{
}

bool ctx::social_status_email::is_supported()
{
	return get_system_info_bool("tizen.org/feature/network.telephony");
}

void ctx::social_status_email::submit_trigger_item()
{
	context_manager::register_trigger_item(SOCIAL_ST_SUBJ_EMAIL, OPS_SUBSCRIBE,
			"{"
				"\"Event\":{\"type\":\"string\",\"values\":[\"Received\",\"Sent\"]}"
			"}",
			NULL);
}

void ctx::social_status_email::onSignal(const char* sender, const char* path, const char* iface, const char* name, GVariant* param)
{
	gint sub_type = 0;
	gint gi1 = 0;
	const gchar *gc = NULL;
	gint gi2 = 0;
	gint gi3 = 0;

	g_variant_get(param, "(ii&sii)", &sub_type, &gi1, &gc, &gi2, &gi3);

	if (sub_type == NOTI_DOWNLOAD_FINISH) {
		//TODO: Check if this signal actually means that there are new mails
		_D("sub type: %d, gi1: %d, gc: %s, gi2: %d, gi3: %d", sub_type, gi1, gc, gi2, gi3);
		ctx::Json data_updated;
		data_updated.set(NULL, SOCIAL_ST_EVENT, SOCIAL_ST_RECEIVED);
		context_manager::publish(SOCIAL_ST_SUBJ_EMAIL, NULL, ERR_NONE, data_updated);

	} else if (sub_type == NOTI_SEND_FINISH) {
		_D("sub type: %d, gi1: %d, gc: %s, gi2: %d, gi3: %d", sub_type, gi1, gc, gi2, gi3);
		ctx::Json data_updated;
		data_updated.set(NULL, SOCIAL_ST_EVENT, SOCIAL_ST_SENT);
		context_manager::publish(SOCIAL_ST_SUBJ_EMAIL, NULL, ERR_NONE, data_updated);
	}
}


int ctx::social_status_email::subscribe()
{
	dbus_signal_id = __dbusWatcher.watch(NULL, NULL, "User.Email.NetworkStatus", "email", this);
	IF_FAIL_RETURN_TAG(dbus_signal_id >= 0, ERR_OPERATION_FAILED, _E, "Email dbus signal subscription failed");
	return ERR_NONE;
}


int ctx::social_status_email::unsubscribe()
{
	__dbusWatcher.unwatch(dbus_signal_id);
	return ERR_NONE;
}
