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

#include "place_recognition.h"
#include "user_places/user_places.h"

void ctx::PlaceRecognitionProvider::getPrivilege(std::vector<const char*> &privilege)
{
	privilege.push_back(PRIV_LOCATION);
	privilege.push_back(PRIV_NETWORK);
}

int ctx::PlaceRecognitionProvider::subscribe(ctx::Json option, ctx::Json* requestResult)
{
	return ERR_NOT_SUPPORTED;
}

int ctx::PlaceRecognitionProvider::unsubscribe(ctx::Json option)
{
	return ERR_NOT_SUPPORTED;
}

int ctx::PlaceRecognitionProvider::read(ctx::Json option, ctx::Json* requestResult)
{
	_I(BLUE("Read"));
	_J("Option", option);

	std::vector<std::shared_ptr<ctx::Place>> places = __engine.getPlaces();
	Json dataRead = UserPlaces::composeJson(places);

	/*
	 * The below function needs to be called once.
	 * It does not need to be called within this read() function.
	 * In can be called later, in another scope.
	 * Please just be sure that, the 2nd input parameter "option" should be the same to the
	 * "option" parameter received via ctx::PlaceRecognitionProvider::read().
	 */
	replyToRead(option, ERR_NONE, dataRead);

	return ERR_NONE;
}

int ctx::PlaceRecognitionProvider::write(ctx::Json data, ctx::Json* requestResult)
{
	return ERR_NOT_SUPPORTED;
}

bool ctx::PlaceRecognitionProvider::isSupported()
{
	/* TODO: This function should be implemented properly */
	return true;
}
