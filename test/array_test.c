#include <malloc.h>
#include <string.h>
#include "../src/array.h"
#include "unit_test.h"


MU_TEST(test_array)
{
    Array *array = array_init();

    mu_assert(NULL == array_pop(array), "Array is not empty");
    mu_assert(NULL == array_last(array), "Array is not empty");
    mu_assert(array_is_empty(array), "Array is not empty");

    for (int i = 0; i < 10; i++) {
        array_push(array, (void *) i);
    }

    mu_assert_int_eq(10, array_count(array));
    mu_assert(9 == (int) array_pop(array), "Last item is not 9");
    mu_assert(false == array_is_empty(array), "Array is empty");
    mu_assert(8 == (int) array_last(array), "Last item is not 8");

    array_destroy(array);
}

MU_TEST(test_array_filter)
{
    Array *array = array_init();
    array_push(array, "first");
    array_push(array, "second");
    array_push(array, "third");

    array_filter(array, (ArrayFilter) function(bool, (char *item){
            return 0 == strcmp("second", item);
    }));

    mu_assert_int_eq(1, array_count(array));
    assert_string("second", array_pop(array));
    mu_assert(NULL == array_pop(array), "Array should be empty");

    array_destroy(array);
}

void run_stack_test(void)
{
    puts("ARRAY TEST");

    MU_RUN_TEST(test_array);
    MU_RUN_TEST(test_array_filter);

    MU_REPORT();
}

