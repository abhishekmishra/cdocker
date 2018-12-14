/*
 * docker_containers.h
 *
 *  Created on: 10-Dec-2018
 *      Author: abhishek
 */

#ifndef DOCKER_CONTAINERS_H_
#define DOCKER_CONTAINERS_H_

typedef struct docker_container_ports_t {
	int private_port;
	int public_port;
	char* type;
} DockerContainerPorts;

typedef struct docker_container_label_t {
	char* key;
	char* value;
} DockerContainerLabel;

typedef struct docker_container_host_config_t {
	char* network_mode;
} DockerContainerHostConfig;

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
} DockerContainerNetworkSettingsNetworkItem;

typedef struct docker_container_network_settings_network_t {
	char* name;
	DockerContainerNetworkSettingsNetworkItem* item;
} DockerContainerNetworkSettingsNetwork;

typedef struct docker_container_network_settings_t {
	DockerContainerNetworkSettingsNetwork** network_items;
	int num_network_items;
} DockerContainerNetworkSettings;

typedef struct docker_container_mount_t {
	char* name;
	char* type;
	char* source;
	char* destination;
	char* driver;
	char* mode;
	int rw;
	char* propagation;
} DockerContainerMount;

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
	DockerContainerPorts** ports;
	int num_ports;
	DockerContainerLabel** labels;
	int num_labels;
	long long size_rw;
	long long size_root_fs;
	DockerContainerHostConfig* hostConfig;
	DockerContainerNetworkSettings* network_settings;
	DockerContainerMount** mounts;
	int num_mounts;
} DockerContainersListItem;

typedef struct docker_containers_list_t {
	DockerContainersListItem** containers;
	int num_containers;
} DockerContainersList;

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
} DockerContainersListFilter;

/**
 * Create filter object and create filter items.
 */
DockerContainersListFilter* make_docker_containers_list_filter();
void containers_filter_add_ancestor(DockerContainersListFilter* filter, char* val);
void containers_filter_add_before(DockerContainersListFilter* filter, char* val);
void containers_filter_add_expose(DockerContainersListFilter* filter, char* val);
void containers_filter_add_exited(DockerContainersListFilter* filter, int val);
void containers_filter_add_health(DockerContainersListFilter* filter, char* val);
void containers_filter_add_id(DockerContainersListFilter* filter, char* val);
void containers_filter_add_isolation(DockerContainersListFilter* filter, char* val);
void containers_filter_add_is_task(DockerContainersListFilter* filter, int val);
void containers_filter_add_label(DockerContainersListFilter* filter, char* val);
void containers_filter_add_name(DockerContainersListFilter* filter, char* val);
void containers_filter_add_network(DockerContainersListFilter* filter, char* val);
void containers_filter_add_publish(DockerContainersListFilter* filter, char* val);
void containers_filter_add_since(DockerContainersListFilter* filter, char* val);
void containers_filter_add_status(DockerContainersListFilter* filter, char* val);
void containers_filter_add_volume(DockerContainersListFilter* filter, char* val);


DockerContainersList* docker_containers_list(int all, int limit, int size,
		DockerContainersListFilter* filters);

#endif /* DOCKER_CONTAINERS_H_ */
