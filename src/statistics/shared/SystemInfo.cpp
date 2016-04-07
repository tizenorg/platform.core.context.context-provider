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

#include <glib.h>
#include <runtime_info.h>
#include <sound_manager.h>
#include <types_internal.h>
#include <SharedVars.h>
#include "SystemInfo.h"

#define CONNECTED		1
#define NOT_CONNECTED	0

bool ctx::system_info::getAudioJackState(int* state)
{
	int value = NOT_CONNECTED;
	int err = runtime_info_get_value_int(RUNTIME_INFO_KEY_AUDIO_JACK_STATUS, &value);
	IF_FAIL_RETURN(err == RUNTIME_INFO_ERROR_NONE, false);

	*state = (value == NOT_CONNECTED ? NOT_CONNECTED : CONNECTED);

	return true;
}

bool ctx::system_info::getVolume(int* systemVolume, int* mediaVolume)
{
	int err;

	err = sound_manager_get_volume(SOUND_TYPE_SYSTEM, systemVolume);
	IF_FAIL_RETURN(err == RUNTIME_INFO_ERROR_NONE, false);

	err = sound_manager_get_volume(SOUND_TYPE_MEDIA, mediaVolume);
	IF_FAIL_RETURN(err == RUNTIME_INFO_ERROR_NONE, false);

	return true;
}

bool ctx::system_info::getWifiBssid(std::string& bssid)
{
#if 0
	/* NOTE: This routine does not work, because the wifi API does not support multi-sessions in one process */
	int err;
	char *strBuf = NULL;
	wifi_ap_h ap = NULL;

	err = wifi_initialize();
	IF_FAIL_RETURN_TAG(err == WIFI_ERROR_NONE, false, _W, "wifi_initialize() failed (%d)", err);

	err = wifi_get_connected_ap(&ap);
	if (err != WIFI_ERROR_NONE) {
		_D("wifi_get_connected_ap() failed (%d)", err);
		wifi_deinitialize();
		return false;
	}

	wifi_ap_get_bssid(ap, &strBuf);
	bssid = (strBuf != NULL ? strBuf : "");
	g_free(strBuf);

	wifi_ap_destroy(ap);
	wifi_deinitialize();

	return !bssid.empty();
#endif
	bssid = ctx::SharedVars().get(ctx::SharedVars::WIFI_BSSID);
	return true;
}
