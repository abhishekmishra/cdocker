/*
 * docker_containers.h
 *
 *  Created on: 10-Dec-2018
 *      Author: abhishek
 */

#ifndef DOCKER_CONTAINERS_H_
#define DOCKER_CONTAINERS_H_

#include "docker_result.h"
#include "docker_connection_util.h"

#define DOCKER_CONTAINER_GETTER(object, type, name) \
		type docker_container_ ## object ## _get_ ## name(docker_container_ ## object* object);

#define DOCKER_CONTAINER_GETTER_ARR_ADD(object, type, name) \
		int docker_container_ ## object ## _get_ ## name ## _add(docker_container_ ## object* object, type data);

#define DOCKER_CONTAINER_GETTER_ARR_LEN(object, name) \
		int docker_container_ ## object ## _get_ ## name ##_length(docker_container_ ## object* object);

#define DOCKER_CONTAINER_GETTER_ARR_GET_IDX(object, type, name) \
		type docker_container_ ## object ## _get_ ## name ## _get_idx(docker_container_ ## object* object, int i);


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
error_t make_docker_container_ports(docker_container_ports** ports, long priv,
		long pub, char* type);

void free_docker_container_ports(docker_container_ports* ports);

DOCKER_CONTAINER_GETTER(ports, long, public_port)
DOCKER_CONTAINER_GETTER(ports, long, private_port)
DOCKER_CONTAINER_GETTER(ports, docker_container_port_type, type)

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
error_t make_docker_container_label(docker_container_label** label,
		const char* key, const char* value);

void free_docker_container_label(docker_container_label* label);

DOCKER_CONTAINER_GETTER(label, char*, key)
DOCKER_CONTAINER_GETTER(label, char*, value)

/**
 * Struct for the container host config.
 */
typedef struct docker_container_host_config_t {
	char* network_mode;
} docker_container_host_config;

/**
 * Create a new host config
 *
 * \param pointer to return instance
 * \param network_mode
 * \return error_code
 */
error_t make_docker_container_host_config(
		docker_container_host_config** host_config, const char* network_mode);

void free_docker_container_host_config(docker_container_host_config* host_config);

DOCKER_CONTAINER_GETTER(host_config, char*, network_mode)

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
error_t make_docker_container_network_settings_item(
		docker_container_network_settings_item** item, const char* name,
		const char* network_id, const char* endpoint_id, const char* gateway,
		const char* ip_address, int ip_prefix_len, const char* ipv6_gateway,
		const char* global_ipv6_address, int global_ipv6_prefix_len,
		const char* mac_address);

void free_docker_container_network_settings_item(docker_container_network_settings_item* settings_item);

DOCKER_CONTAINER_GETTER(network_settings_item, char*, name)
DOCKER_CONTAINER_GETTER(network_settings_item, char*, network_id)
DOCKER_CONTAINER_GETTER(network_settings_item, char*, endpoint_id)
DOCKER_CONTAINER_GETTER(network_settings_item, char*, gateway)
DOCKER_CONTAINER_GETTER(network_settings_item, char*, ip_address)
DOCKER_CONTAINER_GETTER(network_settings_item, int, ip_prefix_len)
DOCKER_CONTAINER_GETTER(network_settings_item, char*, ipv6_gateway)
DOCKER_CONTAINER_GETTER(network_settings_item, char*, global_ipv6_address)
DOCKER_CONTAINER_GETTER(network_settings_item, int, global_ipv6_prefix_len)
DOCKER_CONTAINER_GETTER(network_settings_item, char*, mac_address)

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
error_t make_docker_container_mount(docker_container_mount** mount,
		const char* name, const char* type, const char* source,
		const char* destination, const char* driver, const char* mode,
		const int rw, const char* propagation);

void free_docker_container_mount(docker_container_mount* mount);

DOCKER_CONTAINER_GETTER(mount, char*, name)
DOCKER_CONTAINER_GETTER(mount, char*, type)
DOCKER_CONTAINER_GETTER(mount, char*, source)
DOCKER_CONTAINER_GETTER(mount, char*, destination)
DOCKER_CONTAINER_GETTER(mount, char*, driver)
DOCKER_CONTAINER_GETTER(mount, char*, mode)
DOCKER_CONTAINER_GETTER(mount, int, rw)
DOCKER_CONTAINER_GETTER(mount, char*, propagation)

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
	struct array_list* names;
	struct array_list* ports;
	struct array_list* labels;
	struct array_list* network_settings;
	struct array_list* mounts;
} docker_container_list_item;

/**
 * Create a new containers list item.
 */
error_t make_docker_containers_list_item(docker_container_list_item** item,
		const char* id,
		const char* image, const char* image_id, const char* command,
		const long long created, const char* state, const char* status,
		const long long size_rw, const long long size_root_fs,
		const docker_container_host_config* hostConfig);

void free_docker_container_list_item(docker_container_list_item* item);

DOCKER_CONTAINER_GETTER(list_item, char*, id)
DOCKER_CONTAINER_GETTER(list_item, char*, image)
DOCKER_CONTAINER_GETTER(list_item, char*, image_id)
DOCKER_CONTAINER_GETTER(list_item, char*, command)
DOCKER_CONTAINER_GETTER(list_item, long long, created)
DOCKER_CONTAINER_GETTER(list_item, char*, state)
DOCKER_CONTAINER_GETTER(list_item, char*, status)
DOCKER_CONTAINER_GETTER(list_item, long long, size_rw)
DOCKER_CONTAINER_GETTER(list_item, long long, size_root_fs)

DOCKER_CONTAINER_GETTER_ARR_ADD(list_item, char*, names)
DOCKER_CONTAINER_GETTER_ARR_LEN(list_item, names)
DOCKER_CONTAINER_GETTER_ARR_GET_IDX(list_item, char*, names)

DOCKER_CONTAINER_GETTER_ARR_ADD(list_item, docker_container_ports*, ports)
DOCKER_CONTAINER_GETTER_ARR_LEN(list_item, ports)
DOCKER_CONTAINER_GETTER_ARR_GET_IDX(list_item, docker_container_ports*, ports)

DOCKER_CONTAINER_GETTER_ARR_ADD(list_item, docker_container_label*, labels)
DOCKER_CONTAINER_GETTER_ARR_LEN(list_item, labels)
DOCKER_CONTAINER_GETTER_ARR_GET_IDX(list_item, docker_container_label*, labels)

DOCKER_CONTAINER_GETTER_ARR_ADD(list_item, docker_container_network_settings_item*, network_settings)
DOCKER_CONTAINER_GETTER_ARR_LEN(list_item, network_settings)
DOCKER_CONTAINER_GETTER_ARR_GET_IDX(list_item, docker_container_network_settings_item*, network_settings)

DOCKER_CONTAINER_GETTER_ARR_ADD(list_item, docker_container_mount*, mounts)
DOCKER_CONTAINER_GETTER_ARR_LEN(list_item, mounts)
DOCKER_CONTAINER_GETTER_ARR_GET_IDX(list_item, docker_container_mount*, mounts)

typedef struct docker_containers_list_filter_t {
	char** ancestor;
	int num_ancestor;
	char** before;
	int num_before;
	char** expose;
	int num_expose;
	int* exited;
	int num_exited;
	char** health;
	int num_health;
	char** id;
	int num_id;
	char** isolation;
	int num_isolation;
	int* is_task;
	int num_is_task;
	char** label;
	int num_label;
	char** name;
	int num_name;
	char** network;
	int num_network;
	char** publish;
	int num_publish;
	char** since;
	int num_since;
	char** status;
	int num_status;
	char** volume;
	int num_volume;
} docker_containers_list_filter;

/**
 * Create filter object and create filter items.
 */
error_t make_docker_containers_list_filter(
		docker_containers_list_filter** filter);

void containers_filter_add_ancestor(docker_containers_list_filter* filter,
		char* val);
void containers_filter_add_before(docker_containers_list_filter* filter,
		char* val);
void containers_filter_add_expose(docker_containers_list_filter* filter,
		char* val);
void containers_filter_add_exited(docker_containers_list_filter* filter,
		int val);
void containers_filter_add_health(docker_containers_list_filter* filter,
		char* val);
void containers_filter_add_id(docker_containers_list_filter* filter, char* val);
void containers_filter_add_isolation(docker_containers_list_filter* filter,
		char* val);
void containers_filter_add_is_task(docker_containers_list_filter* filter,
		int val);
void containers_filter_add_label(docker_containers_list_filter* filter,
		char* val);
void containers_filter_add_name(docker_containers_list_filter* filter,
		char* val);
void containers_filter_add_network(docker_containers_list_filter* filter,
		char* val);
void containers_filter_add_publish(docker_containers_list_filter* filter,
		char* val);
void containers_filter_add_since(docker_containers_list_filter* filter,
		char* val);
void containers_filter_add_status(docker_containers_list_filter* filter,
		char* val);
void containers_filter_add_volume(docker_containers_list_filter* filter,
		char* val);

error_t docker_container_list(docker_context* ctx, docker_result** result,
		struct array_list** container_list, int all, int limit, int size,
		docker_containers_list_filter* filters);

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

error_t make_docker_create_container_params(
		docker_create_container_params** params);

error_t docker_create_container(docker_context* ctx, docker_result** result,
		char** id, docker_create_container_params* params);

//docker_container_details* docker_inspect_container(docker_context* ctx, char* id, int size);

/**
 * Values for a single 'top'/'ps' line. Details about a process.
 */
typedef struct docker_container_ps_item_t {
	char** vals;
	int num_vals;
} docker_container_ps_item;

/**
 * Struct which holds the titles of the process line, and the details of all processes.
 */
typedef struct docker_container_ps_t {
	char** titles;
	int num_titles;
	docker_container_ps_item* processes;
	int num_processes;
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
error_t docker_process_list_container(docker_context* ctx,
		docker_result** result, docker_container_ps** ps, char* id,
		char* process_args);

error_t docker_start_container(docker_context* ctx, docker_result** result,
		char* id);

error_t docker_wait_container(docker_context* ctx, docker_result** result,
		char* id);

error_t docker_stdout_container(docker_context* ctx, docker_result** result,
		char** log, char* id);

#endif /* DOCKER_CONTAINERS_H_ */
