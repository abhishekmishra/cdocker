/*
 * docker_containers.h
 *
 *  Created on: 10-Dec-2018
 *      Author: abhishek
 */

#ifndef DOCKER_CONTAINERS_H_
#define DOCKER_CONTAINERS_H_

#include "docker_connection_util.h"

typedef struct docker_container_ports_t {
	int private_port;
	int public_port;
	char* type;
} docker_container_ports;

typedef struct docker_container_label_t {
	char* key;
	char* value;
} docker_container_label;

typedef struct docker_container_host_config_t {
	char* network_mode;
} docker_container_host_config;

typedef struct docker_container_network_settings_network_item_t {
	char* network_id;
	char* endpoint_id;
	char* gateway;
	char* ip_address;
	int ip_prefix_len;
	char* ipv6_gateway;
	char* global_ipv6_address;
	int global_ipv6_prefix_len;
	char* mac_address;
} docker_container_network_settings_network_item;

typedef struct docker_container_network_settings_network_t {
	char* name;
	docker_container_network_settings_network_item* item;
} docker_container_network_settings_network;

typedef struct docker_container_network_settings_t {
	docker_container_network_settings_network** network_items;
	int num_network_items;
} docker_container_network_settings;

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

typedef struct docker_containers_list_item_t {
	char* id;
	char** names;
	int num_names;
	char* image;
	char* image_id;
	char* command;
	long long created;
	char* state;
	char* status;
	docker_container_ports** ports;
	int num_ports;
	docker_container_label** labels;
	int num_labels;
	long long size_rw;
	long long size_root_fs;
	docker_container_host_config* hostConfig;
	docker_container_network_settings* network_settings;
	docker_container_mount** mounts;
	int num_mounts;
} docker_containers_list_item;

typedef struct docker_containers_list_t {
	docker_containers_list_item** containers;
	int num_containers;
} docker_containers_list;

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
docker_containers_list_filter* make_docker_containers_list_filter();

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

docker_containers_list* docker_container_list(docker_context* ctx, int all, int limit, int size,
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

docker_create_container_params* make_docker_create_container_params();

char* docker_create_container(docker_context* ctx, docker_create_container_params* params);

//docker_container_details* docker_inspect_container(docker_context* ctx, char* id, int size);

int docker_start_container(docker_context* ctx, char* id);

int docker_wait_container(docker_context* ctx, char* id);

char* docker_stdout_container(docker_context* ctx, char* id);

#endif /* DOCKER_CONTAINERS_H_ */
