/*
 * clibdocker: docker_images.c
 * Created on: 31-Dec-2018
 *
 * clibdocker
 * Copyright (C) 2018 Abhishek Mishra <abhishekmishra3@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdlib.h>
#include "string.h"
#include "docker_images.h"
#include "log.h"

void parse_status_cb(char* msg, void* cb) {
	void (*status_cb)(
			docker_image_create_status*) = (void (*)(docker_image_create_status*))cb;
	if (msg) {
		if(status_cb) {
			json_object* response_obj = json_tokener_parse(msg);
			char* status_msg = get_attr_str(response_obj, "status");
			char* id = get_attr_str(response_obj, "id");
			docker_image_create_status* status = (docker_image_create_status*)calloc(1, sizeof(docker_image_create_status));
			if(status != NULL) {
				status->status = status_msg;
				status->id = id;
			}
			status_cb(status);
		} else {
			docker_log_debug("Message = Empty");
		}
	}
}

//Docker Image Create commands
/**
 * see https://docs.docker.com/engine/api/v1.39/#operation/ImageCreate
 * Create a new image by pulling image:tag for platform
 *
 * \param ctx docker context
 * \param result the docker result object to return
 * \param from_image image name
 * \param tag which tag to pull, for e.g. "latest"
 * \param platform which platform to pull the image for (format os[/arch[/variant]]),
 * 			default is ""
 * \return error code.
 */
error_t docker_image_create_from_image(docker_context* ctx,
		docker_result** result, char* from_image, char* tag, char* platform) {
	return docker_image_create_from_image_cb(ctx, result, NULL, from_image, tag,
			platform);
}

/**
 * see https://docs.docker.com/engine/api/v1.39/#operation/ImageCreate
 * Create a new image by pulling image:tag for platform, with a progress callback
 *
 * \param ctx docker context
 * \param result the docker result object to return
 * \param status_cb callback to call with the status object
 * \param from_image image name
 * \param tag which tag to pull, for e.g. "latest"
 * \param platform which platform to pull the image for (format os[/arch[/variant]]),
 * 			default is ""
 * \return error code.
 */
error_t docker_image_create_from_image_cb(docker_context* ctx,
		docker_result** result, void (*status_cb)(docker_image_create_status*),
		char* from_image, char* tag, char* platform) {
	if (from_image == NULL || strlen(from_image) == 0) {
		return E_INVALID_INPUT;
	}

	char* url = create_service_url_id_method(IMAGE, NULL, "create");

	struct array_list* params = array_list_new(
			(void (*)(void *)) &free_url_param);
	url_param* p;

	make_url_param(&p, "fromImage", from_image);
	array_list_add(params, p);
	if (tag != NULL) {
		make_url_param(&p, "tag", tag);
		array_list_add(params, p);
	}
	if (platform != NULL) {
		make_url_param(&p, " platform", platform);
		array_list_add(params, p);
	}

	json_object *response_obj = NULL;
	struct http_response_memory chunk;
	docker_api_post_cb(ctx, result, url, params, "", &chunk, &response_obj,
			&parse_status_cb, status_cb);

	if ((*result)->http_error_code >= 200) {
		return E_UNKNOWN_ERROR;
	}

	return E_SUCCESS;
}

