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

#ifndef _CONTEXT_PLACE_RECOGNITION_H_
#define _CONTEXT_PLACE_RECOGNITION_H_

#include <ContextProvider.h>
#include "place_recognition_types.h"
#include "user_places/user_places.h"

namespace ctx {

	class PlaceRecognitionProvider : public ContextProvider {
	public:
		PlaceRecognitionProvider() :
			ContextProvider(PLACE_SUBJ_RECOGNITION),
			__engine(PLACE_RECOG_HIGH_ACCURACY_MODE) {}

		~PlaceRecognitionProvider() {}

		int subscribe(ctx::Json option, ctx::Json *requestResult);
		int unsubscribe(ctx::Json option);
		int read(ctx::Json option, ctx::Json *requestResult);
		int write(ctx::Json data, ctx::Json *requestResult);

		bool isSupported();
		void submitTriggerItem() {}

	private:
		UserPlaces __engine;
	};	/* class PlaceRecognitionProvider */

}	/* namespace ctx */

#endif /* End of _CONTEXT_PLACE_RECOGNITION_H_ */
