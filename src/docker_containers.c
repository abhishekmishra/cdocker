/**
 * Copyright (c) 2020 Abhishek Mishra
 * 
 * This software is released under the MIT License.
 * https://opensource.org/licenses/MIT
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
	docker_container_ps** ps, char* id, char* process_args) {
	docker_call* call;
	if (make_docker_call(&call, ctx->url, CONTAINER, id, "top") != 0) {
		return E_ALLOC_FAILED;
	}

	json_object* response_obj = NULL;
	d_err_t err = docker_call_exec(ctx, call, &response_obj);

	if (err == E_SUCCESS) {
		docker_container_ps* p;
		p = (docker_container_ps*)malloc(sizeof(docker_container_ps));
		if (!p) {
			return E_ALLOC_FAILED;
		}
		json_object* titles_obj;
		json_object_object_get_ex(response_obj, "Titles", &titles_obj);
		size_t num_titles = json_object_array_length(titles_obj);
		arraylist_new(&p->titles, &free);
		for (int i = 0; i < num_titles; i++) {
			arraylist_add(p->titles,
				(char*)json_object_get_string(
					json_object_array_get_idx(titles_obj, i)));
		}

		json_object* processes_obj;
		json_object_object_get_ex(response_obj, "Processes", &processes_obj);
		size_t num_processes = json_object_array_length(processes_obj);
		arraylist_new(&p->processes, (void (*)(void*)) & arraylist_free);
		for (int i = 0; i < num_processes; i++) {
			json_object* process_obj = json_object_array_get_idx(processes_obj,
				i);
			arraylist* process_arr;
			arraylist_new(&process_arr, &free);
			size_t num_vals = json_object_array_length(process_obj);
			for (int j = 0; j < num_vals; j++) {
				arraylist_add(process_arr,
					(char*)json_object_get_string(
						json_object_array_get_idx(process_obj, j)));
			}
			arraylist_add(p->processes, process_arr);
		}
		(*ps) = p;
	}

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

#define MULTI_PERFORM_HANG_TIMEOUT 60 * 1000

static struct timeval tvnow(void)
{
	struct timeval now;

	/* time() returns the value of time in seconds since the epoch */
	now.tv_sec = (long)time(NULL);
	now.tv_usec = 0;

	return now;
}

static long tvdiff(struct timeval newer, struct timeval older)
{
	return (newer.tv_sec - older.tv_sec) * 1000 +
		(newer.tv_usec - older.tv_usec) / 1000;
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

	dump(text, stderr, data, size, TRUE);
	return 0;
}

static size_t write_callback_for_attach(void* contents, size_t size, size_t nmemb,
	void* userp)
{
	size_t realsize = size * nmemb;

	return realsize;
}

d_err_t docker_container_attach_default(docker_context* ctx, char* id,
	char* detach_keys, int logs, int stream, int attach_stdin, int attach_stdout, int attach_stderr) {
	docker_call* call;
	if (make_docker_call(&call, ctx->url, CONTAINER, id, "attach") != 0) {
		return E_ALLOC_FAILED;
	}

	if (detach_keys != NULL) {
		docker_call_params_add(call, "detachKeys", detach_keys);
	}
	docker_call_params_add_boolean(call, "logs", logs);
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
	mcurl = curl_multi_init();
	if (!mcurl)
		return 2;

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

		// Now specify the POST data if request type is POST
		// and request_data is not NULL.
		if (docker_call_request_data_get(call) != NULL &&
			strcmp(docker_call_request_method_get(call), "POST") == 0) {
			curl_easy_setopt(curl, CURLOPT_POSTFIELDS, docker_call_request_data_get(call));
			curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, docker_call_request_data_len_get(call));
		}

		curl_easy_setopt(curl, CURLOPT_WRITEDATA, stdout);
		/* please be verbose */
		curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
		curl_easy_setopt(curl, CURLOPT_DEBUGFUNCTION, my_trace);

		/* send all data to this function  */
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback_for_attach);

		//long response_code;
		//char* effective_url;
		//curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
		//curl_easy_getinfo(curl, CURLINFO_EFFECTIVE_URL, &effective_url);

		/* Tell the multi stack about our easy handle */
		curl_multi_add_handle(mcurl, curl);

		/* Record the start time which we can use later */
		mp_start = tvnow();

		/* We start some action by calling perform right away */
		curl_multi_perform(mcurl, &still_running);

		while (still_running) {
			struct timeval timeout;
			fd_set fdread;
			fd_set fdwrite;
			fd_set fdexcep;
			int maxfd = -1;
			int rc;
			CURLMcode mc; /* curl_multi_fdset() return code */

			long curl_timeo = -1;

			/* Initialise the file descriptors */
			FD_ZERO(&fdread);
			FD_ZERO(&fdwrite);
			FD_ZERO(&fdexcep);

			/* Set a suitable timeout to play around with */
			timeout.tv_sec = 1;
			timeout.tv_usec = 0;

			curl_multi_timeout(mcurl, &curl_timeo);
			if (curl_timeo >= 0) {
				timeout.tv_sec = curl_timeo / 1000;
				if (timeout.tv_sec > 1)
					timeout.tv_sec = 1;
				else
					timeout.tv_usec = (curl_timeo % 1000) * 1000;
			}

			/* get file descriptors from the transfers */
			mc = curl_multi_fdset(mcurl, &fdread, &fdwrite, &fdexcep, &maxfd);

			if (mc != CURLM_OK) {
				fprintf(stderr, "curl_multi_fdset() failed, code %d.\n", mc);
				break;
			}

			/* On success the value of maxfd is guaranteed to be >= -1. We call
			   select(maxfd + 1, ...); specially in case of (maxfd == -1) there are
			   no fds ready yet so we call select(0, ...) --or Sleep() on Windows--
			   to sleep 100ms, which is the minimum suggested value in the
			   curl_multi_fdset() doc. */

			if (maxfd == -1) {
#ifdef _WIN32
				Sleep(100);
				rc = 0;
#else
				/* Portable sleep for platforms other than Windows. */
				struct timeval wait = { 0, 100 * 1000 }; /* 100ms */
				rc = select(0, NULL, NULL, NULL, &wait);
#endif
			}
			else {
				/* Note that on some platforms 'timeout' may be modified by select().
				   If you need access to the original value save a copy beforehand. */
				rc = select(maxfd + 1, &fdread, &fdwrite, &fdexcep, &timeout);
			}

			if (tvdiff(tvnow(), mp_start) > MULTI_PERFORM_HANG_TIMEOUT) {
				fprintf(stderr,
					"ABORTING: Since it seems that we would have run forever.\n");
				break;
			}

			switch (rc) {
			case -1:  /* select error */
				break;
			case 0:   /* timeout */
			default:  /* action */
				curl_multi_perform(mcurl, &still_running);
				break;
			}
		}

		/* Always cleanup */
		curl_multi_remove_handle(mcurl, curl);
		curl_multi_cleanup(mcurl);
		curl_easy_cleanup(curl);
	}
	// cleanup docker_result
	free_docker_result(result);

	free_docker_call(call);
	return E_SUCCESS;
}