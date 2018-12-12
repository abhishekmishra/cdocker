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

DockerContainersList* docker_containers_list();

#endif /* DOCKER_CONTAINERS_H_ */
