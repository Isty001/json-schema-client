#include <malloc.h>
#include <pcre.h>
#include "unit_test.h"
#include "../src/href.h"


static char **expected;


static void href_setup(void)
{
    expected = malloc(12);
    expected[0] = "{id}";
    expected[1] = "{location}";
}

static void href_teardown(void)
{
    free(expected);
}

static void test_href_callback(void *match_list)
{
    static int i = 0;
    char **matches = (char **) match_list;

    assert_string(expected[i++], matches[0]);
    pcre_free_substring_list((const char **) matches);
}

MU_TEST(test_href)
{
    char *href = "/user/{id}/{location}";

    Iterator *arguments = href_arguments(href);

    iterator_walk(arguments, test_href_callback);

    iterator_destroy(arguments);
}

void run_href_test(void)
{
    puts("HREF TEST");

    MU_SUITE_CONFIGURE(href_setup, href_teardown);

    MU_RUN_TEST(test_href);

    MU_REPORT();
}


