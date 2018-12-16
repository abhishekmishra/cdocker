/*
 * test_all.c
 *
 *  Created on: 16-Dec-2018
 *      Author: abhishek
 */

#include <stdio.h>
#include "minunit.h"
#include "test_docker_containers.h"


int main(int argc, char **argv) {
	char *result = docker_container_tests();
	if (result != 0) {
		printf("%s\n", result);
	} else {
		printf("ALL TESTS PASSED\n");
	}
	printf("Tests run: %d\n", tests_run);

	return result != 0;
}

