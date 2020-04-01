/**
 * Copyright (c) 2020 Abhishek Mishra
 * 
 * This software is released under the MIT License.
 * https://opensource.org/licenses/MIT
 */

#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include <stdio.h>

#include "docker_log.h"
#include "test_docker_containers.h"
#include "test_docker_system.h"
#include "test_docker_images.h"
#include "test_docker_networks.h"
#include "test_docker_volumes.h"
#include <docker_result.h>
#include <docker_containers.h>

int main(int argc, char **argv) {
	int res = 0;
	docker_log_set_level(LOG_INFO);
	docker_log_info("#### Starting clibdocker tests ####");

	docker_log_info("#### Docker container API      ####");
	res = docker_container_tests();
	docker_log_info("#### Done                      ####");

	if (res > 0) {
		return res;
	}

	docker_log_info("#### Docker images API      	####");
	res = docker_images_tests();
	docker_log_info("#### Done                      ####");

	if (res > 0) {
		return res;
	}

	docker_log_info("#### Docker system API      	####");
	res = docker_system_tests();
	docker_log_info("#### Done                      ####");

	if (res > 0) {
		return res;
	}

	docker_log_info("#### Docker networks API      	####");
	res = docker_networks_tests();
	docker_log_info("#### Done                      ####");

	if (res > 0) {
		return res;
	}

	docker_log_info("#### Docker volumes API      	####");
	res = docker_volumes_tests();
	docker_log_info("#### Done                      ####");

	return res;
}

