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

#include <email-api-etc.h>
#include <Util.h>
#include "Email.h"

using namespace ctx;

EmailEventProvider::EmailEventProvider()	:
	BasicProvider(SUBJ_STATE_EMAIL),
	__dbusSignalId(-1),
	__dbusWatcher(DBusType::SESSION)
{
}

EmailEventProvider::~EmailEventProvider()
{
}

bool EmailEventProvider::isSupported()
{
	return util::getSystemInfoBool("tizen.org/feature/network.telephony");
}

void EmailEventProvider::onSignal(const char* sender, const char* path, const char* iface, const char* name, GVariant* param)
{
	gint subType = 0;
	gint gi1 = 0;
	const gchar *gc = NULL;
	gint gi2 = 0;
	gint gi3 = 0;

	g_variant_get(param, "(ii&sii)", &subType, &gi1, &gc, &gi2, &gi3);

	if (subType == NOTI_DOWNLOAD_FINISH) {
		//TODO: Check if this signal actually means that there are new mails
		_D("sub type: %d, gi1: %d, gc: %s, gi2: %d, gi3: %d", subType, gi1, gc, gi2, gi3);
		Json dataUpdated;
		dataUpdated.set(NULL, KEY_EVENT, VAL_RECEIVED);
		publish(NULL, ERR_NONE, dataUpdated);

	} else if (subType == NOTI_SEND_FINISH) {
		_D("sub type: %d, gi1: %d, gc: %s, gi2: %d, gi3: %d", subType, gi1, gc, gi2, gi3);
		Json dataUpdated;
		dataUpdated.set(NULL, KEY_EVENT, VAL_SENT);
		publish(NULL, ERR_NONE, dataUpdated);
	}
}


int EmailEventProvider::subscribe()
{
	__dbusSignalId = __dbusWatcher.watch(NULL, NULL, "User.Email.NetworkStatus", "email", this);
	IF_FAIL_RETURN_TAG(__dbusSignalId >= 0, ERR_OPERATION_FAILED, _E, "Email dbus signal subscription failed");
	return ERR_NONE;
}


int EmailEventProvider::unsubscribe()
{
	__dbusWatcher.unwatch(__dbusSignalId);
	return ERR_NONE;
}
