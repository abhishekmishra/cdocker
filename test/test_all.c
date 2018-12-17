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
	log_set_level(LOG_INFO);
	log_info("#### Starting clibdocker tests ####");
	int res = docker_container_tests();
	return res;
}

