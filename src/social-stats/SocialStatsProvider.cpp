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

#include "SocialStatsProvider.h"
#include "DbHandle.h"

using namespace ctx;

SocialStatsProvider::SocialStatsProvider(const char *subject) :
	ContextProvider(subject)
{
}

SocialStatsProvider::~SocialStatsProvider()
{
}

void SocialStatsProvider::getPrivilege(std::vector<const char*> &privilege)
{
	privilege.push_back(PRIV_CALL_HISTORY);
}

bool SocialStatsProvider::isSupported()
{
	/* TODO */
	return true;
}

int SocialStatsProvider::read(Json option, Json* requestResult)
{
	SocialDbHandle *handle = new(std::nothrow) SocialDbHandle(this);
	IF_FAIL_RETURN_TAG(handle, ERR_OPERATION_FAILED, _E, "Memory allocation failed");

	int err = handle->read(option);
	if (err != ERR_NONE) {
		delete handle;
		return err;
	}

	return ERR_NONE;
}
