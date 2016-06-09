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

#include "CustomProvider.h"

using namespace ctx;

CustomProvider::CustomProvider(const char* subject, std::string name, Json tmpl, std::string owner) :
	BasicProvider(subject),
	__name(name),
	__tmpl(tmpl),
	__owner(owner)
{
}

CustomProvider::~CustomProvider()
{
}

bool CustomProvider::isSupported()
{
	return true;
}

bool CustomProvider::unloadable()
{
	return false;
}

int CustomProvider::subscribe()
{
	return ERR_NONE;

}

int CustomProvider::unsubscribe()
{
	return ERR_NONE;
}

int CustomProvider::read()
{
	Json data = __latest.str();

	replyToRead(NULL, ERR_NONE, data);
	return ERR_NONE;
}

void CustomProvider::handleUpdate(Json data)
{
	// Store latest state
	__latest = data.str();

	publish(NULL, ERR_NONE, data);
}

std::string CustomProvider::getOwner()
{
	return __owner;
}

Json CustomProvider::getTemplate()
{
	return __tmpl;
}
