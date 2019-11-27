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

	typedef enum docker_container_port_type_t {
		CONTAINER_PORT_TYPE_TCP = 1
	} docker_container_port_type;

	/**
	 * This struct holds the public port, private port and port type.
	 */
	typedef struct docker_container_ports_t {
		long private_port;
		long public_port;
		docker_container_port_type type;
	} docker_container_ports;

	/**
	 * Create a docker_container_ports instance.
	 *
	 * \param ports the instance to create.
	 * \param priv the private port
	 * \param pub the public port
	 * \param type port type
	 * \return error code
	 */
	d_err_t make_docker_container_ports(docker_container_ports** ports, long priv,
		long pub, char* type);

	/**
	 * Free docker_container_ports instance.
	 *
	 * \param ports the ports struct to free.
	 */
	void free_docker_container_ports(docker_container_ports* ports);

	/**
	 * A container label struct: Key value pair.
	 */
	typedef struct docker_container_label_t {
		char* key;
		char* value;
	} docker_container_label;

	/**
	 * Create a new label.
	 *
	 * \param label ptr to return value.
	 * \param key
	 * \param value
	 * \return error code
	 */
	d_err_t make_docker_container_label(docker_container_label** label,
		const char* key, const char* value);

	/**
	 * Free docker_container_label.
	 *
	 * \param label the label instance.
	 */
	void free_docker_container_label(docker_container_label* label);

	/**
	 * Struct for the container host config.
	 */
	typedef struct docker_container_host_config_t {
		long max_iops;
		long max_iobps;
		long blkio_weight;
		json_object* blkio_device_read_bps;
		json_object* blkio_device_write_bps;
		json_object* blkio_device_read_iops;
		json_object* blkio_device_write_iops;
		char* container_id_file;
		char* cpuset_spus;
		char* cpuset_mems;
		int cpu_percent;
		int cpu_shares;
		long cpu_period;
		long cpu_realtime_period;
		long cpu_realtime_runtime;
		arraylist* devices;
		char* ipc_mode;
		arraylist* lxc_conf;
		long memory;
		long memory_swap;
		long memory_reservation;
		long kernel_memory;
		bool oom_kill_disable;
		bool oom_score_adj;
		char* network_mode;
		char* pid_mode;
		json_object* port_bindings;
		bool privileged;
		bool readonly_rootfs;
		bool publish_all_ports;
		json_object* restart_policy;
		json_object* log_config;
		json_object* sysctls;
		json_object* ulimits;
		char* volume_driver;
		long shm_size;
	}docker_container_host_config;


	/**
	 * Create a new host config
	 *
	 * \param pointer to return instance
	 * \param network_mode
	 * \return error_code
	 */
	d_err_t make_docker_container_host_config(
		docker_container_host_config** host_config, const char* network_mode);

	/**
	 * Free the docker_container_host_config
	 * \param host_config
	 */
	void free_docker_container_host_config(
		docker_container_host_config* host_config);

	/**
	 * Struct for the network settings item
	 */
	typedef struct docker_container_network_settings_item_t {
		char* name;
		char* network_id;
		char* endpoint_id;
		char* gateway;
		char* ip_address;
		int ip_prefix_len;
		char* ipv6_gateway;
		char* global_ipv6_address;
		int global_ipv6_prefix_len;
		char* mac_address;
	} docker_container_network_settings_item;

	/**
	 * Create a new network settings item
	 */
	d_err_t make_docker_container_network_settings_item(
		docker_container_network_settings_item** item, const char* name,
		const char* network_id, const char* endpoint_id, const char* gateway,
		const char* ip_address, int ip_prefix_len, const char* ipv6_gateway,
		const char* global_ipv6_address, int global_ipv6_prefix_len,
		const char* mac_address);

	void free_docker_container_network_settings_item(
		docker_container_network_settings_item* settings_item);

	typedef struct docker_container_mount_t {
		char* name;
		char* type;
		char* source;
		char* destination;
		char* driver;
		char* mode;
		int rw;
		char* propagation;
	} docker_container_mount;

	/**
	 * Create a new mount object
	 */
	d_err_t make_docker_container_mount(docker_container_mount** mount,
		const char* name, const char* type, const char* source,
		const char* destination, const char* driver, const char* mode,
		const int rw, const char* propagation);

	void free_docker_container_mount(docker_container_mount* mount);

	typedef struct docker_container_list_item_t {
		char* id;
		char* image;
		char* image_id;
		char* command;
		long long created;
		char* state;
		char* status;
		long long size_rw;
		long long size_root_fs;
		docker_container_host_config* hostConfig;
		arraylist* names;
		arraylist* ports;
		arraylist* labels;
		arraylist* network_settings;
		arraylist* mounts;
	} docker_container_list_item;

	/**
	 * Create a new containers list item.
	 */
	d_err_t make_docker_containers_list_item(docker_container_list_item** item,
		const char* id, const char* image, const char* image_id,
		const char* command, const long long created, const char* state,
		const char* status, const long long size_rw,
		const long long size_root_fs,
		const docker_container_host_config* hostConfig);

	void free_docker_container_list_item(docker_container_list_item* item);

	typedef arraylist docker_containers_list;

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
	d_err_t docker_container_list(docker_context* ctx, docker_result** result,
		docker_containers_list** container_list, int all, int limit, int size,
		...);

	d_err_t make_docker_containers_list(docker_containers_list** container_list);
	int docker_containers_list_add(docker_containers_list* list,
		docker_container_list_item* item);
	docker_container_list_item* docker_containers_list_get_idx(
		docker_containers_list* list, int i);
	size_t docker_containers_list_length(docker_containers_list* list);

	typedef struct health_config_t {
		char** test;
		int num_test;
		int interval;
		int timeout;
		int retries;
		int start_period;
	} health_config;

	typedef struct docker_create_container_params_t {
		char* hostname;
		char* domainname;
		char* user;
		int attach_stdin;
		int attach_stdout;
		int attach_stderr;
		char* exposed_ports;
		int tty;
		int open_stdin;
		int stdin_once;
		char** env;
		int num_env;
		char** cmd;
		int num_cmd;
		health_config* health_check;
		int args_escaped;
		char* image;
		//TODO: Add type for volumes;
		char* volumes;
		char* working_dir;
		char* entrypoint;
		int network_disabled;
		char* mac_address;
		char** on_build;
		int num_on_build;
		//TODO: Add labels type;
		char* labels;
		char* stop_signal;
		int stop_timeout;
		char* shell;
		//TODO: Add type for host_config
		char* host_config;
		//TODO: Add type for network_config
		char* network_config;
	} docker_create_container_params;

	d_err_t make_docker_create_container_params(
		docker_create_container_params** params);

	d_err_t docker_create_container(docker_context* ctx, docker_result** result,
		char** id, docker_create_container_params* params);

	typedef struct docker_container_config_t {
		bool attach_stderr;
		bool attach_stdin;
		bool attach_stdout;
		arraylist* cmd;
		char* domain_name;
		arraylist* env;
		char* hostname;
		char* image;
		arraylist* labels;
		char* mac_address;
		bool network_disabled;
		bool open_stdin;
		bool stdin_once;
		bool tty;
		char* user;
		arraylist* volumes;
		char* working_dir;
		char* stop_signal;
		long stop_timeout;
	} docker_container_config;

	typedef struct docker_container_state_t {
		char* error;
		int exit_code;
		char* finished_at;
		bool oom_killed;
		bool dead;
		bool paused;
		long pid;
		bool restarting;
		bool running;
		char* started_at;
		char* status;
	}docker_container_state;

	typedef struct docker_container_details_t {
		char* app_armor_profile;
		arraylist* args;
		docker_container_config* config;
		time_t created;
		char* driver;
		arraylist* exec_ids;
		docker_container_host_config* host_config;
		char* hostname_path;
		char* hosts_path;
		char* log_path;
		char* id;
		char* image;
		char* mount_label;
		char* name;
		json_object* network_settings;
		char* path;
		char* process_label;
		char* resolv_conv_path;
		long restart_count;
		docker_container_state* state;
		arraylist* mounts;
	} docker_container_details;

	//docker_container_details* docker_inspect_container(docker_context* ctx, char* id, int size);

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

	typedef struct docker_container_pids_stats_t {
		int current;
	} docker_container_pids_stats;

	d_err_t make_docker_container_pids_stats(
		docker_container_pids_stats** pids_stats, int current);
	void free_docker_container_pids_stats(docker_container_pids_stats* pids);

	typedef struct docker_container_net_stats_t {
		char* name;
		unsigned long rx_bytes;
		unsigned long rx_dropped;
		unsigned long rx_errors;
		unsigned long rx_packets;
		unsigned long tx_bytes;
		unsigned long tx_dropped;
		unsigned long tx_errors;
		unsigned long tx_packets;
	} docker_container_net_stats;

	d_err_t make_docker_container_net_stats(docker_container_net_stats** net_stats,
		char* name, unsigned long rx_bytes, unsigned long rx_dropped,
		unsigned long rx_errors, unsigned long rx_packets,
		unsigned long tx_bytes, unsigned long tx_dropped,
		unsigned long tx_errors, unsigned long tx_packets);
	void free_docker_container_net_stats(docker_container_net_stats* net_stats);

	typedef struct docker_container_mem_stats_t {
		unsigned long max_usage;
		unsigned long usage;
		unsigned long failcnt;
		unsigned long limit;
	} docker_container_mem_stats;

	d_err_t make_docker_container_mem_stats(docker_container_mem_stats** mem_stats,
		unsigned long max_usage, unsigned long usage, unsigned long failcnt,
		unsigned long limit);
	void free_docker_container_mem_stats(docker_container_mem_stats* mem_stats);

	//TODO: add throttling data
	typedef struct docker_container_cpu_stats_t {
		arraylist* percpu_usage; //of unsigned long
		unsigned long total_usage;
		unsigned long usage_in_usermode;
		unsigned long usage_in_kernelmode;
		unsigned long system_cpu_usage;
		int online_cpus;
	} docker_container_cpu_stats;

	d_err_t make_docker_container_cpu_stats(docker_container_cpu_stats** cpu_stats,
		unsigned long total_usage, unsigned long usage_in_usermode,
		unsigned long usage_in_kernelmode, unsigned long system_cpu_usage,
		int online_cpus);
	void free_docker_container_cpu_stats(docker_container_cpu_stats* cpu_stats);

	//TODO: define and add blkio stats
	typedef struct docker_container_stats_t {
		struct tm* read;
		docker_container_pids_stats* pid_stats;
		arraylist* net_stats_list; //of docker_container_net_stats*
		docker_container_mem_stats* mem_stats;
		docker_container_cpu_stats* cpu_stats;
		docker_container_cpu_stats* precpu_stats;
	} docker_container_stats;

	d_err_t make_docker_container_stats(docker_container_stats** stats,
		struct tm* read, docker_container_pids_stats* pid_stats,
		docker_container_mem_stats* mem_stats,
		docker_container_cpu_stats* cpu_stats,
		docker_container_cpu_stats* precpu_stats);

	void free_docker_container_stats(docker_container_stats* stats);

	float docker_container_stats_get_cpu_usage_percent(
		docker_container_stats* stats);

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
