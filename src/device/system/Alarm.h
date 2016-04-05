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

#ifndef _DEVICE_SYSTEM_STATUS_ALARM_H_
#define _DEVICE_SYSTEM_STATUS_ALARM_H_

#include <map>
#include <set>
#include <ContextProviderBase.h>
#include <TimerManager.h>
#include "../DeviceProviderBase.h"

namespace ctx {

	class DeviceStatusAlarm : public ContextProviderBase, ITimerListener {

		GENERATE_PROVIDER_COMMON_DECL(DeviceStatusAlarm);

	public:
		int subscribe(const char *subject, ctx::Json option, ctx::Json *requestResult);
		int unsubscribe(const char *subject, ctx::Json option);
		int read(const char *subject, ctx::Json option, ctx::Json *requestResult);
		int write(const char *subject, ctx::Json data, ctx::Json *requestResult);

		int subscribe(ctx::Json option);
		int unsubscribe(ctx::Json option);
		static bool isSupported();
		static void submitTriggerItem();

	protected:
		bool onTimerExpired(int timerId);

	private:
		DeviceStatusAlarm();
		~DeviceStatusAlarm();

		struct RefCountArray {
			int count[7];	/* reference counts for days of week*/
			RefCountArray();
		};

		struct TimerState {
			int timerId;
			int dayOfWeek; /* day of week, merged into one integer */
			TimerState() : timerId(-1), dayOfWeek(0) {}
		};

		typedef std::map<int, RefCountArray> RefCountMap;
		typedef std::map<int, TimerState> TimerStateMap;
		typedef std::set<ctx::Json*> OptionSet;

		RefCountMap __refCountMap;
		TimerStateMap __timerStateMap;
		OptionSet __optionSet;
		TimerManager __timerManager;

		bool __add(int minute, int dayOfWeek);
		bool __remove(int minute, int dayOfWeek);
		bool __resetTimer(int hour);
		void __clear();
		void __handleUpdate(int hour, int min, int dayOfWeek);

		int __getArrangedDayOfWeek(ctx::Json& option);
		int __mergeDayOfWeek(int *refCnt);

		bool __isMatched(ctx::Json& option, int time, std::string day);
		OptionSet::iterator __findOption(ctx::Json& option);

		void __destroyIfUnused();

	};
}

#endif // _DEVICE_SYSTEM_STATUS_ALARM_H_
