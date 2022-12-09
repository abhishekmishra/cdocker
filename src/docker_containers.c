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
#include <arraylist.h>
#include <docker_log.h>
#include "docker_containers.h"

#include <json-c/json_object.h>
#include <json-c/json_tokener.h>
#include <json-c/linkhash.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <errno.h>
#include "docker_connection_util.h"

 /**
  * List docker containers
  *
  * \param ctx the docker context
  * \param result the result object to return
  * \param container_list array_list of containers to be returned
  * \param all all or running only
  * \param limit max containers to return
  * \param size return the size of containers in response
  * \param varargs pairs of filters char* filter_name, char* filter_value (terminated by a NULL)
  * \return error code
  */
d_err_t docker_container_list(docker_context* ctx, docker_ctr_list** container_list,
	int all, int limit, int size, ...) {
	docker_call* call;
	if (make_docker_call(&call, ctx->url, CONTAINER, NULL, "json") != 0) {
		return E_ALLOC_FAILED;
	}

	if (all > 0) {
		docker_call_params_add(call, "all", "true");
	}

	if (limit > 0) {
		char* lim_val = (char*)calloc(128, sizeof(char));
		if (lim_val == NULL) {
			return E_ALLOC_FAILED;
		}
		sprintf(lim_val, "%d", limit);
		docker_call_params_add(call, "limit", lim_val);
		free(lim_val);
	}

	if (size > 0) {
		docker_call_params_add(call, "size", "true");
	}

	va_list kvargs;
	va_start(kvargs, size);
	json_object* filters = make_filters();
	while (true) {
		char* filter_name = va_arg(kvargs, char*);
		if (filter_name == NULL) {
			break;
		}
		char* filter_value = va_arg(kvargs, char*);
		if (filter_value == NULL) {
			break;
		}
		add_filter_str(filters, filter_name, filter_value);
	}
	docker_call_params_add(call, "filters", (char*)filters_to_str(filters));

	d_err_t err = docker_call_exec(ctx, call, container_list);

	free_docker_call(call);
	return err;

}

/**
* List docker containers
*
* \param ctx the docker context
* \param container_list array_list of containers to be returned
* \param all all or running only
* \param limit max containers to return
* \param size return the size of containers in response
* \param filters filters json object as string
* \return error code
*/
MODULE_API d_err_t docker_container_list_filter_str(docker_context* ctx, docker_ctr_list** container_list, 
	int all, int limit, int size, const char* filters) {
			docker_call* call;
	if (make_docker_call(&call, ctx->url, CONTAINER, NULL, "json") != 0) {
		return E_ALLOC_FAILED;
	}

	if (all > 0) {
		docker_call_params_add(call, "all", "true");
	}

	if (limit > 0) {
		char* lim_val = (char*)calloc(128, sizeof(char));
		if (lim_val == NULL) {
			return E_ALLOC_FAILED;
		}
		sprintf(lim_val, "%d", limit);
		docker_call_params_add(call, "limit", lim_val);
		free(lim_val);
	}

	if (size > 0) {
		docker_call_params_add(call, "size", "true");
	}

	docker_call_params_add(call, "filters", filters);

	d_err_t err = docker_call_exec(ctx, call, container_list);

	free_docker_call(call);
	return err;
}

d_err_t docker_create_container(docker_context* ctx,
	char** id, docker_ctr_create_params* params) {
	docker_call* call;
	if (make_docker_call(&call, ctx->url, CONTAINER, NULL, "create") != 0) {
		return E_ALLOC_FAILED;
	}

	docker_call_request_data_set(call, (char*)json_object_to_json_string(params));
	docker_call_request_method_set(call, "POST");
	docker_call_content_type_header_set(call, HEADER_JSON);

	(*id) = NULL;
	json_object* response_obj = NULL;

	d_err_t err = docker_call_exec(ctx, call, &response_obj);

	json_object* idObj;
	if (json_object_object_get_ex(response_obj, "Id", &idObj)) {
		const char* container_id = json_object_get_string(idObj);
		(*id) = str_clone(container_id);
	}
	else {
		docker_log_debug("Id not found.");
	}

	json_object_put(response_obj);
	free_docker_call(call);
	return err;
}

docker_ctr* docker_inspect_container(docker_context* ctx, char* id, int size) {
	docker_call* call;
	if (make_docker_call(&call, ctx->url, CONTAINER, id, "json") != 0) {
		return NULL;
	}

	docker_ctr* ctr = NULL;
	d_err_t err = docker_call_exec(ctx, call, &ctr);

	free_docker_call(call);
	return ctr;
}

/**
 * List all processes in a container identified by id.
 *
 * \param ctx is a docker context
 * \param id is the container id
 * \param ps_args is the command line args to be passed to the ps command (can be NULL).
 * \return the process details as docker_container_ps list.
 */
d_err_t docker_process_list_container(docker_context* ctx,
	docker_ctr_ps** ps, char* id, char* process_args) {
	docker_call* call;
	if (make_docker_call(&call, ctx->url, CONTAINER, id, "top") != 0) {
		return E_ALLOC_FAILED;
	}

	d_err_t err = docker_call_exec(ctx, call, ps);

	free_docker_call(call);
	return err;
}

d_err_t docker_container_logs(docker_context* ctx, char** log, size_t* log_length, char* id, int follow,
	int std_out, int std_err, long since, long until, int timestamps, int tail) {
	docker_call* call;
	if (make_docker_call(&call, ctx->url, CONTAINER, id, "logs") != 0) {
		return E_ALLOC_FAILED;
	}

	if (std_out > 0) {
		docker_call_params_add(call, "stdout", "true");
	}

	if (std_err > 0) {
		docker_call_params_add(call, "stderr", "true");
	}

	if (since >= 0) {
		char* since_val = (char*)malloc(128 * sizeof(char));
		if (since_val == NULL) {
			return E_ALLOC_FAILED;
		}
		sprintf(since_val, "%ld", since);
		docker_call_params_add(call, "since", since_val);
		free(since_val);
	}

	if (until > 0) {
		char* until_val = (char*)malloc(128 * sizeof(char));
		if (until_val == NULL) {
			return E_ALLOC_FAILED;
		}
		sprintf(until_val, "%ld", until);
		docker_call_params_add(call, "until", until_val);
		free(until_val);
	}

	if (timestamps > 0) {
		docker_call_params_add(call, "timestamps", "true");
	}

	if (tail > 0) {
		char* tail_val = (char*)malloc(128 * sizeof(char));
		if (tail_val == NULL) {
			return E_ALLOC_FAILED;
		}
		sprintf(tail_val, "%d", tail);
		docker_call_params_add(call, "tail", tail_val);
		free(tail_val);
	}

	json_object* response_obj = NULL;
	d_err_t ret = docker_call_exec(ctx, call, &response_obj);
	json_object_put(response_obj);

	*log_length = docker_call_response_data_length(call);
	*log = (char*)calloc(*log_length, sizeof(char));
	memcpy(*log, docker_call_response_data_get(call), *log_length);

	free_docker_call(call);
	return ret;
}

d_err_t docker_container_logs_foreach(void* handler_args, char* log, size_t log_length,
	docker_log_line_handler* line_handler) {
	size_t current_loc = 0;
	int line_num = 0;
	if (log != NULL && log_length > 0) {
		while (current_loc < log_length) {
			if (log[current_loc] < 3) {
				int stream_type = log[current_loc];
				char* size_arr = log + current_loc + 4;
				uint32_t size = size_arr[0] << 24 |
					size_arr[1] << 16 |
					size_arr[2] << 8 |
					size_arr[3];
				current_loc += 8;

				char* current_line = (char*)calloc(size + 1, sizeof(char));
				if (current_line == NULL) {
					return E_ALLOC_FAILED;
				}
				current_line[0] = '\0';
				strncat(current_line, log + current_loc, size);
				current_line[size] = '\0';
				(*line_handler)(handler_args, stream_type, line_num, current_line);
				current_loc += size;
				free(current_line);
			}
			else {
				(*line_handler)(handler_args, 0, line_num, log);
			}
		}
	}
	return E_SUCCESS;
}
///////////// Get Container FS Changes

/**
 * Create a new container change item.
 */
d_err_t make_docker_container_change(docker_container_change** item,
	const char* path, const char* kind) {
	(*item) = (docker_container_change*)calloc(1,
		sizeof(docker_container_change));
	if (!(*item)) {
		return E_ALLOC_FAILED;
	}
	(*item)->path = str_clone(path);
	if (kind != NULL) {
		if (strcmp(kind, "0") == 0) {
			(*item)->kind = DOCKER_FS_MODIFIED;
		}
		if (strcmp(kind, "1") == 0) {
			(*item)->kind = DOCKER_FS_ADDED;
		}
		if (strcmp(kind, "2") == 0) {
			(*item)->kind = DOCKER_FS_DELETED;
		}
	}
	return E_SUCCESS;
}

void free_docker_container_change(docker_container_change* item) {
	if (item) {
		if (item->path) {
			free(item->path);
		}
		free(item);
	}
}

d_err_t make_docker_changes_list(docker_changes_list** changes_list) {
	arraylist_new(changes_list,
		(void (*)(void*)) & free_docker_container_change);
	return E_SUCCESS;
}

int docker_changes_list_add(docker_changes_list* list,
	docker_container_change* item) {
	return arraylist_add(list, item);
}

docker_container_change* docker_changes_list_get_idx(docker_changes_list* list,
	int i) {
	return (docker_container_change*)arraylist_get(list, i);
}
size_t docker_changes_list_length(docker_changes_list* list) {
	return arraylist_length(list);
}

/**
 * Get the file system changes for the docker container.
 *
 * \param ctx docker context
 * \param changes pointer to struct to be returned.
 * \param id container id
 * \return error code
 */
d_err_t docker_container_changes(docker_context* ctx, docker_changes_list** changes, char* id) {
	docker_call* call;
	if (make_docker_call(&call, ctx->url, CONTAINER, id, "changes") != 0) {
		return E_ALLOC_FAILED;
	}

	json_object* response_obj = NULL;
	d_err_t ret = docker_call_exec(ctx, call, &response_obj);

	if ((json_object_get_type(response_obj) != json_type_null)) {
		docker_log_debug("Response = %s",
			json_object_to_json_string(response_obj));

		make_docker_changes_list(changes);
		for (int i = 0; i < json_object_array_length(response_obj); i++) {
			json_object* change_obj = json_object_array_get_idx(response_obj,
				i);
			docker_container_change* change;
			make_docker_container_change(&change,
				get_attr_str(change_obj, "Path"),
				get_attr_str(change_obj, "Kind"));
			docker_changes_list_add((*changes), change);
		}
	}
	else {
		docker_log_warn("Response = %s",
			json_object_to_json_string(response_obj));

		(*changes) = NULL;
	}

	free_docker_call(call);
	return ret;
}

/////// Docker container stats

/**
 * Get stats from a running container. (the non-streaming version)
 *
 * \param ctx docker context
 * \param stats the stats object to return
 * \param id container id
 * \return error code
 */
d_err_t docker_container_get_stats(docker_context* ctx, docker_container_stats** stats,
	char* id) {
	if (id == NULL || strlen(id) == 0) {
		return E_INVALID_INPUT;
	}

	docker_call* call;
	if (make_docker_call(&call, ctx->url, CONTAINER, id, "stats") != 0) {
		return E_ALLOC_FAILED;
	}
	docker_call_params_add(call, "stream", str_clone("false"));

	d_err_t ret = docker_call_exec(ctx, call, stats);

	free_docker_call(call);
	return ret;
}

void parse_container_stats_cb(char* msg, void* cb, void* cbargs) {
	void (*docker_container_stats_cb)(docker_container_stats*,
		void*) = (void (*)(docker_container_stats*, void*))cb;
	if (msg) {
		if (docker_container_stats_cb) {
			json_object* response_obj = json_tokener_parse(msg);
			if (response_obj) {
				docker_container_stats_cb((docker_container_stats*)response_obj, cbargs);
			}
		}
	}
}

/**
 * Get stats from a running container. (the streaming version)
 *
 * \param ctx docker context
 * \param docker_container_stats_cb the callback which receives the stats object, and any client args
 * \param cbargs client args to be passed on to the callback (closure)
 * \param id container id
 * \return error code
 */
d_err_t docker_container_get_stats_cb(docker_context* ctx,
	void (*docker_container_stats_cb)(docker_container_stats* stats,
		void* cbargs), void* cbargs, char* id) {
	if (id == NULL || strlen(id) == 0) {
		return E_INVALID_INPUT;
	}

	docker_call* call;
	if (make_docker_call(&call, ctx->url, CONTAINER, id, "stats") != 0) {
		return E_ALLOC_FAILED;
	}
	docker_call_params_add(call, "stream", str_clone("true"));
	docker_call_status_cb_set(call, &parse_container_stats_cb);
	docker_call_cb_args_set(call, docker_container_stats_cb);
	docker_call_client_cb_args_set(call, cbargs);

	json_object* response_obj = NULL;
	d_err_t ret = docker_call_exec(ctx, call, &response_obj);

	free_docker_call(call);
	return ret;
}

float docker_container_stats_get_cpu_usage_percent(
	docker_container_stats* stats) {
	float cpu_percent = 0.0;
	docker_container_cpu_stats* cpu_stats = docker_container_stats_cpu_stats_get(stats);
	docker_container_cpu_stats* precpu_stats = docker_container_stats_precpu_stats_get(stats);
	int cpu_count = docker_container_cpu_stats_online_cpus_get(cpu_stats);

	long long cpu_delta = docker_cpu_usage_total_get(docker_container_cpu_stats_cpu_usage_get(cpu_stats))
		- docker_cpu_usage_total_get(docker_container_cpu_stats_cpu_usage_get(precpu_stats));
	long long sys_delta = docker_container_cpu_stats_system_cpu_usage_get(cpu_stats)
		- docker_container_cpu_stats_system_cpu_usage_get(precpu_stats);
	if (sys_delta > 0) {
		cpu_percent = (float)((100.0 * cpu_delta * cpu_count) / sys_delta);
	}
	return cpu_percent;
}

/**
 * Start a container
 *
 * \param ctx docker context
 * \param id container id
 * \param detachKeys (optional, pass NULL if not needed) key combination for detaching a container.
 * \return error code
 */
d_err_t docker_start_container(docker_context* ctx, char* id, char* detachKeys) {
	docker_call* call;
	if (make_docker_call(&call, ctx->url, CONTAINER, id, "start") != 0) {
		return E_ALLOC_FAILED;
	}

	if (detachKeys != NULL) {
		docker_call_params_add(call, "detachKeys", detachKeys);
	}
	docker_call_request_data_set(call, "");
	docker_call_request_method_set(call, HTTP_POST_STR);

	json_object* response_obj = NULL;
	d_err_t ret = docker_call_exec(ctx, call, &response_obj);
	json_object_put(response_obj);

	free_docker_call(call);
	return ret;
}

/**
 * Stop a container
 *
 * \param ctx docker context
 * \param id container id
 * \param t number of seconds to wait before killing the container
 * \return error code
 */
d_err_t docker_stop_container(docker_context* ctx, char* id, int t) {
	docker_call* call;
	if (make_docker_call(&call, ctx->url, CONTAINER, id, "stop") != 0) {
		return E_ALLOC_FAILED;
	}

	if (t > 0) {
		char* tstr = (char*)calloc(128, sizeof(char));
		if (tstr == NULL) {
			return E_ALLOC_FAILED;
		}
		sprintf(tstr, "%d", t);
		docker_call_params_add(call, "t", tstr);
		free(tstr);
	}
	docker_call_request_data_set(call, "");
	docker_call_request_method_set(call, HTTP_POST_STR);

	json_object* response_obj = NULL;
	d_err_t ret = docker_call_exec(ctx, call, &response_obj);
	json_object_put(response_obj);

	//if (ret == 304) {
	//	(*result)->message = str_clone(
	//		"container is already stopped.");
	//}

	//if (ret == 404) {
	//	(*result)->message = str_clone("container not found.");
	//}

	free_docker_call(call);
	return ret;
}

/**
 * Restart a container
 *
 * \param ctx docker context
 * \param id container id
 * \param t number of seconds to wait before killing the container
 * \return error code
 */
d_err_t docker_restart_container(docker_context* ctx, char* id, int t) {
	docker_call* call;
	if (make_docker_call(&call, ctx->url, CONTAINER, id, "restart") != 0) {
		return E_ALLOC_FAILED;
	}

	if (t > 0) {
		char* tstr = (char*)calloc(128, sizeof(char));
		if (tstr == NULL) {
			return E_ALLOC_FAILED;
		}
		sprintf(tstr, "%d", t);
		docker_call_params_add(call, "t", tstr);
		free(tstr);
	}
	docker_call_request_data_set(call, "");
	docker_call_request_method_set(call, HTTP_POST_STR);

	json_object* response_obj = NULL;
	d_err_t ret = docker_call_exec(ctx, call, &response_obj);
	json_object_put(response_obj);

	//if (ret == 404) {
	//	(*result)->message = str_clone("container not found.");
	//}

	free_docker_call(call);
	return ret;
}

/**
 * Kill a container
 *
 * \param ctx docker context
 * \param id container id
 * \param signal (optional - NULL for default i.e. SIGKILL) signal name to send
 * \return error code
 */
d_err_t docker_kill_container(docker_context* ctx, char* id, char* signal) {
	docker_call* call;
	if (make_docker_call(&call, ctx->url, CONTAINER, id, "kill") != 0) {
		return E_ALLOC_FAILED;
	}

	if (signal != NULL) {
		docker_call_params_add(call, "signal", str_clone(signal));
	}
	docker_call_request_data_set(call, "");
	docker_call_request_method_set(call, HTTP_POST_STR);

	json_object* response_obj = NULL;
	d_err_t ret = docker_call_exec(ctx, call, &response_obj);
	json_object_put(response_obj);

	//if ((*result)->http_error_code == 404) {
	//	(*result)->message = str_clone("container not found.");
	//}

	//if ((*result)->http_error_code == 409) {
	//	(*result)->message = str_clone("container is not running.");
	//}

	free_docker_call(call);
	return ret;
}

/**
 * Rename a container
 *
 * \param ctx docker context
 * \param id container id
 * \param name new name for the container
 * \return error code
 */
d_err_t docker_rename_container(docker_context* ctx, char* id, char* name) {
	docker_call* call;
	if (make_docker_call(&call, ctx->url, CONTAINER, id, "rename") != 0) {
		return E_ALLOC_FAILED;
	}

	if (name != NULL) {
		docker_call_params_add(call, "name", str_clone(name));
	}
	docker_call_request_data_set(call, "");
	docker_call_request_method_set(call, HTTP_POST_STR);

	json_object* response_obj = NULL;
	d_err_t ret = docker_call_exec(ctx, call, &response_obj);
	json_object_put(response_obj);

	//if ((*result)->http_error_code == 404) {
	//	(*result)->message = str_clone("container not found.");
	//}

	//if ((*result)->http_error_code == 409) {
	//	(*result)->message = str_clone("name is already in use");
	//}

	free_docker_call(call);
	return ret;
}

/**
 * Pause a container
 *
 * \param ctx docker context
 * \param id container id
 * \return error code
 */
d_err_t docker_pause_container(docker_context* ctx, char* id) {
	docker_call* call;
	if (make_docker_call(&call, ctx->url, CONTAINER, id, "pause") != 0) {
		return E_ALLOC_FAILED;
	}

	docker_call_request_data_set(call, "");
	docker_call_request_method_set(call, HTTP_POST_STR);

	json_object* response_obj = NULL;
	d_err_t ret = docker_call_exec(ctx, call, &response_obj);
	json_object_put(response_obj);

	//if ((*result)->http_error_code == 404) {
	//	(*result)->message = str_clone("container not found.");
	//}

	free_docker_call(call);
	return ret;
}

/**
 * Unpause a container
 *
 * \param ctx docker context
 * \param id container id
 * \return error code
 */
d_err_t docker_unpause_container(docker_context* ctx, char* id) {
	docker_call* call;
	if (make_docker_call(&call, ctx->url, CONTAINER, id, "unpause") != 0) {
		return E_ALLOC_FAILED;
	}

	docker_call_request_data_set(call, "");
	docker_call_request_method_set(call, HTTP_POST_STR);

	json_object* response_obj = NULL;
	d_err_t ret = docker_call_exec(ctx, call, &response_obj);
	json_object_put(response_obj);

	//if ((*result)->http_error_code == 404) {
	//	(*result)->message = str_clone("container not found.");
	//}

	free_docker_call(call);
	return ret;
}

/**
 * Wait for a container
 *
 * \param ctx docker context
 * \param id container id
 * \param condition (optional - NULL for default "not-running") condition to wait for
 * \return error code
 */
d_err_t docker_wait_container(docker_context* ctx, char* id, char* condition) {
	docker_call* call;
	if (make_docker_call(&call, ctx->url, CONTAINER, id, "wait") != 0) {
		return E_ALLOC_FAILED;
	}

	if (condition != NULL) {
		docker_call_params_add(call, "condition", str_clone(condition));
	}
	docker_call_request_data_set(call, "");
	docker_call_request_method_set(call, HTTP_POST_STR);

	json_object* response_obj = NULL;
	d_err_t ret = docker_call_exec(ctx, call, &response_obj);
	json_object_put(response_obj);

	free_docker_call(call);
	return ret;
}

/**
 * Remove a container
 *
 * \param ctx docker context
 * \param result pointer to docker_result
 * \param id container id
 * \param v remove volumes associated with the container
 * \param force if the container is running, kill it before removing it.
 * \param link remove specified link
 * \return error code
 */
d_err_t docker_remove_container(docker_context* ctx, char* id, int v, int force, int link) {
	docker_call* call;
	if (make_docker_call(&call, ctx->url, CONTAINER, NULL, id) != 0) {
		return E_ALLOC_FAILED;
	}

	docker_call_request_method_set(call, HTTP_DELETE_STR);

	json_object* response_obj = NULL;
	d_err_t ret = docker_call_exec(ctx, call, &response_obj);
	json_object_put(response_obj);

	free_docker_call(call);
	return ret;
}

static
void dump(const char* text,
	FILE* stream, unsigned char* ptr, size_t size,
	bool nohex)
{
	size_t i;
	size_t c;

	unsigned int width = 0x10;

	if (nohex)
		/* without the hex output, we can fit more on screen */
		width = 0x40;

	fprintf(stream, "%s, %10.10lu bytes (0x%8.8lx)\n",
		text, (unsigned long)size, (unsigned long)size);

	for (i = 0; i < size; i += width) {

		fprintf(stream, "%4.4lx: ", (unsigned long)i);

		if (!nohex) {
			/* hex not disabled, show it */
			for (c = 0; c < width; c++)
				if (i + c < size)
					fprintf(stream, "%02x ", ptr[i + c]);
				else
					fputs("   ", stream);
		}

		for (c = 0; (c < width) && (i + c < size); c++) {
			/* check for 0D0A; if found, skip past and start a new line of output */
			if (nohex && (i + c + 1 < size) && ptr[i + c] == 0x0D &&
				ptr[i + c + 1] == 0x0A) {
				i += (c + 2 - width);
				break;
			}
			fprintf(stream, "%c",
				(ptr[i + c] >= 0x20) && (ptr[i + c] < 0x80) ? ptr[i + c] : '.');
			/* check again for 0D0A, to avoid an extra \n if it's at width */
			if (nohex && (i + c + 2 < size) && ptr[i + c + 1] == 0x0D &&
				ptr[i + c + 2] == 0x0A) {
				i += (c + 3 - width);
				break;
			}
		}
		fputc('\n', stream); /* newline */
	}
	fflush(stream);
}

static
int my_trace(CURL* handle, curl_infotype type,
	unsigned char* data, size_t size,
	void* userp)
{
	const char* text;

	(void)userp;
	(void)handle; /* prevent compiler warning */

	switch (type) {
	case CURLINFO_TEXT:
		fprintf(stderr, "== Info: %s", data);
		/* FALLTHROUGH */
	default: /* in case a new one is introduced to shock us */
		return 0;

	case CURLINFO_HEADER_OUT:
		text = "=> Send header";
		break;
	case CURLINFO_DATA_OUT:
		text = "=> Send data";
		break;
	case CURLINFO_HEADER_IN:
		text = "<= Recv header";
		break;
	case CURLINFO_DATA_IN:
		text = "<= Recv data";
		break;
	}

	dump(text, stderr, data, size, 1);
	return 0;
}

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
#define USE_WINSOCK
#endif

#ifdef USE_WINSOCK
#undef  EINTR
#define EINTR    4 /* errno.h value */
#undef  EAGAIN
#define EAGAIN  11 /* errno.h value */
#undef  ENOMEM
#define ENOMEM  12 /* errno.h value */
#undef  EINVAL
#define EINVAL  22 /* errno.h value */
#endif

#ifdef USE_WINSOCK
typedef SSIZE_T ssize_t;
/*
** curl_socket_t to signed int
*/

int curlx_sktosi(curl_socket_t s)
{
	return (int)((ssize_t)s);
}

/*
** signed int to curl_socket_t
*/

curl_socket_t curlx_sitosk(int i)
{
	return (curl_socket_t)((ssize_t)i);
}

#endif /* USE_WINSOCK */

#ifdef USE_WINSOCK
/*
 * WinSock select() does not support standard file descriptors,
 * it can only check SOCKETs. The following function is an attempt
 * to re-create a select() function with support for other handle types.
 *
 * select() function with support for WINSOCK2 sockets and all
 * other handle types supported by WaitForMultipleObjectsEx() as
 * well as disk files, anonymous and names pipes, and character input.
 *
 * https://msdn.microsoft.com/en-us/library/windows/desktop/ms687028.aspx
 * https://msdn.microsoft.com/en-us/library/windows/desktop/ms741572.aspx
 */
struct select_ws_wait_data {
	HANDLE handle; /* actual handle to wait for during select */
	HANDLE event;  /* internal event to abort waiting thread */
};
static DWORD WINAPI select_ws_wait_thread(LPVOID lpParameter)
{
	struct select_ws_wait_data* data;
	HANDLE handle, handles[2];
	INPUT_RECORD inputrecord;
	LARGE_INTEGER size, pos;
	DWORD type, length;

	/* retrieve handles from internal structure */
	data = (struct select_ws_wait_data*) lpParameter;
	if (data) {
		handle = data->handle;
		handles[0] = data->event;
		handles[1] = handle;
		free(data);
	}
	else
		return (DWORD)-1;

	/* retrieve the type of file to wait on */
	type = GetFileType(handle);
	switch (type) {
	case FILE_TYPE_DISK:
		/* The handle represents a file on disk, this means:
		 * - WaitForMultipleObjectsEx will always be signalled for it.
		 * - comparison of current position in file and total size of
		 *   the file can be used to check if we reached the end yet.
		 *
		 * Approach: Loop till either the internal event is signalled
		 *           or if the end of the file has already been reached.
		 */
		while (WaitForMultipleObjectsEx(1, handles, FALSE, 0, FALSE)
			== WAIT_TIMEOUT) {
			/* get total size of file */
			length = 0;
			size.QuadPart = 0;
			size.LowPart = GetFileSize(handle, &length);
			if ((size.LowPart != INVALID_FILE_SIZE) ||
				(GetLastError() == NO_ERROR)) {
				size.HighPart = length;
				/* get the current position within the file */
				pos.QuadPart = 0;
				pos.LowPart = SetFilePointer(handle, 0, &pos.HighPart,
					FILE_CURRENT);
				if ((pos.LowPart != INVALID_SET_FILE_POINTER) ||
					(GetLastError() == NO_ERROR)) {
					/* compare position with size, abort if not equal */
					if (size.QuadPart == pos.QuadPart) {
						/* sleep and continue waiting */
						SleepEx(0, FALSE);
						continue;
					}
				}
			}
			/* there is some data available, stop waiting */
			docker_log_debug("[select_ws_wait_thread] data available on DISK: %p", handle);
			break;
		}
		break;

	case FILE_TYPE_CHAR:
		/* The handle represents a character input, this means:
		 * - WaitForMultipleObjectsEx will be signalled on any kind of input,
		 *   including mouse and window size events we do not care about.
		 *
		 * Approach: Loop till either the internal event is signalled
		 *           or we get signalled for an actual key-event.
		 */
		while (WaitForMultipleObjectsEx(2, handles, FALSE, INFINITE, FALSE)
			== WAIT_OBJECT_0 + 1) {
			/* check if this is an actual console handle */
			length = 0;
			if (GetConsoleMode(handle, &length)) {
				/* retrieve an event from the console buffer */
				length = 0;
				if (PeekConsoleInput(handle, &inputrecord, 1, &length)) {
					/* check if the event is not an actual key-event */
					if (length == 1 && inputrecord.EventType != KEY_EVENT) {
						/* purge the non-key-event and continue waiting */
						ReadConsoleInput(handle, &inputrecord, 1, &length);
						continue;
					}
				}
			}
			/* there is some data available, stop waiting */
			docker_log_debug("[select_ws_wait_thread] data available on CHAR: %p", handle);
			break;
		}
		break;

	case FILE_TYPE_PIPE:
		/* The handle represents an anonymous or named pipe, this means:
		 * - WaitForMultipleObjectsEx will always be signalled for it.
		 * - peek into the pipe and retrieve the amount of data available.
		 *
		 * Approach: Loop till either the internal event is signalled
		 *           or there is data in the pipe available for reading.
		 */
		while (WaitForMultipleObjectsEx(1, handles, FALSE, 0, FALSE)
			== WAIT_TIMEOUT) {
			/* peek into the pipe and retrieve the amount of data available */
			length = 0;
			if (PeekNamedPipe(handle, NULL, 0, NULL, &length, NULL)) {
				/* if there is no data available, sleep and continue waiting */
				if (length == 0) {
					SleepEx(0, FALSE);
					continue;
				}
				else {
					docker_log_debug("[select_ws_wait_thread] PeekNamedPipe len: %d", length);
				}
			}
			else {
				/* if the pipe has been closed, sleep and continue waiting */
				length = GetLastError();
				docker_log_debug("[select_ws_wait_thread] PeekNamedPipe error: %d", length);
				if (length == ERROR_BROKEN_PIPE) {
					SleepEx(0, FALSE);
					continue;
				}
			}
			/* there is some data available, stop waiting */
			docker_log_debug("[select_ws_wait_thread] data available on PIPE: %p", handle);
			break;
		}
		break;

	default:
		/* The handle has an unknown type, try to wait on it */
		WaitForMultipleObjectsEx(2, handles, FALSE, INFINITE, FALSE);
		docker_log_debug("[select_ws_wait_thread] data available on HANDLE: %p", handle);
		break;
	}

	return 0;
}
static HANDLE select_ws_wait(HANDLE handle, HANDLE event)
{
	struct select_ws_wait_data* data;
	HANDLE thread = NULL;

	/* allocate internal waiting data structure */
	data = malloc(sizeof(struct select_ws_wait_data));
	if (data) {
		data->handle = handle;
		data->event = event;

		/* launch waiting thread */
		thread = CreateThread(NULL, 0,
			&select_ws_wait_thread,
			data, 0, NULL);

		/* free data if thread failed to launch */
		if (!thread) {
			free(data);
		}
	}

	return thread;
}
struct select_ws_data {
	curl_socket_t fd;      /* the original input handle   (indexed by fds) */
	curl_socket_t wsasock; /* the internal socket handle  (indexed by wsa) */
	WSAEVENT wsaevent;     /* the internal WINSOCK2 event (indexed by wsa) */
	HANDLE thread;         /* the internal threads handle (indexed by thd) */
};
static int select_ws(int nfds, fd_set* readfds, fd_set* writefds,
	fd_set* exceptfds, struct timeval* timeout)
{
	DWORD milliseconds, wait, idx;
	WSANETWORKEVENTS wsanetevents;
	struct select_ws_data* data;
	HANDLE handle, * handles;
	WSAEVENT wsaevent;
	int error, fds;
	HANDLE waitevent = NULL;
	DWORD nfd = 0, thd = 0, wsa = 0;
	int ret = 0;

	/* check if the input value is valid */
	if (nfds < 0) {
		errno = EINVAL;
		return -1;
	}

	/* check if we got descriptors, sleep in case we got none */
	if (!nfds) {
		Sleep((timeout->tv_sec * 1000) + (DWORD)(((double)timeout->tv_usec) / 1000.0));
		return 0;
	}

	/* create internal event to signal waiting threads */
	waitevent = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (!waitevent) {
		errno = ENOMEM;
		return -1;
	}

	/* allocate internal array for the internal data */
	data = calloc(nfds, sizeof(struct select_ws_data));
	if (data == NULL) {
		CloseHandle(waitevent);
		errno = ENOMEM;
		return -1;
	}

	/* allocate internal array for the internal event handles */
	handles = calloc(nfds, sizeof(HANDLE));
	if (handles == NULL) {
		CloseHandle(waitevent);
		free(data);
		errno = ENOMEM;
		return -1;
	}

	/* loop over the handles in the input descriptor sets */
	for (fds = 0; fds < nfds; fds++) {
		long networkevents = 0;
		handles[nfd] = 0;

		if (FD_ISSET(fds, readfds))
			networkevents |= FD_READ | FD_ACCEPT | FD_CLOSE;

		if (FD_ISSET(fds, writefds))
			networkevents |= FD_WRITE | FD_CONNECT;

		if (FD_ISSET(fds, exceptfds))
			networkevents |= FD_OOB | FD_CLOSE;

		/* only wait for events for which we actually care */
		if (networkevents) {
			data[nfd].fd = curlx_sitosk(fds);
			if (fds == fileno(stdin)) {
				handle = GetStdHandle(STD_INPUT_HANDLE);
				handle = select_ws_wait(handle, waitevent);
				handles[nfd] = handle;
				data[thd].thread = handle;
				thd++;
			}
			else if (fds == fileno(stdout)) {
				handles[nfd] = GetStdHandle(STD_OUTPUT_HANDLE);
			}
			else if (fds == fileno(stderr)) {
				handles[nfd] = GetStdHandle(STD_ERROR_HANDLE);
			}
			else {
				wsaevent = WSACreateEvent();
				if (wsaevent != WSA_INVALID_EVENT) {
					error = WSAEventSelect(fds, wsaevent, networkevents);
					if (error != SOCKET_ERROR) {
						handle = (HANDLE)wsaevent;
						handles[nfd] = handle;
						data[wsa].wsasock = curlx_sitosk(fds);
						data[wsa].wsaevent = wsaevent;
						wsa++;
					}
					else {
						curl_socket_t socket = curlx_sitosk(fds);
						WSACloseEvent(wsaevent);
						handle = (HANDLE)socket;
						handle = select_ws_wait(handle, waitevent);
						handles[nfd] = handle;
						data[thd].thread = handle;
						thd++;
					}
				}
			}
			nfd++;
		}
	}

	/* convert struct timeval to milliseconds */
	if (timeout) {
		milliseconds = ((timeout->tv_sec * 1000) + (timeout->tv_usec / 1000));
	}
	else {
		milliseconds = INFINITE;
	}

	/* wait for one of the internal handles to trigger */
	wait = WaitForMultipleObjectsEx(nfd, handles, FALSE, milliseconds, FALSE);

	/* signal the event handle for the waiting threads */
	SetEvent(waitevent);

	/* loop over the internal handles returned in the descriptors */
	for (idx = 0; idx < nfd; idx++) {
		curl_socket_t sock = data[idx].fd;
		handle = handles[idx];
		fds = curlx_sktosi(sock);

		/* check if the current internal handle was triggered */
		if (wait != WAIT_FAILED && (wait - WAIT_OBJECT_0) <= idx &&
			WaitForSingleObjectEx(handle, 0, FALSE) == WAIT_OBJECT_0) {
			/* first handle stdin, stdout and stderr */
			if (fds == fileno(stdin)) {
				/* stdin is never ready for write or exceptional */
				FD_CLR(sock, writefds);
				FD_CLR(sock, exceptfds);
			}
			else if (fds == fileno(stdout) || fds == fileno(stderr)) {
				/* stdout and stderr are never ready for read or exceptional */
				FD_CLR(sock, readfds);
				FD_CLR(sock, exceptfds);
			}
			else {
				/* try to handle the event with the WINSOCK2 functions */
				wsanetevents.lNetworkEvents = 0;
				error = WSAEnumNetworkEvents(fds, handle, &wsanetevents);
				if (error != SOCKET_ERROR) {
					/* remove from descriptor set if not ready for read/accept/close */
					if (!(wsanetevents.lNetworkEvents & (FD_READ | FD_ACCEPT | FD_CLOSE)))
						FD_CLR(sock, readfds);

					/* remove from descriptor set if not ready for write/connect */
					if (!(wsanetevents.lNetworkEvents & (FD_WRITE | FD_CONNECT)))
						FD_CLR(sock, writefds);

					/* HACK:
					 * use exceptfds together with readfds to signal
					 * that the connection was closed by the client.
					 *
					 * Reason: FD_CLOSE is only signaled once, sometimes
					 * at the same time as FD_READ with data being available.
					 * This means that recv/sread is not reliable to detect
					 * that the connection is closed.
					 */
					 /* remove from descriptor set if not exceptional */
					if (!(wsanetevents.lNetworkEvents & (FD_OOB | FD_CLOSE)))
						FD_CLR(sock, exceptfds);
				}
			}

			/* check if the event has not been filtered using specific tests */
			if (FD_ISSET(sock, readfds) || FD_ISSET(sock, writefds) ||
				FD_ISSET(sock, exceptfds)) {
				ret++;
			}
		}
		else {
			/* remove from all descriptor sets since this handle did not trigger */
			FD_CLR(sock, readfds);
			FD_CLR(sock, writefds);
			FD_CLR(sock, exceptfds);
		}
	}

	for (fds = 0; fds < nfds; fds++) {
		if (FD_ISSET(fds, readfds))
			docker_log_debug("select_ws: %d is readable", fds);

		if (FD_ISSET(fds, writefds))
			docker_log_debug("select_ws: %d is writable", fds);

		if (FD_ISSET(fds, exceptfds))
			docker_log_debug("select_ws: %d is excepted", fds);
	}

	for (idx = 0; idx < wsa; idx++) {
		WSAEventSelect(data[idx].wsasock, NULL, 0);
		WSACloseEvent(data[idx].wsaevent);
	}

	for (idx = 0; idx < thd; idx++) {
		WaitForSingleObject(data[idx].thread, INFINITE);
		CloseHandle(data[idx].thread);
	}

	CloseHandle(waitevent);

	free(handles);
	free(data);

	return ret;
}
#define select(a,b,c,d,e) select_ws(a,b,c,d,e)
#endif  /* USE_WINSOCK */

/* Auxiliary function that waits on the socket. */
static int wait_on_socket(curl_socket_t sockfd, int for_recv, long timeout_ms)
{
	struct timeval tv;
	fd_set infd, outfd, errfd;
	int res;

	tv.tv_sec = timeout_ms / 1000;
	tv.tv_usec = (timeout_ms % 1000) * 1000;

	FD_ZERO(&infd);
	FD_ZERO(&outfd);
	FD_ZERO(&errfd);

	FD_SET(sockfd, &errfd); /* always check for error */

	if (for_recv) {
		FD_SET(sockfd, &infd);
	}
	else {
		FD_SET(sockfd, &outfd);
	}

	/* select() returns the number of signalled sockets or -1 */
	res = select((int)sockfd + 1, &infd, &outfd, &errfd, &tv);
	if (FD_ISSET(sockfd, &errfd)) {
		return -1;
	}
	return res;
}

d_err_t docker_container_attach_default(docker_context* ctx, char* id,
	char* detach_keys, int logs, int stream, int attach_stdin, int attach_stdout, int attach_stderr) {
#ifdef USE_WINSOCK
	HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);
	DWORD mode = 0;
	GetConsoleMode(hStdin, &mode);
	SetConsoleMode(hStdin, mode & (~ENABLE_ECHO_INPUT));
#endif
	docker_call* call;
	if (make_docker_call(&call, ctx->url, CONTAINER, id, "attach") != 0) {
		return E_ALLOC_FAILED;
	}

	if (detach_keys != NULL) {
		docker_call_params_add(call, "detachKeys", detach_keys);
	}
	if (logs > 0) {
		docker_call_params_add_boolean(call, "logs", logs);
	}
	docker_call_params_add_boolean(call, "stream", stream);
	docker_call_params_add_boolean(call, "stdin", attach_stdin);
	docker_call_params_add_boolean(call, "stdout", attach_stdout);
	docker_call_params_add_boolean(call, "stderr", attach_stderr);

	docker_call_request_data_set(call, "");
	docker_call_request_method_set(call, HTTP_POST_STR);

	// Execute the HTTP Request for Attach
	time_t start, end;
	d_err_t err = E_SUCCESS;
	docker_result* result;

	// set the start time
	start = time(NULL);

	// allocate the docker result object
	err = new_docker_result(&result);
	if (err != E_SUCCESS) {
		return err;
	}

	char* docker_site_url = call->site_url;

	//call->site_url = "/";
	//char* service_url = docker_call_get_url(call);
	//if (service_url == NULL) {
	//	return E_ALLOC_FAILED;
	//}

	CURL* curl;
	CURLcode res;
	struct curl_slist* headers = NULL;
	CURLM* mcurl;
	int still_running = 1;
	struct timeval mp_start;

	/* get a curl handle */
	curl = curl_easy_init();

	char* svc_url = docker_call_get_svc_url(call);
	char* request = (char*)calloc(2048 + strlen(svc_url), sizeof(char));
	sprintf(request, "POST /v1.40/%s HTTP/1.1\r\nHost: %s\r\nContent-Length: 0\r\nContent-Type: text/plain\r\nUpgrade: tcp\r\nConnection: Upgrade\r\n\r\n", svc_url, "localhost");
	size_t request_len = strlen(request);

	if (curl)
	{
		curl_socket_t sockfd;
		size_t nsent_total = 0;

		// Set the URL
		char* docker_url = docker_call_get_url(call);
		if (is_unix_socket(ctx->url))
		{
			curl_easy_setopt(curl, CURLOPT_UNIX_SOCKET_PATH, ctx->url);
		}
		curl_easy_setopt(curl, CURLOPT_URL, docker_url);

		// Set content type headers if any
		if (docker_call_content_type_header_get(call) != NULL) {
			headers = curl_slist_append(headers, "Expect:");
			headers = curl_slist_append(headers, docker_call_content_type_header_get(call));
			curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
		}

		/* Do not do the transfer - only connect to host */
		curl_easy_setopt(curl, CURLOPT_CONNECT_ONLY, 1L);
		res = curl_easy_perform(curl);

		if (res != CURLE_OK) {
			printf("Error: %s\n", curl_easy_strerror(res));
			return 1;
		}

		/* Extract the socket from the curl handle - we'll need it for waiting. */
		res = curl_easy_getinfo(curl, CURLINFO_ACTIVESOCKET, &sockfd);

		if (res != CURLE_OK) {
			printf("Error: %s\n", curl_easy_strerror(res));
			return 1;
		}

		printf("Sending request.\n");

		do {
			/* Warning: This example program may loop indefinitely.
			 * A production-quality program must define a timeout and exit this loop
			 * as soon as the timeout has expired. */
			size_t nsent;
			do {
				nsent = 0;
				res = curl_easy_send(curl, request + nsent_total,
					request_len - nsent_total, &nsent);
				nsent_total += nsent;

				if (res == CURLE_AGAIN && !wait_on_socket(sockfd, 0, 60000L)) {
					printf("Error: timeout.\n");
					return 1;
				}
			} while (res == CURLE_AGAIN);

			if (res != CURLE_OK) {
				printf("Error: %s\n", curl_easy_strerror(res));
				return 1;
			}

			printf("Sent %" CURL_FORMAT_CURL_OFF_T " bytes.\n",
				(curl_off_t)nsent);
			printf("Request: %s\n", request);

		} while (nsent_total < request_len);

		printf("Reading response.\n");

		//read once
		do {
			char buf[1025];
			size_t nread;
			do {
				nread = 0;
				res = curl_easy_recv(curl, buf, sizeof(buf) - 1, &nread);

				if (res == CURLE_AGAIN && !wait_on_socket(sockfd, 1, 60000L)) {
					printf("Error: timeout.\n");
					return 1;
				}
			} while (res == CURLE_AGAIN);

			if (res != CURLE_OK) {
				printf("Error: %s\n", curl_easy_strerror(res));
				break;
			}

			if (nread == 0) {
				/* end of the response */
				break;
			}

			buf[nread] = NULL;
			printf("Received %" CURL_FORMAT_CURL_OFF_T " bytes.\n",
				(curl_off_t)nread);
			printf("Response: %s\n", buf);
		} while (false);

		docker_start_container(ctx, id, NULL);

		/* Warning: This example program may loop indefinitely (see above). */
		char buf[1025];
		size_t nread;
		int timeout_ms = 10000L;
		res = 0;
		do {
			nread = 0;
			res = curl_easy_recv(curl, buf, sizeof(buf) - 1, &nread);
			if (nread > 0) {
				buf[nread] = NULL;
				//printf("Received %" CURL_FORMAT_CURL_OFF_T " bytes.\n",
				//	(curl_off_t)nread);
				//printf("Response: %s\n", buf);
				printf("%s", buf);
			}

			if (res == CURLE_AGAIN) {
				struct timeval tv;
				fd_set infd, outfd, errfd;
				int maxfd = fileno(stdin);

				tv.tv_sec = timeout_ms / 1000;
				tv.tv_usec = (timeout_ms % 1000) * 1000;

				FD_ZERO(&infd);
				FD_ZERO(&outfd);
				FD_ZERO(&errfd);

				FD_SET(sockfd, &errfd);
				FD_SET(sockfd, &infd);
				FD_SET(fileno(stdin), &infd);
				if (sockfd > maxfd) {
					maxfd = sockfd;
				}

				/* select() returns the number of signalled sockets or -1 */
				res = select((int)maxfd + 1, &infd, &outfd, &errfd, &tv);

				if ((res < 0) && (errno > 0))
				{
					printf("select error\n");
					perror(strerror(errno));
				}

				if (res > 0)
				{
					//printf("select on reads returned true.\n");
					//check if error in read socket
					if (FD_ISSET(sockfd, &errfd)) {
						//printf("We don't have inbound socket anymore\n");
						res = -1;
					}
					// is stdin available for read?
					else if (FD_ISSET(fileno(stdin), &infd)) {
						//printf("stdin has text to read\n");
						fgets(buf, 1024, stdin);
						//scanf("%1024s", buf);
						//buf[0] = getchar();
						//buf[1] = NULL;
						//printf("Read from stdin %s\n", buf);
						//printf("Done reading stdin.\n");
						request = buf;
						request_len = strlen(buf);

						nsent_total = 0;
						do {
							/* Warning: This example program may loop indefinitely.
							 * A production-quality program must define a timeout and exit this loop
							 * as soon as the timeout has expired. */
							size_t nsent;
							do {
								nsent = 0;
								//printf("trying send\n");
								res = curl_easy_send(curl, request + nsent_total,
									request_len - nsent_total, &nsent);
								nsent_total += nsent;

								if (res == CURLE_AGAIN && !wait_on_socket(sockfd, 0, 10000L)) {
									printf("Error: timeout.\n");
									return 1;
								}
							} while (res == CURLE_AGAIN);

							if (res != CURLE_OK) {
								printf("Error: %s\n", curl_easy_strerror(res));
								return 1;
							}

							//printf("Sent %" CURL_FORMAT_CURL_OFF_T " bytes.\n",
							//	(curl_off_t)nsent);
							//printf("Request: %s\n", request);

						} while (nsent_total < request_len);
						res = wait_on_socket(sockfd, 1, 60000L);
					}
					else {
						printf("All done.");
					}
				}
			}
		} while (res >= 0);

		/* always cleanup */
		curl_easy_cleanup(curl);
	}
	// cleanup docker_result
	free_docker_result(result);

	free_docker_call(call);
	return E_SUCCESS;
}