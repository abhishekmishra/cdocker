/*
 * clibdocker: docker_volumes.c
 * Created on: 06-Jan-2019
 *
 * MIT License
 *
 * Copyright (c) 2018 Abhishek Mishra
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 */
#include "docker_util.h"
#include <docker_log.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>
#include "docker_volumes.h"

/**
 * Get the list of volumes, matching the filters provided.
 * (Any and all filters can be null/0.)
 *
 * \param ctx the docker context
 * \param result the result object to be returned.
 * \param volumes the list of docker_volume_item objects
 * \param warnings the list of warnings
 * \param filter_dangling 0 indicates false, anything else is true
 * \param filter_driver driver filter
 * \param filter_label
 * \param filter_name
 * \return error code.
 */
d_err_t docker_volumes_list(docker_context* ctx, docker_result** result,
		docker_volume_list** volumes, docker_volume_warnings** warnings,
		int filter_dangling, char* filter_driver, char* filter_label,
		char* filter_name) {
	char* url = create_service_url_id_method(VOLUME, NULL, NULL);
	if (url == NULL) {
		return E_ALLOC_FAILED;
	}

	arraylist* params;
	arraylist_new(&params,
			(void (*)(void *)) &free_url_param);
	url_param* p;
	json_object* filters = make_filters();

	if (filter_dangling == 0) {
		add_filter_str(filters, "dangling", "false");
	}
	if (filter_driver != NULL) {
		add_filter_str(filters, "driver", filter_driver);
	}
	if (filter_label != NULL) {
		add_filter_str(filters, "label", filter_label);
	}
	if (filter_name != NULL) {
		add_filter_str(filters, "name", filter_name);
	}
	make_url_param(&p, "filters", (char*) filters_to_str(filters));
	arraylist_add(params, p);

	json_object *response_obj = NULL;
	struct http_response_memory chunk;
	docker_api_get(ctx, result, url, params, &chunk, &response_obj);

	*volumes = json_object_get(get_attr_json_object(response_obj, "Volumes"));
	*warnings = json_object_get(get_attr_json_object(response_obj, "Warnings"));

	json_object_put(response_obj);
	free(params);
	if (chunk.memory != NULL) {
		free(chunk.memory);
	}
	return E_SUCCESS;
}

/**
 * Create a new volume by providing name, driver and an optional list of key/value pairs for labels
 *
 * \param ctx the docker context
 * \param result the result object to return
 * \param volume the volume object to return
 * \param name name of the volume to create (cannot be NULL)
 * \param driver name of the driver to use
 * \param num_labels the number of labels to be attached.
 * \param key/values char* key, char* value args pair for each label
 * \return error code
 */
d_err_t docker_volume_create(docker_context* ctx, docker_result** result,
		docker_volume** volume, char* name, char* driver, int num_labels,
		...) {
	va_list kvargs;
	va_start(kvargs, num_labels);
	json_object* request_obj = json_object_new_object();
	json_object_object_add(request_obj, "Name", json_object_new_string(name));
	json_object_object_add(request_obj, "Driver",
			json_object_new_string(driver));
	json_object* labels = json_object_new_object();
	for (int i = 0; i < num_labels; i++) {
		char* key = va_arg(kvargs, char*);
		char* val = va_arg(kvargs, char*);
		json_object_object_add(labels, key, json_object_new_string(val));
	}
	json_object_object_add(request_obj, "Labels", labels);

	const char* request_str = json_object_get_string(request_obj);
	docker_log_debug("Request body is %s", request_str);

	char* url = create_service_url_id_method(VOLUME, NULL, "create");
	if (url == NULL) {
		return E_ALLOC_FAILED;
	}

	struct http_response_memory chunk;
	docker_api_post(ctx, result, url, NULL, request_str, &chunk, volume);

	json_object_put(request_obj);
	if (chunk.memory != NULL) {
		free(chunk.memory);
	}
	return E_SUCCESS;
}

/**
 * Inspect an existing volume.
 *
 * \param ctx the docker context
 * \param result the result object to return
 * \param volume the volume object to return
 * \param name name of the volume to inspect (cannot be NULL)
 * \return error code
 */
d_err_t docker_volume_inspect(docker_context* ctx, docker_result** result,
		docker_volume** volume, char* name) {
	if (name == NULL) {
		return E_INVALID_INPUT;
	}
	char* url = create_service_url_id_method(VOLUME, NULL, name);
	if (url == NULL) {
		return E_ALLOC_FAILED;
	}

	struct http_response_memory chunk;
	docker_api_get(ctx, result, url, NULL, &chunk, volume);

	if (chunk.memory != NULL) {
		free(chunk.memory);
	}
	return E_SUCCESS;
}

/**
 * Delete the given volume (identified by name).
 *
 * \param ctx the docker context
 * \param result the result object to return
 * \param name name of the volume to delete
 * \param force force delete if this value is 1
 * \return error code
 */
d_err_t docker_volume_delete(docker_context* ctx, docker_result** result,
		const char* name, int force) {
	char* url = create_service_url_id_method(VOLUME, NULL, name);

	arraylist* params;
	arraylist_new(&params,
			(void (*)(void *)) &free_url_param);
	url_param* p;
	if (force == 1) {
		make_url_param(&p, "force", str_clone("true"));
		arraylist_add(params, p);
	}

	json_object *response_obj = NULL;
	struct http_response_memory chunk;
	docker_api_delete(ctx, result, url, params, &chunk, &response_obj);

	return E_SUCCESS;
}

/**
 * Delete unused volumes.
 *
 * \param ctx the docker context
 * \param result the result object to return
 * \param volumes_deleted array_list with names of volumes deleted
 * \param space_reclaimed num bytes freed.
 * \param num_label_filters how many label filters are there
 * \param varargs triples (int filter_not, char* label_name, char* label_value)
 * \return error code
 */
d_err_t docker_volumes_delete_unused(docker_context* ctx,
		docker_result** result, arraylist** volumes_deleted,
		unsigned long* space_reclaimed, int num_label_filters, ...) {
	char* url = create_service_url_id_method(VOLUME, NULL, "prune");

	arraylist* params;
	arraylist_new(&params,
			(void (*)(void *)) &free_url_param);

	url_param* p;

	va_list kvargs;
	va_start(kvargs, num_label_filters);
	json_object* filters = make_filters();
	for (int i = 0; i < num_label_filters; i++) {
		int filter_not = va_arg(kvargs, int);
		char* label_name = va_arg(kvargs, char*);
		char* label_value = va_arg(kvargs, char*);

		char* filter_value;
		if (label_value != NULL) {
			filter_value = (char*) calloc(
					strlen(label_name) + strlen(label_value) + 2, sizeof(char));
			strcpy(filter_value, label_name);
			strcat(filter_value, "=");
			strcat(filter_value, label_value);
		} else {
			filter_value = (char*) calloc(strlen(label_name) + 1, sizeof(char));
			strcpy(filter_value, label_name);
		}
		if (filter_not == 1) {
			add_filter_str(filters, "label!", filter_value);
		} else {
			add_filter_str(filters, "label", filter_value);
		}
	}
	make_url_param(&p, "filters", (char*) filters_to_str(filters));
	arraylist_add(params, p);

	json_object *response_obj = NULL;
	struct http_response_memory chunk;
	docker_api_post(ctx, result, url, params, "", &chunk, &response_obj);

	if ((*result)->http_error_code == 200) {
		if (response_obj) {
			arraylist* vols_del;
			arraylist_new(&vols_del, &free);
			json_object* vols_obj;
			json_object_object_get_ex(response_obj, "VolumesDeleted",
					&vols_obj);
			if (vols_obj) {
				size_t vols_len = json_object_array_length(vols_obj);
				for (int i = 0; i < vols_len; i++) {
					arraylist_add(vols_del,
							str_clone(
									json_object_get_string(
											json_object_array_get_idx(vols_obj,
													i))));
				}
			}
			(*volumes_deleted) = vols_del;
			(*space_reclaimed) = get_attr_unsigned_long(response_obj,
					"SpaceReclaimed");
		}
	}
	return E_SUCCESS;
}
