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

#include <types_internal.h>
#include <ContextManager.h>
#include "place_recognition.h"
#include "user_places/user_places.h"

ctx::place_recognition_provider *ctx::place_recognition_provider::__instance = NULL;

ctx::ContextProviderBase *ctx::place_recognition_provider::create(void *data)
{
	IF_FAIL_RETURN(!__instance, __instance);
	__instance = new(std::nothrow) place_recognition_provider();
	IF_FAIL_RETURN_TAG(__instance, NULL, _E, "Memory allocation failed");
	_I(BLUE("Created"));
	return __instance;
}

void ctx::place_recognition_provider::destroy(void *data)
{
	IF_FAIL_VOID(__instance);
	delete __instance;
	__instance = NULL;
	_I(BLUE("Destroyed"));
}

int ctx::place_recognition_provider::subscribe(const char *subject, ctx::Json option, ctx::Json* requestResult)
{
	return ERR_NOT_SUPPORTED;
}

int ctx::place_recognition_provider::unsubscribe(const char *subject, ctx::Json option)
{
	return ERR_NOT_SUPPORTED;
}

int ctx::place_recognition_provider::read(const char *subject, ctx::Json option, ctx::Json* requestResult)
{
	_I(BLUE("Read"));
	_J("Option", option);

	std::vector<std::shared_ptr<ctx::Place>> places = engine.get_places();
	Json dataRead = engine.compose_json(places);

	// The below function needs to be called once.
	// It does not need to be called within this read() function.
	// In can be called later, in another scope.
	// Please just be sure that, the 2nd input parameter "option" should be the same to the
	// "option" parameter received via ctx::place_recognition_provider::read().
	ctx::context_manager::replyToRead(PLACE_SUBJ_RECOGNITION, option, ERR_NONE, dataRead);

	return ERR_NONE;
}

int ctx::place_recognition_provider::write(const char *subject, ctx::Json data, ctx::Json* requestResult)
{
	return ERR_NOT_SUPPORTED;
}

bool ctx::place_recognition_provider::is_supported()
{
	return true;
}
