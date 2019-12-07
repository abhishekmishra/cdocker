/*
 * docker_containers.h
 *
 *  Created on: 10-Dec-2018
 *      Author: abhishek
 */

 /**
  * \file docker_containers.h
  * \brief Docker Containers API
  */

#ifndef DOCKER_CONTAINERS_H_
#define DOCKER_CONTAINERS_H_

#ifdef __cplusplus  
extern "C" {
#endif

#include <arraylist.h>
#include "docker_result.h"
#include "docker_connection_util.h"
#include "docker_util.h"

typedef json_object										docker_ctr_port;
#define free_docker_ctr_port(ctr_port)					json_object_put((json_object*) ctr_port)
#define docker_ctr_port_type_get(ctr_port)				get_attr_str((json_object*)ctr_port, "Type")
#define docker_ctr_port_private_port_get(ctr_port)		get_attr_long((json_object*)ctr_port, "PrivatePort")
#define docker_ctr_port_public_port_get(ctr_port)		get_attr_long((json_object*)ctr_port, "PublicPort")

typedef json_object										docker_ctr_ls_item;
#define free_docker_ctr_ls_item(ctr)					json_object_put((json_object*) ctr)
#define docker_ctr_ls_item_id_get(ctr)					get_attr_str((json_object*)ctr, "Id")
#define docker_ctr_ls_item_names_get(ctr)				get_attr_json_object((json_object*)ctr, "Names")
#define docker_ctr_ls_item_names_length(ctr)			json_object_array_length(docker_ctr_ls_item_names_get(ctr))
#define docker_ctr_ls_item_names_get_idx(ctr, i)		(char*) json_object_get_string(json_object_array_get_idx(docker_ctr_ls_item_names_get(ctr), i))
#define docker_ctr_ls_item_image_get(ctr)				get_attr_str((json_object*)ctr, "Image")
#define docker_ctr_ls_item_image_id_get(ctr)			get_attr_str((json_object*)ctr, "ImageID")
#define docker_ctr_ls_item_command_get(ctr)				get_attr_str((json_object*)ctr, "Command")
#define docker_ctr_ls_item_created_get(ctr)				get_attr_long_long((json_object*)ctr, "Created")
#define docker_ctr_ls_item_state_get(ctr)				get_attr_str((json_object*)ctr, "State")
#define docker_ctr_ls_item_status_get(ctr)				get_attr_str((json_object*)ctr, "Status")
#define docker_ctr_ls_item_size_rw_get(ctr)				get_attr_long_long((json_object*)ctr, "SizeRw")
#define docker_ctr_ls_item_size_root_fs_get(ctr)		get_attr_long_long((json_object*)ctr, "SizeRootFs")
#define docker_ctr_ls_item_ports_get(ctr)				get_attr_json_object((json_object*)ctr, "Ports")
#define docker_ctr_ls_item_ports_length(ctr)			json_object_array_length(docker_ctr_ls_item_ports_get(ctr))
#define docker_ctr_ls_item_ports_get_idx(ctr, i)		(docker_ctr_port*) json_object_array_get_idx(docker_ctr_ls_item_ports_get(ctr), i)
#define docker_ctr_ls_item_ports_labels_get(vol)		get_attr_json_object((json_object*)vol, "Labels")
#define docker_ctr_ls_item_ports_labels_foreach(vol)	json_object_object_foreach(docker_ctr_ls_item_ports_labels_get(vol), key, val)

typedef json_object										docker_ctr_list;
#define free_docker_ctr_list(ctr_ls)					json_object_put(ctr_ls)
#define docker_ctr_list_length(ctr_ls)					json_object_array_length(ctr_ls)
#define docker_ctr_list_get_idx(ctr_ls, i)				(docker_ctr*) json_object_array_get_idx(ctr_ls, i)

/**
	* List docker containers
	*
	* \param ctx the docker context
	* \param container_list array_list of containers to be returned
	* \param all all or running only
	* \param limit max containers to return
	* \param size return the size of containers in response
	* \param varargs pairs of filters char* filter_name, char* filter_value (terminated by a NULL)
	* \return error code
	*/
d_err_t docker_container_list(docker_context* ctx, docker_ctr_list** container_list, 
	int all, int limit, int size, ...);

typedef json_object													docker_ctr_create_params;
#define make_docker_ctr_create_params								(docker_ctr_create_params*)json_object_new_object
#define free_docker_ctr_create_params(ctr_create)					json_object_put(ctr_create)
#define docker_ctr_create_params_image_set(ctr_create, img)			set_attr_str(ctr_create, "Image", img)
#define docker_ctr_create_params_entrypoint_set(ctr_create, entry)	set_attr_str(ctr_create, "Entrypoint", entry)
#define docker_ctr_create_params_cmd_add(ctr_create, cmd)			add_array_str(ctr_create, "Cmd", cmd)
#define docker_ctr_create_params_env_add(ctr_create, env)			add_array_str(ctr_create, "Env", env)

d_err_t docker_create_container(docker_context* ctx,
	char** id, docker_ctr_create_params* params);

typedef json_object									docker_ctr;
#define free_docker_ctr(ctr)						json_object_put((json_object*) ctr)
#define docker_ctr_id_get(ctr)						get_attr_str((json_object*)ctr, "Id")
#define docker_ctr_image_get(ctr)					get_attr_str((json_object*)ctr, "Image")
#define docker_ctr_name_get(ctr)					get_attr_str((json_object*)ctr, "Name")
#define docker_ctr_path_get(ctr)					get_attr_str((json_object*)ctr, "Path")
#define docker_ctr_created_get(ctr)					get_attr_long_long((json_object*)ctr, "Created")
#define docker_ctr_state_get(ctr)					get_attr_str((json_object*)ctr, "State")
#define docker_ctr_status_get(ctr)					get_attr_str((json_object*)ctr, "Status")
#define docker_ctr_size_rw_get(ctr)					get_attr_long_long((json_object*)ctr, "SizeRw")
#define docker_ctr_size_root_fs_get(ctr)			get_attr_long_long((json_object*)ctr, "SizeRootFs")
#define docker_ctr_ports_get(ctr)					get_attr_json_object((json_object*)ctr, "Ports")
#define docker_ctr_ports_length(ctr)				json_object_array_length(docker_ctr_ports_get(ctr))
#define docker_ctr_ports_get_idx(ctr, i)			(docker_ctr_port*) json_object_array_get_idx(docker_ctr_ports_get(ctr), i)
#define docker_ctr_ports_labels_get(vol)			get_attr_json_object((json_object*)vol, "Labels")
#define docker_ctr_ports_labels_foreach(vol)		json_object_object_foreach(docker_ctr_ports_labels_get(vol), key, val)

docker_ctr* docker_inspect_container(docker_context* ctx, char* id, int size);

/**
	* Struct which holds the titles of the process line, and the details of all processes.
	*/
typedef struct docker_container_ps_t {
	arraylist* titles;

	//each item in this list is another list with values for
	//each process
	arraylist* processes;
} docker_container_ps;

/**
	* List all processes in a container identified by id.
	*
	* \param ctx is a docker context
	* \param result is the pointer to the result to be returned.
	* \param ps is the pointer to the process list to be returned.
	* \param id is the container id
	* \param ps_args is the command line args to be passed to the ps command (can be NULL).
	* \return the process details as docker_container_ps list.
	* \return error code of the result
	*/
d_err_t docker_process_list_container(docker_context* ctx,
	docker_result** result, docker_container_ps** ps, char* id,
	char* process_args);

///////////// Get Container Logs

/**
	* Get the logs for the docker container.
	*
	* \param ctx docker context
	* \param result pointer to docker_result
	* \param log pointer to string to be returned.
	* \param id container id
	* \param follow - this param has no effect for now, as socket support is not implemented.
	* \param stdout whether to get stdout (>0 means yes)
	* \param stderr whether to get stdin (>0 means yes)
	* \param since time since which the logs are to be fetched (unix timestamp)
	* \param until time till which the logs are to be fetched (unix timestamp)
	* \param timestamps add timestamps to log lines (>0 means yes)
	* \param tail 0 means all, any positive number indicates the number of lines to fetch.
	* \return error code
	*/
d_err_t docker_container_logs(docker_context* ctx, docker_result** result,
	char** log, char* id, int follow, int std_out, int std_err, long since,
	long until, int timestamps, int tail);

///////////// Get Container FS Changes

typedef enum {
	DOCKER_FS_MODIFIED = 0, DOCKER_FS_ADDED = 1, DOCKER_FS_DELETED = 2
} change_kind;

typedef struct docker_container_change_t {
	char* path;
	change_kind kind;
} docker_container_change;

/**
	* Create a new container change item.
	*/
d_err_t make_docker_container_change(docker_container_change** item,
	const char* path, const char* kind);

void free_docker_container_change(docker_container_change* item);

typedef arraylist docker_changes_list;

d_err_t make_docker_changes_list(docker_changes_list** changes_list);
int docker_changes_list_add(docker_changes_list* list,
	docker_container_change* item);
docker_container_change* docker_changes_list_get_idx(docker_changes_list* list,
	int i);
size_t docker_changes_list_length(docker_changes_list* list);

/**
	* Get the file system changes for the docker container.
	*
	* \param ctx docker context
	* \param result pointer to docker_result
	* \param changes pointer to struct to be returned.
	* \param id container id
	* \return error code
	*/
d_err_t docker_container_changes(docker_context* ctx, docker_result** result,
	docker_changes_list** changes, char* id);

/////// Docker container stats

typedef json_object													docker_percpu_usage_list;
#define docker_percpu_usage_list_length(percpu_usage_ls)			json_object_array_length(percpu_usage_ls)
#define docker_percpu_usage_list_get_idx(percpu_usage_ls, i)		(docker_percpu_usage*) json_object_array_get_idx(percpu_usage_ls, i)

typedef json_object													docker_cpu_usage;
#define free_docker_cpu_usage(cpu_usg)								json_object_put((json_object*) cpu_usg)
#define docker_cpu_usage_usermode_get(cpu_usg)						get_attr_long_long((json_object*)cpu_usg, "usage_in_usermode")
#define docker_cpu_usage_total_get(cpu_usg)							get_attr_long_long((json_object*)cpu_usg, "total_usage")
#define docker_cpu_usage_kernelmode_get(cpu_usg)					get_attr_long_long((json_object*)cpu_usg, "usage_in_kernelmode")
#define docker_cpu_usage_percpu_usage_get(cpu_usg)					(docker_percpu_usage_list*)get_attr_json_object((json_object*)cpu_usg, "percpu_usage")

typedef json_object													docker_throttling_data;
#define free_docker_throttling_data(throt)							json_object_put((json_object*) throt)
#define docker_throttling_data_periods_get(throt)					get_attr_long_long((json_object*)throt, "periods")
#define docker_throttling_data_throttled_periods_get(throt)			get_attr_long_long((json_object*)throt, "throttled_periods")
#define docker_throttling_data_throttled_time_get(throt)			get_attr_long_long((json_object*)throt, "throttled_time")

typedef json_object													docker_container_cpu_stats;
#define free_docker_container_cpu_stats(cpu_stats)					json_object_put((json_object*) cpu_stats)
#define docker_container_cpu_stats_system_cpu_usage_get(cpu_stats)	get_attr_long_long((json_object*)cpu_stats, "system_cpu_usage")
#define docker_container_cpu_stats_online_cpus_get(cpu_stats)		get_attr_long((json_object*)cpu_stats, "online_cpus")
#define docker_container_cpu_stats_cpu_usage_get(cpu_stats)			(docker_cpu_usage*)get_attr_json_object((json_object*)cpu_stats, "cpu_usage")
#define docker_container_cpu_stats_throttling_data_get(cpu_stats)	(docker_throttling_data*)get_attr_json_object((json_object*)cpu_stats, "throttling_data")

typedef json_object													docker_container_mem_stats;
#define free_docker_container_mem_stats(mem_stats)					json_object_put((json_object*) mem_stats)
#define docker_container_mem_stats_max_usage_get(mem_stats)			get_attr_long_long((json_object*)mem_stats, "max_usage")
#define docker_container_mem_stats_usage_get(mem_stats)				get_attr_long_long((json_object*)mem_stats, "usage")
#define docker_container_mem_stats_failcnt_get(mem_stats)			get_attr_long_long((json_object*)mem_stats, "failcnt")
#define docker_container_mem_stats_limit_get(mem_stats)				get_attr_long_long((json_object*)mem_stats, "limit")
#define docker_container_mem_stats_stats_get(mem_stats)				get_attr_json_object((json_object*)mem_stats, "stats")
#define docker_container_mem_stats_stats_foreach(mem_stats)			json_object_object_foreach(docker_container_mem_stats_stats_get(mem_stats), key, val)

typedef json_object													docker_container_net_stats;
#define free_docker_container_net_stats(net_stats)					json_object_put((json_object*) net_stats)
#define docker_container_net_stats_foreach(net_stats)				json_object_object_foreach(net_stats, key, val)
#define docker_container_net_stats_stats_foreach(net_stats)			json_object_object_foreach(net_stats_val, key, val)

typedef json_object													docker_container_pid_stats;
#define free_docker_container_pid_stats(pid_stats)					json_object_put((json_object*) pid_stats)
#define docker_container_pid_stats_foreach(pid_stats)				json_object_object_foreach(pid_stats, key, val)

typedef json_object													docker_container_stats;
#define free_docker_container_stats(stats)							json_object_put((json_object*) stats)
#define docker_container_stats_read_get(stats)						get_attr_str((json_object*)stats, "read")
#define docker_container_stats_pid_stats_get(stats)					(docker_container_pid_stats*)get_attr_json_object((json_object*)stats, "pid_stats")
#define docker_container_stats_networks_get(stats)					(docker_container_net_stats*)get_attr_json_object((json_object*)stats, "networks")
#define docker_container_stats_mem_stats_get(stats)					(docker_container_mem_stats*)get_attr_json_object((json_object*)stats, "memory_stats")
#define docker_container_stats_cpu_stats_get(stats)					(docker_container_cpu_stats*)get_attr_json_object((json_object*)stats, "cpu_stats")
#define docker_container_stats_precpu_stats_get(stats)				(docker_container_cpu_stats*)get_attr_json_object((json_object*)stats, "precpu_stats")

/**
	* Get stats from a running container. (the non-streaming version)
	*
	* \param ctx docker context
	* \param result pointer to docker_result
	* \param stats the stats object to return
	* \param id container id
	* \return error code
	*/
d_err_t docker_container_get_stats(docker_context* ctx, docker_result** result,
	docker_container_stats** stats, char* id);

/**
	* Get stats from a running container. (the streaming version)
	*
	* \param ctx docker context
	* \param result pointer to docker_result
	* \param docker_container_stats_cb the callback which receives the stats object, and any client args
	* \param cbargs client args to be passed on to the callback (closure)
	* \param id container id
	* \return error code
	*/
d_err_t docker_container_get_stats_cb(docker_context* ctx,
	docker_result** result,
	void (*docker_container_stats_cb)(docker_container_stats* stats,
		void* cbargs), void* cbargs, char* id);

float docker_container_stats_get_cpu_usage_percent(
		docker_container_stats* stats);

///////////// Get Container Start, Stop, Restart, Kill, Rename, Pause, Unpause, Wait

/**
	* Start a container
	*
	* \param ctx docker context
	* \param result pointer to docker_result
	* \param id container id
	* \param detachKeys (optional) key combination for detaching a container.
	* \return error code
	*/
d_err_t docker_start_container(docker_context* ctx, docker_result** result,
	char* id, char* detachKeys);

/**
	* Stop a container
	*
	* \param ctx docker context
	* \param result pointer to docker_result
	* \param id container id
	* \param t number of seconds to wait before killing the container
	* \return error code
	*/
d_err_t docker_stop_container(docker_context* ctx, docker_result** result,
	char* id, int t);

/**
	* Restart a container
	*
	* \param ctx docker context
	* \param result pointer to docker_result
	* \param id container id
	* \param t number of seconds to wait before killing the container
	* \return error code
	*/
d_err_t docker_restart_container(docker_context* ctx, docker_result** result,
	char* id, int t);

/**
	* Kill a container
	*
	* \param ctx docker context
	* \param result pointer to docker_result
	* \param id container id
	* \param signal (optional - NULL for default i.e. SIGKILL) signal name to send
	* \return error code
	*/
d_err_t docker_kill_container(docker_context* ctx, docker_result** result,
	char* id, char* signal);

/**
	* Rename a container
	*
	* \param ctx docker context
	* \param result pointer to docker_result
	* \param id container id
	* \param name new name for the container
	* \return error code
	*/
d_err_t docker_rename_container(docker_context* ctx, docker_result** result,
	char* id, char* name);

/**
	* Pause a container
	*
	* \param ctx docker context
	* \param result pointer to docker_result
	* \param id container id
	* \return error code
	*/
d_err_t docker_pause_container(docker_context* ctx, docker_result** result,
	char* id);

/**
	* Unpause a container
	*
	* \param ctx docker context
	* \param result pointer to docker_result
	* \param id container id
	* \return error code
	*/
d_err_t docker_unpause_container(docker_context* ctx, docker_result** result,
	char* id);

//TODO: implement wait status code in API.
/**
	* Wait for a container
	*
	* \param ctx docker context
	* \param result pointer to docker_result
	* \param id container id
	* \param condition (optional - NULL for default "not-running") condition to wait for
	* \return error code
	*/
d_err_t docker_wait_container(docker_context* ctx, docker_result** result,
	char* id, char* condition);


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
d_err_t docker_remove_container(docker_context* ctx, docker_result** result,
	char* id, int v, int force, int link);

#ifdef __cplusplus 
}
#endif

#endif /* DOCKER_CONTAINERS_H_ */
