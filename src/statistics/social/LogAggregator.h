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

#ifndef _CONTEXT_STATS_SOCIAL_CONTACT_LOG_AGGREGATOR_H_
#define _CONTEXT_STATS_SOCIAL_CONTACT_LOG_AGGREGATOR_H_

#include <contacts.h>
#include <db_listener_iface.h>
#include <TimerManager.h>

namespace ctx {

	class ContactLogAggregator : public db_listener_iface, public ITimerListener {
	private:
		int __timerId;
		int __timeDiff;
		TimerManager __timerManager;
		void __createTable();
		void __getUpdatedContactLogList(int last_time, contacts_list_h *list);
		void __insertContactLogList(contacts_list_h list);
		void __destroyContactLogList(contacts_list_h list);
		void __removeExpiredLog();

	public:
		ContactLogAggregator();
		~ContactLogAggregator();

		void aggregateContactLog();

		void on_creation_result_received(unsigned int queryId, int error) {}
		void on_insertion_result_received(unsigned int queryId, int error, int64_t rowId) {}
		void on_query_result_received(unsigned int queryId, int error, std::vector<Json>& records);
		bool onTimerExpired(int timerId);

	};	/* class ContactLogAggregator */

}	/* namespace ctx */

#endif	/* End of _CONTEXT_STATS_SOCIAL_CONTACT_LOG_AGGREGTOR_H_ */
