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

#ifndef _CONTEXT_DEVICE_PROVIDER_BASE_H_
#define _CONTEXT_DEVICE_PROVIDER_BASE_H_

#include <types_internal.h>
#include <Json.h>
#include <ContextProviderBase.h>

#define CREATE_INSTANCE(prvd) \
	do { \
		IF_FAIL_RETURN(!__instance, __instance); \
		__instance = new(std::nothrow) prvd(); \
		IF_FAIL_RETURN_TAG(__instance, NULL, _E, "Memory allocation failed"); \
		_I(BLUE("Created")); \
		return __instance; \
	} while (0) \

#define DESTROY_INSTANCE() \
	do { \
		IF_FAIL_VOID(__instance); \
		delete __instance; \
		__instance = NULL; \
		_I(BLUE("Destroyed")); \
	} while (0) \

#define GENERATE_PROVIDER_COMMON_DECL(prvd) \
	public: \
		static ContextProviderBase *create(void *data); \
		static void destroy(void *data); \
	protected: \
		void destroySelf(); \
	private: \
		static prvd *__instance; \

#define GENERATE_PROVIDER_COMMON_IMPL(prvd) \
	ctx::prvd *ctx::prvd::__instance = NULL; \
	ctx::ContextProviderBase *ctx::prvd::create(void *data) \
	{ \
		CREATE_INSTANCE(prvd); \
	} \
	void ctx::prvd::destroy(void *data) \
	{ \
		DESTROY_INSTANCE(); \
	} \
	void ctx::prvd::destroySelf() \
	{ \
		destroy(NULL); \
	} \

namespace ctx {

	class DeviceProviderBase : public ContextProviderBase {
	public:
		int subscribe(const char *subject, ctx::Json option, ctx::Json *requestResult);
		int unsubscribe(const char *subject, ctx::Json option);
		int read(const char *subject, ctx::Json option, ctx::Json *requestResult);
		int write(const char *subject, ctx::Json data, ctx::Json *requestResult);

	protected:
		bool __beingSubscribed;

		DeviceProviderBase();
		virtual ~DeviceProviderBase() {}

		virtual int subscribe();
		virtual int unsubscribe();
		virtual int read();
		virtual int write();
		virtual void destroySelf() = 0;

		static bool getSystemInfoBool(const char *key);
	};
}

#endif	// _CONTEXT_DEVICE_PROVIDER_BASE_H_
