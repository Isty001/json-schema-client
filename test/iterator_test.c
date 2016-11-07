#include <malloc.h>
#include <string.h>
#include "../src/iterator.h"
#include "../src/util.h"
#include "unit_test.h"


static void walk_test(void *item)
{
    static int offset = 0;
    char *msg = "Item value does not match with the expected";

    if (0 == offset++) {
        mu_assert(0 == strcmp("12345", item), msg);
    } else {
        mu_assert(0 == strcmp("67890", item), msg);
    }
}

MU_TEST(test_iterator)
{
    char **items = malloc(16);
    items[0] = "12345";
    items[1] = "67890";

    Iterator *iterator = iterator_init((void *) items, 2);
    iterator_walk(iterator, walk_test);

    int expected = 0;

    iterator_walk_index(iterator, function(void, (void unused(*item), int i){
        mu_assert_int_eq(expected++, i);
    }));

    iterator_destroy(iterator);
}

static int map_int(int i)
{
    return i * 10;
}

static void assert_mapped(int new)
{
    mu_assert(0 == (new % 10), "Invalid item in mapped Iterator");
}

MU_TEST(test_iterator_map)
{
    int items[2] = {1,3};

    Iterator *iterator = iterator_init((void **)items, 2);
    Iterator *new = iterator_map(iterator, (MapCallback) map_int);

    iterator_walk(new, (WalkCallback) assert_mapped);

    free(iterator);
    iterator_destroy(new);
}

void run_iterator_test(void)
{
    puts("ITERATOR TEST");

    MU_RUN_TEST(test_iterator);
    MU_RUN_TEST(test_iterator_map);

    MU_REPORT();
}


