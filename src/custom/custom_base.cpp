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

#include <context_mgr.h>
#include "custom_base.h"

ctx::custom_base::custom_base(std::string subject, std::string name, ctx::Json tmpl, std::string owner) :
	_subject(subject),
	_name(name),
	_tmpl(tmpl),
	_owner(owner)
{
}

ctx::custom_base::~custom_base()
{
}

bool ctx::custom_base::is_supported()
{
	return true;
}

void ctx::custom_base::submit_trigger_item()
{
	context_manager::register_trigger_item(_subject.c_str(), OPS_SUBSCRIBE | OPS_READ,
			_tmpl.str(), NULL, _owner.c_str());
}

void ctx::custom_base::unsubmit_trigger_item()
{
	context_manager::unregister_trigger_item(_subject.c_str());
}

int ctx::custom_base::subscribe(const char *subject, ctx::Json option, ctx::Json *request_result)
{
	return ERR_NONE;

}

int ctx::custom_base::unsubscribe(const char *subject, ctx::Json option)
{
	return ERR_NONE;
}

int ctx::custom_base::read(const char *subject, ctx::Json option, ctx::Json *request_result)
{
	ctx::Json data = latest.str();
	ctx::context_manager::reply_to_read(_subject.c_str(), NULL, ERR_NONE, data);
	return ERR_NONE;
}

int ctx::custom_base::write(const char *subject, ctx::Json data, ctx::Json *request_result)
{
	return ERR_NONE;
}

void ctx::custom_base::handle_update(ctx::Json data)
{
	// Store latest state
	latest = data.str();
	ctx::context_manager::publish(_subject.c_str(), NULL, ERR_NONE, data);
}

const char* ctx::custom_base::get_subject()
{
	return _subject.c_str();
}

std::string ctx::custom_base::get_owner()
{
	return _owner;
}

ctx::Json ctx::custom_base::get_template()
{
	return _tmpl;
}
