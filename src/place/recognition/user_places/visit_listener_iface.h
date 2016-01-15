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

#ifndef __CONTEXT_PLACE_STATUS_VISIT_LISTENER_IFACE_H__
#define __CONTEXT_PLACE_STATUS_VISIT_LISTENER_IFACE_H__

namespace ctx {

	class IVisitListener {

	public:
		virtual ~IVisitListener() {};
		virtual void on_visit_start() = 0;
		virtual void on_visit_end() = 0;

	};	/* class IVisitListener */

}	/* namespace ctx */

#endif /* __CONTEXT_PLACE_STATUS_VISIT_LISTENER_IFACE_H__ */
