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

#ifndef _CONTEXT_PLACE_RECOGNITION_WIFI_LISTENER_IFACE_H_
#define _CONTEXT_PLACE_RECOGNITION_WIFI_LISTENER_IFACE_H_

#include "../facade/user_places_types.h"

namespace ctx {

	class IWifiListener {

	public:
		virtual ~IWifiListener() {};
		virtual void onWifiScan(ctx::MacEvent macEvent) = 0;

	};	/* IWifiListener */

}	/* namespace ctx */

#endif	/* End of _CONTEXT_PLACE_RECOGNITION_WIFI_LISTENER_IFACE_H_ */
