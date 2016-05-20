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

#ifndef _CONTEXT_PLACE_RECOGNITION_VISIT_DETECTOR_H_
#define _CONTEXT_PLACE_RECOGNITION_VISIT_DETECTOR_H_

#include <memory>
#include <vector>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include "user_places_types.h"
#include <Json.h>
#include <DatabaseManager.h>
#include "visit_listener_iface.h"
#include "location_logger.h"
#include "location_listener_iface.h"
#include "wifi_listener_iface.h"
#include "wifi_logger.h"

namespace ctx {

	class VisitDetector : public IWifiListener, ILocationListener {

	private:
		bool __testMode;
		std::shared_ptr<Visits> __detectedVisits; // only used in test mode
		LocationLogger *__locationLogger;
		WifiLogger *__wifiLogger;
		std::vector<IVisitListener*> __listeners;
		std::shared_ptr<MacEvents> __currentMacEvents;
		Interval __currentInterval;
		std::vector<LocationEvent> __locationEvents;
		std::vector<std::shared_ptr<Frame>> __historyFrames;  // python: history_scans  + history_times
		std::vector<std::shared_ptr<Frame>> __bufferedFrames; // python: buffered_scans + buffered_times
		int __stableCounter;
		int __tolerance;
		bool __entranceToPlace;
		int __periodSeconds;
		DatabaseManager *__dbManager;

		// fields that  are used only in case of entrance detection
		std::shared_ptr<MacSet> __representativesMacs; // macs that represent the current place
		std::shared_ptr<MacSet> __stayMacs; // macs that can appear in the current place
		time_t __entranceTime;
		time_t __departureTime;

		bool __isValid(const Mac &mac);
		void __shiftCurrentInterval();
		void __detectEntranceOrDeparture(std::shared_ptr<Frame> frame);
		void __detectEntrance(std::shared_ptr<Frame> frame);
		void __detectDeparture(std::shared_ptr<Frame> frame);
		void __processBuffer(std::shared_ptr<Frame> frame); // python: buffer_analysing
		std::shared_ptr<Frame> __makeFrame(std::shared_ptr<MacEvents> macEvents, Interval interval);  // python: scans2fingerprint
		void __resetHistory();
		void __resetHistory(std::shared_ptr<Frame> frame);
		void __visitStartDetected();
		void __visitEndDetected();
		void __putLocationToVisit(Visit &visit);
		std::shared_ptr<MacSet> __selectRepresentatives(const std::vector<std::shared_ptr<Frame>> &frames);
		std::shared_ptr<MacSet> __macSetOfGreaterOrEqualShare(const Macs2Shares &macs2Shares, share_t threshold);
		std::shared_ptr<Macs2Shares> __macSharesFromCounts(Macs2Counts const &macs2Counts, count_t denominator); // python: response_rate
		share_t __calcMaxShare(const Macs2Shares &macs2Shares);
		bool __isDisjoint(const Macs2Counts &macs2Counts, const MacSet &macSet);
		bool __protrudesFrom(const Macs2Counts &macs2Counts, const MacSet &macSet);
		void __setPeriod(PlaceRecogMode mode);
		void __processCurrentLogger();

		/* DATABASE */
		void __dbCreateTable();
		int __dbInsertVisit(Visit visit);
		void __putVisitCategToJson(const char* key, const Categs &categs, int categType, Json &data);
		void __putVisitCategsToJson(const Categs &categs, Json &data);

		/* INPUT */
		void onWifiScan(MacEvent event);
		void onNewLocation(LocationEvent location);

	public:
		VisitDetector(time_t startScan, PlaceRecogMode energyMode = PLACE_RECOG_HIGH_ACCURACY_MODE, bool testMode = false);
		~VisitDetector();

		std::shared_ptr<Visits> getVisits(); // only used in test mode
		void setMode(PlaceRecogMode energyMode);

		friend class Test;
		friend class UserPlacesTestUtils;

	};	/* class VisitDetector */

}	/* namespace ctx */

#endif /* End of _CONTEXT_PLACE_RECOGNITION_VISIT_DETECTOR_H_ */
