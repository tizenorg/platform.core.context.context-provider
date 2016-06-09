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

#include <algorithm>
#include <Types.h>
#include "Median.h"

static bool compareFun(std::pair<double, int> &i, std::pair<double, int> &j) {
	return (i.first < j.first);
}

double ctx::median(std::vector<double> &values, int &elemIdx, int &evenCaseElemIdx)
{
	if (values.empty()) {
		_E("Median of empty set");
		return -1;
	}
	std::vector<std::pair<double, int>> valuesTemp;
	for (size_t i = 0; i < values.size(); i++) {
		valuesTemp.push_back(std::pair<double, int>(values[i], i));
	}
	int n = valuesTemp.size() / 2;
	std::nth_element(valuesTemp.begin(), valuesTemp.begin() + n, valuesTemp.end(), compareFun);
	std::pair<double, int> valuesTempN = valuesTemp[n];
	elemIdx = valuesTempN.second;
	if (valuesTemp.size() % 2 == 1) { //odd size
		evenCaseElemIdx = -1;
		return valuesTempN.first;
	} else { // even size
		std::nth_element(valuesTemp.begin(), valuesTemp.begin() + n - 1, valuesTemp.end());
		evenCaseElemIdx = valuesTemp[n - 1].second;
		return 0.5 * (valuesTempN.first + valuesTemp[n - 1].first);
	}
}

ctx::Location ctx::medianLocation(std::vector<double> &latitudes, std::vector<double> &longitudes, std::vector<double> &accuracy)
{
	ctx::Location location;
	if (latitudes.empty() || latitudes.size() != longitudes.size() || latitudes.size() != accuracy.size()) {
		_E("Incorrect input vectors size");
		return location;
	}
	int idx;
	int additionalIdx;
	location.latitude = median(latitudes, idx, additionalIdx);
	double latitudeAccuracy = accuracy[idx];
	if (additionalIdx >= 0) {
		latitudeAccuracy = 0.5 * (latitudeAccuracy + accuracy[additionalIdx]);
	}
	location.longitude = median(longitudes, idx, additionalIdx);
	double longitudeAccuracy = accuracy[idx];
	if (additionalIdx >= 0) {
		longitudeAccuracy = 0.5 * (longitudeAccuracy + accuracy[additionalIdx]);
	}
	location.accuracy = 0.5 * (latitudeAccuracy + longitudeAccuracy);
	return location;
}
