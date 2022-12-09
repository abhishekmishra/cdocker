/*
 *
 * Copyright (c) 2018-2022 Abhishek Mishra
 *
 * This file is part of clibdocker.
 *
 * clibdocker is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as 
 * published by the Free Software Foundation, 
 * either version 3 of the License, or (at your option) 
 * any later version.
 *
 * clibdocker is distributed in the hope that it will be useful, 
 * but WITHOUT ANY WARRANTY; without even the implied warranty 
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
 * See the GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public 
 * License along with clibdocker. 
 * If not, see <https://www.gnu.org/licenses/>.
 *
 */

#include "docker_util.h"
#include <docker_log.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>
#include "docker_volumes.h"

d_err_t docker_volumes_list(docker_context* ctx, docker_volume_list** volumes, 
		docker_volume_warnings** warnings,	int filter_dangling, 
		char* filter_driver, char* filter_label, char* filter_name) {
	docker_call* call;
	if (make_docker_call(&call, ctx->url, VOLUME, NULL, NULL) != 0) {
		return E_ALLOC_FAILED;
	}

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
	char* filter_str = (char*)filters_to_str(filters);
	docker_call_params_add(call, "filters", filter_str);
	free(filter_str);

	json_object *response_obj = NULL;
	d_err_t err = docker_call_exec(ctx, call, &response_obj);

	*volumes = json_object_get(get_attr_json_object(response_obj, "Volumes"));
	*warnings = json_object_get(get_attr_json_object(response_obj, "Warnings"));

	json_object_put(response_obj);
	free_docker_call(call);
	return err;
}

d_err_t docker_volume_create(docker_context* ctx, 
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

	docker_call* call;
	if (make_docker_call(&call, ctx->url, VOLUME, NULL, "create") != 0) {
		return E_ALLOC_FAILED;
	}

	docker_call_request_data_set(call, (char*) request_str);
	docker_call_request_method_set(call, "POST");
	docker_call_content_type_header_set(call, HEADER_JSON);

	d_err_t err = docker_call_exec(ctx, call, volume);

	json_object_put(request_obj);
	free_docker_call(call);
	return err;
}

d_err_t docker_volume_inspect(docker_context* ctx, docker_volume** volume, char* name) {
	if (name == NULL) {
		return E_INVALID_INPUT;
	}
	docker_call* call;
	if (make_docker_call(&call, ctx->url, VOLUME, NULL, name) != 0) {
		return E_ALLOC_FAILED;
	}

	d_err_t err = docker_call_exec(ctx, call, volume);

	free_docker_call(call);
	return err;
}

d_err_t docker_volume_delete(docker_context* ctx,
		const char* name, int force) {
	docker_call* call;
	if (make_docker_call(&call, ctx->url, VOLUME, NULL, name) != 0) {
		return E_ALLOC_FAILED;
	}
	if (force == 1) {
		docker_call_params_add(call, "force", str_clone("true"));
	}

	docker_call_request_method_set(call, HTTP_DELETE_STR);
	json_object *response_obj = NULL;
	d_err_t err = docker_call_exec(ctx, call, &response_obj);

	json_object_put(response_obj);
	free_docker_call(call);
	return err;
}

d_err_t docker_volumes_delete_unused(docker_context* ctx,
		arraylist** volumes_deleted,
		unsigned long* space_reclaimed, int num_label_filters, ...) {
	docker_call* call;
	if (make_docker_call(&call, ctx->url, VOLUME, NULL, "prune") != 0) {
		return E_ALLOC_FAILED;
	}

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
			if (filter_value == NULL) {
				return E_ALLOC_FAILED;
			}
			strcpy(filter_value, label_name);
			strcat(filter_value, "=");
			strcat(filter_value, label_value);
		} else {
			filter_value = (char*) calloc(strlen(label_name) + 1, sizeof(char));
			if (filter_value == NULL) {
				return E_ALLOC_FAILED;
			}
			strcpy(filter_value, label_name);
		}
		if (filter_not == 1) {
			add_filter_str(filters, "label!", filter_value);
		} else {
			add_filter_str(filters, "label", filter_value);
		}
	}
	char* filters_str = (char*)filters_to_str(filters);
	docker_call_params_add(call, "filters", filters_str);
	free(filters_str);

	docker_call_request_data_set(call, "");
	docker_call_request_method_set(call, "POST");
	docker_call_content_type_header_set(call, HEADER_JSON);

	json_object *response_obj = NULL;
	d_err_t err = docker_call_exec(ctx, call, &response_obj);

	if (docker_call_http_code_get(call) == 200) {
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

	json_object_put(response_obj);
	free_docker_call(call);
	return err;
}
