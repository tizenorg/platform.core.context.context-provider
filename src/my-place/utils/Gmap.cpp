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

#include "Gmap.h"
#include <iostream>
#include <fstream>

const std::string ctx::Gmap::__HTML_HEADER = R"(
<!DOCTYPE html>
<html>
  <head>
    <meta name="viewport" content="initial-scale=1.0, user-scalable=no">
    <meta charset="utf-8">
    <title>Simple markers</title>
    <style>
      html, body, #map-canvas {
        height: 100%;
        margin: 0px;
        padding: 0px
      }
    </style>
    <script src="https://maps.googleapis.com/maps/api/js?v=3.exp&signed_in=true"></script>
    <script>
function initialize() {
  var mapOptions = {
    zoom: 2,
    center: new google.maps.LatLng(20,60)
  }
  var map = new google.maps.Map(document.getElementById('map-canvas'), mapOptions);
)";

const std::string ctx::Gmap::__HTML_FOOTER = R"(
}

google.maps.event.addDomListener(window, 'load', initialize);

    </script>
  </head>
  <body>
    <div id="map-canvas"></div>
  </body>
</html>
)";

std::string ctx::Gmap::__iconForCategId(PlaceCategId categId)
{
	switch (categId) {
	case PLACE_CATEG_ID_HOME:  return "markerH.png";
	case PLACE_CATEG_ID_WORK:  return "markerW.png";
	case PLACE_CATEG_ID_OTHER: return "markerO.png";
	case PLACE_CATEG_ID_NONE:  return "markerN.png";
	default: return "markerD.png";
	}
}

void ctx::Gmap::__placeMarker2Stream(const ctx::Place& place, std::ostream& out)
{
	if (place.locationValid) {
		out << "new google.maps.Marker({" << std::endl;
		out << "    position: new google.maps.LatLng(" << place.location.latitude << "," << place.location.longitude << ")," << std::endl;
		out << "    map: map," << std::endl;
		out << "    icon: \"http://maps.google.com/mapfiles/" << __iconForCategId(place.categId)<< "\"" << std::endl;
		out << "});" << std::endl;
	}
}

void ctx::Gmap::__html2Stream(const std::vector<std::shared_ptr<ctx::Place>>& places, std::ostream& out)
{
	out << __HTML_HEADER;
	for (std::shared_ptr<ctx::Place> place : places) {
		__placeMarker2Stream(*place, out);
	}
	out << __HTML_FOOTER;
}

void ctx::Gmap::writeMap(const std::vector<std::shared_ptr<ctx::Place>>& places)
{
	std::ofstream out(GMAP_FILE);
	__html2Stream(places, out);
}
