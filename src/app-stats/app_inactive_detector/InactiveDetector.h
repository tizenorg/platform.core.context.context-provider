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

#ifndef _CONTEXT_INACTIVE_DETECTOR_H_
#define _CONTEXT_INACTIVE_DETECTOR_H_

#include <string>
#include <Json.h>
#include <vector>
#include "AppInactiveDetectorTypes.h"
#include <TimerManager.h>

namespace ctx {

	class InactiveDetector : public ITimerListener {
	private:
		int __timerId;
		TimerManager __timerMgr;

	public:
		InactiveDetector();
		~InactiveDetector();

		int read(const char *subject, ctx::Json option);

		int updateClusters();
		void onClusterUpdateFinished(std::vector<AppInfo> *appsClassified, int error);
		ctx::Json toJson(std::vector<AppInfo> *apps);

		bool onTimerExpired(int timerId);
	};	/* class InactiveDetector */

}	/* namespace ctx */


#endif /* _CONTEXT_INACTIVE_DETECTOR_H_ */
