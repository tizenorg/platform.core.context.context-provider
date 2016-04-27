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

#include <media_content.h>
#include <Types.h>

extern "C" SO_EXPORT bool getMediaPlayCount(int updateItem, int updateType, int mediaType, char *uuid, int *count)
{
	IF_FAIL_RETURN(updateItem == MEDIA_ITEM_FILE && updateType == MEDIA_CONTENT_UPDATE, false);
	IF_FAIL_RETURN(mediaType == MEDIA_CONTENT_TYPE_MUSIC || mediaType == MEDIA_CONTENT_TYPE_VIDEO, false);

	int err = media_content_connect();
	IF_FAIL_RETURN_TAG(err == MEDIA_CONTENT_ERROR_NONE, false, _E, "media_content_connect() failed");

	media_info_h media = NULL;
	media_info_get_media_from_db(uuid, &media);
	if (!media) {
		_E("media_info_get_media_from_db() failed");
		media_content_disconnect();
		return false;
	}

	media_info_get_played_count(media, count);
	media_info_destroy(media);
	media_content_disconnect();

	return true;
}
