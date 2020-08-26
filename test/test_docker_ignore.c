#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include <stdlib.h>
#include <curl/curl.h>

#include "test_docker_ignore.h"

#include <arraylist.h>
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
