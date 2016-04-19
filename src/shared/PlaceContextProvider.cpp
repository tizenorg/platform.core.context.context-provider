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

#include <new>
#include <Types.h>
#include <PlaceContextProvider.h>

#ifdef _MOBILE_
#include "../geofence/PlaceGeofenceProvider.h"
#include "../my-place/place_recognition.h"
#endif	/* _MOBILE_ */

template<typename Provider>
void registerProvider(const char *subject, const char *privilege)
{
	Provider *provider = new(std::nothrow) Provider();
	IF_FAIL_VOID_TAG(provider, _E, "Memory allocation failed");

	if (!provider->isSupported()) {
		delete provider;
		return;
	}

	provider->registerProvider(privilege, provider);
	provider->submitTriggerItem();
}

SO_EXPORT bool ctx::initPlaceContextProvider()
{
#ifdef _MOBILE_
	registerProvider<PlaceGeofenceProvider>(PLACE_SUBJ_GEOFENCE, PLACE_PRIV_GEOFENCE);

	/*
	registerProvider<PlaceRecognitionProvider>(PLACE_SUBJ_RECOGNITION, PLACE_PRIV_RECOGNITION);
	*/

#endif	/* _MOBILE_ */
	return true;
}
