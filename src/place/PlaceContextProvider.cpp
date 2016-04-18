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

#include <Types.h>
#include <ContextManager.h>
#include <ContextProviderBase.h>
#include <PlaceContextProvider.h>

#ifdef _MOBILE_
#include "geofence/PlaceGeofenceProvider.h"
#ifndef _DISABLE_RECOG_ENGINE_
#include "recognition/place_recognition.h"
#endif	/* _DISABLE_RECOG_ENGINE_ */
#endif	/* _MOBILE_ */

template<typename Provider>
void registerProvider(const char *subject, const char *privilege)
{
	if (!Provider::isSupported())
		return;

	ctx::ContextProviderInfo providerInfo(Provider::create, Provider::destroy, NULL, privilege);
	ctx::context_manager::registerProvider(subject, providerInfo);
}

SO_EXPORT bool ctx::initPlaceContextProvider()
{
#ifdef _MOBILE_
	registerProvider<PlaceGeofenceProvider>(PLACE_SUBJ_GEOFENCE, PLACE_PRIV_GEOFENCE);
	PlaceGeofenceProvider::submitTriggerItem();

#ifndef _DISABLE_RECOG_ENGINE_
	PlaceRecognitionProvider::create(NULL);
	registerProvider<PlaceRecognitionProvider>(PLACE_SUBJ_RECOGNITION, PLACE_PRIV_RECOGNITION);
#endif	/* _DISABLE_RECOG_ENGINE_ */

#endif	/* _MOBILE_ */
	return true;
}
