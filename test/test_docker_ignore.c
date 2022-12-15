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
#include <stdlib.h>
#include <curl/curl.h>

#include "test_docker_ignore.h"

#include <coll_arraylist.h>
#include "docker_util.h"
#include "docker_log.h"
#include "docker_ignore.h"

static void test_dockerignore_lines(void **state)
{
    arraylist *lines;
    arraylist_new(&lines, NULL);
    d_err_t e = readlines_dockerignore("FROM alpine\nmaintainer <am@z.org>", lines);
    assert_int_equal(e, E_SUCCESS);
    assert_int_equal(2, arraylist_length(lines));
    assert_string_equal("FROM alpine\n", (char *)arraylist_get(lines, 0));
}

int docker_ignore_tests()
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_dockerignore_lines)};
    return cmocka_run_group_tests_name("docker ignore tests", tests, NULL, NULL);
}
