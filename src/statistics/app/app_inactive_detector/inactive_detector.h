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
#include <Json.h>
#include <vector>
#include "app_inactive_detector_types.h"
#include <TimerManager.h>

namespace ctx {

	class inactive_detector : public ITimerListener {
		private:
			int timer_id;
			TimerManager timer_mgr;
		public:
			inactive_detector();
			~inactive_detector();

			int read(const char *subject,
				ctx::Json option);

			int update_clusters();
			void on_cluster_update_finished(
				std::vector<app_t> *apps_classified,
				int error);
			ctx::Json to_json(std::vector<app_t> *apps);

			bool onTimerExpired(int timerId);
	};	/* class inactive_detector */

}	/* namespace ctx */


#endif /* __CONTEXT_INACTIVE_DETECTOR_H__ */
