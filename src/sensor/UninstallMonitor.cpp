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

#include "ClientInfo.h"
#include "UninstallMonitor.h"

using namespace ctx;

UninstallMonitor::UninstallMonitor() :
	__dbusSignalId(-1),
	__dbusWatcher(DBusType::SYSTEM)
{
	__dbusSignalId = __dbusWatcher.watch(NULL,
			"/org/tizen/pkgmgr/signal", "org.tizen.pkgmgr.signal", "uninstall", this);
}

UninstallMonitor::~UninstallMonitor()
{
	if (__dbusSignalId > 0)
		__dbusWatcher.unwatch(__dbusSignalId);
}

void UninstallMonitor::onSignal(const char *sender, const char *path, const char *iface, const char *name, GVariant *param)
{
	const gchar *pkgId = NULL;
	const gchar *key = NULL;
	const gchar *val = NULL;

	g_variant_get(param, "(u&s&s&s&s&s&s)", NULL, NULL, NULL, &pkgId, NULL, &key, &val);
	_D("%s, %s, %s", pkgId, key, val);

	IF_FAIL_VOID(pkgId && STR_EQ(key, "end") && STR_EQ(val, "ok"));

	_I("'%s' has been removed", pkgId);
	ClientInfo::purgeClient(pkgId);
}
