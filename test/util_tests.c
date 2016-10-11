#include <stdio.h>
#include <malloc.h>
#include "../src/util.h"
#include "unit_test.h"


static char *ugly = "{\"name\":\"Someone\", \"age\":30}";
static char *expected =
        "{\n"
        "    \"name\": \"Someone\",\n"
        "    \"age\": 30\n"
        "}";


MU_TEST(test_json)
{
    mu_assert(NULL == json_prettify("Invalid json"), "It should be NULL");

    char *result = json_prettify(ugly);
    assert_string(expected, result);

    free(result);
}

MU_TEST(test_empty_string)
{
    mu_assert(is_string_empty("   "), "String should be empty");
    mu_assert(!is_string_empty(" A  "), "String shouldn't be empty");
}


void run_util_tests(void)
{
    puts("UTIL TESTS");

    MU_RUN_TEST(test_json);
    MU_RUN_TEST(test_empty_string);

    MU_REPORT();
}

