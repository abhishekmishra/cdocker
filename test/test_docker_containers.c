/*
 * test_docker_containers.c
 *
 *  Created on: 16-Dec-2018
 *      Author: abhishek
 */
/* file minunit_example.c */

#include <stdio.h>
#include "minunit.h"
#include "test_docker_containers.h"
#include "docker_containers.h"

int tests_run = 0;

int foo = 7;
int bar = 5;

static char * test_foo() {
	mu_assert("error, foo != 7", foo == 7);
	return 0;
}

static char * test_bar() {
	mu_assert("error, bar != 5", bar == 5);
	return 0;
}

char * docker_container_tests() {
	mu_run_test(test_foo);
	mu_run_test(test_bar);
	return 0;
}
