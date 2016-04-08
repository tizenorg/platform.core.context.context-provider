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

#include <ContextManager.h>
#include "CustomBase.h"

ctx::CustomBase::CustomBase(std::string subject, std::string name, ctx::Json tmpl, std::string owner) :
	__subject(subject),
	__name(name),
	__tmpl(tmpl),
	__owner(owner)
{
}

ctx::CustomBase::~CustomBase()
{
}

bool ctx::CustomBase::isSupported()
{
	return true;
}

void ctx::CustomBase::submitTriggerItem()
{
	context_manager::registerTriggerItem(__subject.c_str(), OPS_SUBSCRIBE | OPS_READ,
			__tmpl.str(), NULL, __owner.c_str());
}

void ctx::CustomBase::unsubmitTriggerItem()
{
	context_manager::unregisterTriggerItem(__subject.c_str());
}

int ctx::CustomBase::subscribe(const char *subject, ctx::Json option, ctx::Json *requestResult)
{
	return ERR_NONE;

}

int ctx::CustomBase::unsubscribe(const char *subject, ctx::Json option)
{
	return ERR_NONE;
}

int ctx::CustomBase::read(const char *subject, ctx::Json option, ctx::Json *requestResult)
{
	ctx::Json data = __latest.str();
	ctx::context_manager::replyToRead(__subject.c_str(), NULL, ERR_NONE, data);
	return ERR_NONE;
}

int ctx::CustomBase::write(const char *subject, ctx::Json data, ctx::Json *requestResult)
{
	return ERR_NONE;
}

void ctx::CustomBase::handleUpdate(ctx::Json data)
{
	// Store latest state
	__latest = data.str();
	ctx::context_manager::publish(__subject.c_str(), NULL, ERR_NONE, data);
}

const char* ctx::CustomBase::getSubject()
{
	return __subject.c_str();
}

std::string ctx::CustomBase::getOwner()
{
	return __owner;
}

ctx::Json ctx::CustomBase::getTemplate()
{
	return __tmpl;
}
