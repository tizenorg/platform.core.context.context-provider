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

#ifndef _CONTEXT_PLACE_RECOGNITION_VISIT_DETECTOR_H_
#define _CONTEXT_PLACE_RECOGNITION_VISIT_DETECTOR_H_

#include <memory>
#include <vector>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include "user_places_types.h"
#include <Json.h>
#include "visit_listener_iface.h"
#include "location_logger.h"
#include "location_listener_iface.h"
#include "wifi_listener_iface.h"
#include "wifi_logger.h"

namespace ctx {

	class VisitDetector : public IWifiListener, ILocationListener {

	private:
		bool __testMode;
		std::shared_ptr<visits_t> __detectedVisits; // only used in test mode
		LocationLogger __locationLogger;
		WifiLogger __wifiLogger;
		std::vector<IVisitListener*> __listeners;
		std::shared_ptr<mac_events> __currentLogger;
		Interval __currentInterval;
		std::vector<LocationEvent> __locationEvents;
		std::vector<std::shared_ptr<Frame>> __historyFrames;  // python: history_scans  + history_times
		std::vector<std::shared_ptr<Frame>> __bufferedFrames; // python: buffered_scans + buffered_times
		int __stableCounter;
		int __tolerance;
		bool __entranceToPlace;
		int __periodSeconds;

		// fields that  are used only in case of entrance detection
		std::shared_ptr<mac_set_t> __representativesMacs; // macs that represent the current place
		std::shared_ptr<mac_set_t> __stayMacs; // macs that can appear in the current place
		time_t __entranceTime;
		time_t __departureTime;

		bool __isValid(const Mac &mac);
		void __shiftCurrentInterval();
		void __detectEntranceOrDeparture(std::shared_ptr<Frame> frame);
		void __detectEntrance(std::shared_ptr<Frame> frame);
		void __detectDeparture(std::shared_ptr<Frame> frame);
		void __processBuffer(std::shared_ptr<Frame> frame); // python: buffer_analysing
		std::shared_ptr<Frame> __makeFrame(std::shared_ptr<mac_events> mac_events, Interval interval);  // python: scans2fingerprint
		void __resetHistory();
		void __resetHistory(std::shared_ptr<Frame> frame);
		void __visitStartDetected();
		void __visitEndDetected();
		void __putLocationToVisit(Visit &visit);
		std::shared_ptr<mac_set_t> __selectRepresentatives(const std::vector<std::shared_ptr<Frame>> &frames);
		std::shared_ptr<mac_set_t> __macSetOfGreaterOrEqualShare(const mac_shares_t &mac_shares, share_t threshold);
		std::shared_ptr<mac_shares_t> __macSharesFromCounts(mac_counts_t const &mac_counts, count_t denominator); // python: response_rate
		share_t __calcMaxShare(const mac_shares_t &mac_shares);
		bool __isDisjoint(const mac_counts_t &mac_counts, const mac_set_t &macSet);
		bool __protrudesFrom(const mac_counts_t &mac_counts, const mac_set_t &macSet);
		void __setPeriod(place_recog_mode_e mode);
		void __processCurrentLogger();

		/* DATABASE */
		void __dbCreateTable();
		int __dbInsertVisit(Visit visit);
		void __putVisitCategToJson(const char* key, const categs_t &categs, int categ_type, Json &data);
		void __putVisitCategsToJson(const categs_t &categs, Json &data);

		/* INPUT */
		void onWifiScan(MacEvent event);
		void onNewLocation(LocationEvent location);

	public:
		VisitDetector(time_t t_start_scan, place_recog_mode_e energyMode = PLACE_RECOG_HIGH_ACCURACY_MODE, bool testMode = false);
		~VisitDetector();

		std::shared_ptr<visits_t> getVisits(); // only used in test mode
		void setMode(place_recog_mode_e energyMode);

	};	/* class VisitDetector */

}	/* namespace ctx */

#endif /* End of _CONTEXT_PLACE_RECOGNITION_VISIT_DETECTOR_H_ */
