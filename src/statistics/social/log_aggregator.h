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

#ifndef __CONTEXT_SOCIAL_CONTACT_LOG_AGGREGATOR_H__
#define __CONTEXT_SOCIAL_CONTACT_LOG_AGGREGATOR_H__

#include <contacts.h>
#include <db_listener_iface.h>
#include <TimerManager.h>

namespace ctx {

	class contact_log_aggregator : public db_listener_iface, public ITimerListener {
		private:
			int timer_id;
			int time_diff;
			TimerManager __timerManager;
			void create_table();
			void get_updated_contact_log_list(int last_time, contacts_list_h *list);
			void insert_contact_log_list(contacts_list_h list);
			void destroy_contact_log_list(contacts_list_h list);
			void remove_expired_log();

		public:
			contact_log_aggregator();
			~contact_log_aggregator();

			void aggregate_contact_log();

			void on_creation_result_received(unsigned int query_id, int error) {}
			void on_insertion_result_received(unsigned int query_id, int error, int64_t row_id) {}
			void on_query_result_received(unsigned int query_id, int error, std::vector<Json>& records);
			bool onTimerExpired(int timer_id);

	};	/* class phone_contact_log_aggregator */

}	/* namespace ctx */

#endif
