/*
 * test_all.c
 *
 *  Created on: 16-Dec-2018
 *      Author: abhishek
 */

#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include <stdio.h>
#include "test_docker_containers.h"
#include "log.h"

int main(int argc, char **argv) {
	docker_log_set_level(LOG_INFO);
	docker_log_info("#### Starting clibdocker tests ####");
	docker_log_info("#### Docker container API      ####");
	int res = docker_container_tests();
	docker_log_info("#### Done                      ####");
	return res;
}

