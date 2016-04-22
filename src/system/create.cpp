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

#include <new>
#include <create.h>
#include "Battery.h"
#include "Charger.h"
#include "Gps.h"
#include "Psmode.h"
#include "Usb.h"

using namespace ctx;

/* TODO: This function will be changed into the following form:
   ContextProvider* create(const char *subject) */

SO_EXPORT bool create()
{
	registerProvider<BatteryStateProvider>(SUBJ_STATE_BATTERY, NULL);
	registerProvider<ChargerStateProvider>(SUBJ_STATE_CHARGER, NULL);
	registerProvider<GpsStateProvider>(SUBJ_STATE_GPS, NULL);
	registerProvider<PowerSaveModeProvider>(SUBJ_STATE_PSMODE, NULL);
	registerProvider<UsbStateProvider>(SUBJ_STATE_USB, NULL);

	return true;
}
