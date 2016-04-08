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

#ifndef _CONTEXT_PLACE_RECOGNITION_GMAP_H_
#define _CONTEXT_PLACE_RECOGNITION_GMAP_H_

#include "user_places_types.h"
#include "../place_recognition_types.h"

#define GMAP_FILE "/opt/usr/media/Others/user_places_map.html"

namespace ctx {

	/*
	 * Class for generating a HTML page with GoogleMaps with all user places.
	 * This class is for test/demo purposes only. TODO: Remove this class from final solution.
	 */
	class Gmap {

	private:
		const static std::string __htmlHeader;
		const static std::string __htmlFooter;
		static std::string __iconForCategId(PlaceCategId categ_id);
		static void __placeMarker2Stream(const Place& place, std::ostream& out);
		static void __html2Stream(const std::vector<std::shared_ptr<Place>>& places, std::ostream& out);

	public:
		static void writeMap(const std::vector<std::shared_ptr<Place>>& places);

	};	/* class Gmap */

}	/* namespace ctx */

#endif /* End of _CONTEXT_PLACE_RECOGNITION_GMAP_H_ */
