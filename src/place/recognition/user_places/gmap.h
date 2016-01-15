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

#ifndef __CONTEXT_PLACE_STATUS_GMAP_H__
#define __CONTEXT_PLACE_STATUS_GMAP_H__

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
		const static std::string html_header;
		const static std::string html_footer;
		static std::string icon_for_categ_id(place_categ_id_e categ_id);
		static void place_marker_to_stream(const Place& place, std::ostream& out);
		static void html_to_stream(const std::vector<std::shared_ptr<Place>>& places, std::ostream& out);

	public:
		static void write_map(const std::vector<std::shared_ptr<Place>>& places);

	};	/* class Gmap */

}	/* namespace ctx */

#endif /* __CONTEXT_PLACE_STATUS_VISIT_GMAP_H__ */
