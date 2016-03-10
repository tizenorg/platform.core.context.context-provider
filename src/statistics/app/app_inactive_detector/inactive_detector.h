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

#ifndef __CONTEXT_INACTIVE_DETECTOR_H__
#define __CONTEXT_INACTIVE_DETECTOR_H__

#include <string>
#include <json.h>
#include <vector>
#include "app_inactive_detector_types.h"
#include "timer_listener_iface.h"

namespace ctx {

	class inactive_detector : public timer_listener_iface {
		private:
			int timer_id;
		public:
			inactive_detector();
			~inactive_detector();

			int read(const char *subject,
				ctx::json option);

			int update_clusters();
			void on_cluster_update_finished(
				std::vector<app_t> *apps_classified,
				int error);
			ctx::json to_json(std::vector<app_t> *apps);
	};	/* class inactive_detector */

}	/* namespace ctx */


#endif /* __CONTEXT_INACTIVE_DETECTOR_H__ */