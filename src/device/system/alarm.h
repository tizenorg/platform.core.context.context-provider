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

#ifndef _DEVICE_STATUS_ALARM_H_
#define _DEVICE_STATUS_ALARM_H_

#include <map>
#include <set>
#include <provider_iface.h>
#include <TimerManager.h>
#include "../provider_base.h"

namespace ctx {

	class device_status_alarm : public context_provider_iface, ITimerListener {

		GENERATE_PROVIDER_COMMON_DECL(device_status_alarm);

	public:
		int subscribe(const char *subject, ctx::Json option, ctx::Json *request_result);
		int unsubscribe(const char *subject, ctx::Json option);
		int read(const char *subject, ctx::Json option, ctx::Json *request_result);
		int write(const char *subject, ctx::Json data, ctx::Json *request_result);

		int subscribe(ctx::Json option);
		int unsubscribe(ctx::Json option);
		static bool is_supported();
		static void submit_trigger_item();

	private:
		device_status_alarm();
		~device_status_alarm();
		void handle_update();
		static void update_cb(void* user_data);
		int get_arranged_day_of_week(ctx::Json& option);

		struct ref_count_array_s {
			int count[7];	/* reference counts for days of week*/
			ref_count_array_s();
		};

		struct timer_state_s {
			int timer_id;
			int day_of_week; /* day of week, merged into one integer */
			timer_state_s() : timer_id(-1), day_of_week(0) {}
		};

		typedef std::map<int, ref_count_array_s> ref_count_map_t;
		typedef std::map<int, timer_state_s> timer_state_map_t;
		typedef std::set<ctx::Json*> option_t;

		ref_count_map_t ref_count_map;
		timer_state_map_t timer_state_map;
		option_t option_set;
		TimerManager __timerManager;

		bool add(int minute, int day_of_week);
		bool remove(int minute, int day_of_week);
		void clear();
		bool empty();

		int merge_day_of_week(int *ref_cnt);
		bool reset_timer(int hour);
		void on_timer_expired(int hour, int min, int day_of_week);
		bool onTimerExpired(int timer_id);

		bool is_matched(ctx::Json& option, int time, std::string day);
		option_t::iterator find_option(ctx::Json& option);

		void destroy_if_unused();

	};
}

#endif // _DEVICE_STATUS_ALARM_H_
