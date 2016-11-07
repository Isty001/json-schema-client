#include <malloc.h>
#include "unit_test.h"
#include "../src/request_data.h"


MU_TEST(test_request_data)
{
    char *actual;
    char *expected = "{\"asd\": 12}\n";
    char *path = strdup("@./test/fixture/data.json");

    actual = request_data_load(path);
    assert_string(expected, actual);
    free(actual);

    assert_string("not a file", request_data_load("not a file"));
}

void run_request_data_test(void)
{
    puts("REQUEST DATA TEST");

    MU_RUN_TEST(test_request_data);

    MU_REPORT();
}
