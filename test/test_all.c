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
#include "test_docker_ignore.h"
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

	docker_log_info("#### Docker ignore files test  ####");
	res = docker_ignore_tests();
	docker_log_info("#### Done                      ####");

	if (res > 0) {
		return res;
	}

	return res;
}

