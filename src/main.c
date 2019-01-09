#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <curl/curl.h>
#include <json-c/json.h>
#include "docker_result.h"
#include "docker_connection_util.h"
#include "docker_containers.h"
#include "docker_system.h"
#include "docker_images.h"
#include "docker_networks.h"
#include "docker_volumes.h"
#include "log.h"

void handle_error(docker_result* res) {
	docker_simple_error_handler_log(res);
}

void log_pull_message(docker_image_create_status* status, void* client_cbargs) {
	if (status) {
		if (status->id) {
			docker_log_debug("message is %s, id is %s", status->status,
					status->id);
		} else {
			docker_log_debug("message is %s", status->status);
		}
	}
}

void log_events(docker_event* evt, void* client_cbargs) {
	if (evt) {
		docker_log_debug("%d: %s: %s: %s", evt->time, evt->type, evt->action,
				evt->actor_id);
	}
}

int extract_args_url_connection(int argc, char** url_ret, char* argv[],
		docker_context** ctx, docker_result** res) {
	char* url;
	int connected = 0;
	if (argc > 1) {
		url = argv[1];
		if (is_http_url(url)) {
			if (make_docker_context_url(ctx, url) == E_SUCCESS) {
				connected = 1;
			}
		} else if (is_unix_socket(url)) {
			if (make_docker_context_socket(ctx, url) == E_SUCCESS) {
				connected = 1;
			}
		}
	} else {
		url = DOCKER_DEFINE_DEFAULT_UNIX_SOCKET;
		if (make_docker_context_socket(ctx, url) == E_SUCCESS) {
			connected = 1;
		}
	}
	if (connected) {
		if (docker_ping((*ctx), res) != E_SUCCESS) {
			docker_log_fatal("Could not ping the server %s", url);
			connected = 0;
		} else {
			docker_log_info("%s is alive.", url);
		}
	}
	(*url_ret) = url;
	return connected;
}

int main(int argc, char* argv[]) {
	curl_global_init(CURL_GLOBAL_ALL);
	char* id;
	char* url;
	int connected = 0;
	docker_context* ctx;
	docker_result* res;

	connected = extract_args_url_connection(argc, &url, argv, &ctx, &res);
	if (!connected) {
		return E_PING_FAILED;
	}

	docker_ping(ctx, &res);
	handle_error(res);

//	array_list* evts;
//	docker_system_events_cb(ctx, &res, &log_events, NULL, &evts,
//			time(NULL) - (3600 * 24), time(NULL));
//	//This is an example for listening endlessly
////		docker_system_events_cb(ctx, &res, &log_events, &evts, time(NULL) - (3600 * 24),
////				0);
//	handle_error(res);
//
//	docker_version* version;
//	docker_system_version(ctx, &res, &version);
//	handle_error(res);
//
//	//Images API
//	docker_image_create_from_image_cb(ctx, &res, &log_pull_message, NULL,
//			"alpine", "latest", NULL);
//	handle_error(res);

//Network API
	struct array_list* networks;
	docker_networks_list(ctx, &res, &networks, NULL, NULL, NULL, NULL, NULL,
	NULL);
	handle_error(res);
	int len_nets = array_list_length(networks);
	for (int i = 0; i < len_nets; i++) {
		docker_network* ni = (docker_network*) array_list_get_idx(
				networks, i);
		docker_log_info("Found network %s %s", ni->name, ni->id);
	}

	docker_network* net;
	docker_network_inspect(ctx, &res, &net, "host", 0, NULL);
	handle_error(res);

	//Volume API

	//create two test volumes
	docker_volume* vi = NULL;
	docker_volume_create(ctx, &res, &vi, "clibdocker_test_vol01", "local", 1,
			"clibdocker_test_label", "clibdocker_test_value");
	handle_error(res);
	if (vi != NULL) {
		docker_log_info("Created volume with name %s at %lu", vi->name,
				vi->created_at);
	}
	docker_volume_create(ctx, &res, &vi, "clibdocker_test_vol02", "local", 1,
			"clibdocker_test_label", "clibdocker_test_value");
	handle_error(res);
	docker_volume_create(ctx, &res, &vi, "clibdocker_test_vol03", "local", 1,
			"clibdocker_test_label", "no_delete");
	handle_error(res);

	//list volumes
	struct array_list* volumes;
	struct array_list* warnings;
	docker_volumes_list(ctx, &res, &volumes, &warnings, 1, NULL, NULL, NULL);
	handle_error(res);
	int len_vols = array_list_length(volumes);
	for (int i = 0; i < len_vols; i++) {
		docker_volume* vi = (docker_volume*) array_list_get_idx(
				volumes, i);
		docker_log_info("Found volume %s at %s", vi->name, vi->mountpoint);
	}

	//inspect volume
	vi = NULL;
	docker_volume_inspect(ctx, &res, &vi, "clibdocker_test_vol01");
	handle_error(res);
	if (vi != NULL) {
		docker_log_info("Inspect found volume with name %s at %lu", vi->name,
				vi->created_at);
	}

	//delete volumes
	docker_volume_delete(ctx, &res, "clibdocker_test_vol01", 0);
	handle_error(res);

	//delete unused volumes
	struct array_list* volumes_deleted;
	long space_reclaimed;
	docker_volumes_delete_unused(ctx, &res, &volumes_deleted, &space_reclaimed,
			1, 0, "clibdocker_test_label", "clibdocker_test_value");
	handle_error(res);
	for (int i = 0; i < array_list_length(volumes_deleted); i++) {
		docker_log_info("Deleted unused volume %s",
				(char* )array_list_get_idx(volumes_deleted, i));
	}

	//Containers API
//	docker_create_container_params* p;
//	make_docker_create_container_params(&p);
//	p->image = "alpine";
//	p->cmd = (char**) malloc(2 * sizeof(char*));
//	p->cmd[0] = "echo";
//	p->cmd[1] = "hello world";
//	p->num_cmd = 2;
//	docker_create_container(ctx, &res, &id, p);
//	handle_error(res);
//
//	printf("Docker container id is %s\n", id);
//
//	docker_start_container(ctx, &res, id, NULL);
//	handle_error(res);
//
//	docker_container_ps* ps;
//	docker_process_list_container(ctx, &res, &ps, id, NULL);
//	handle_error(res);
//
//	docker_wait_container(ctx, &res, id, NULL);
//	handle_error(res);
//
//	char* log;
//	docker_container_logs(ctx, &res, &log, id, 0, 1, 1, -1, -1, 1, -1);
//	docker_log_debug("Log -> |%s|", log);
//	handle_error(res);
//
//	docker_changes_list* changes;
//	docker_container_changes(ctx, &res, &changes, id);
//	handle_error(res);
//
//	docker_stop_container(ctx, &res, id, 0);
//	handle_error(res);
//
//	docker_restart_container(ctx, &res, id, 0);
//	handle_error(res);
//
//	docker_wait_container(ctx, &res, id, NULL);
//	handle_error(res);
//
//	docker_kill_container(ctx, &res, id, NULL);
//	handle_error(res);
//
//	docker_pause_container(ctx, &res, id);
//	handle_error(res);
//
//	docker_unpause_container(ctx, &res, id);
//	handle_error(res);
//
////		docker_rename_container(ctx, &res, id, "dude101");
////		handle_error(res);
//
//	docker_containers_list_filter* filter;
//	make_docker_containers_list_filter(&filter);
////		containers_filter_add_name(filter, "/registryui");
//	containers_filter_add_id(filter, id);
//	docker_containers_list* containers;
//	docker_container_list(ctx, &res, &containers, 1, 0, 1, filter);
//	handle_error(res);
//	docker_log_info("Read %d containers.\n",
//			docker_containers_list_length(containers));
//
//	for (int i = 0; i < docker_containers_list_length(containers); i++) {
//		docker_process_list_container(ctx, &res, &ps,
//				docker_containers_list_get_idx(containers, i)->id, NULL);
//		handle_error(res);
//		docker_changes_list* changes;
//		docker_container_changes(ctx, &res, &changes,
//				docker_containers_list_get_idx(containers, i)->id);
//		if (changes) {
//			docker_log_info("Read %d changes for Id %s",
//					docker_changes_list_length(changes), id);
//		}
//		handle_error(res);
//	}

	free_docker_context(&ctx);
	curl_global_cleanup();
	return 0;

}
