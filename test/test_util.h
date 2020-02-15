/**
 * Copyright (c) 2020 Abhishek Mishra
 * 
 * This software is released under the MIT License.
 * https://opensource.org/licenses/MIT
 */

#ifndef TEST_TEST_UTIL_H_
#define TEST_TEST_UTIL_H_

void handle_error(docker_result* res);

void handle_result_for_test(docker_context* ctx, docker_result* res);

#endif /* TEST_TEST_UTIL_H_ */
