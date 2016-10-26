#include <stdlib.h>
#include "../src/storage.h"
#include "unit_test.h"


#define TEST_STORAGE_DIR "./test/fixture/storage"


static void storage_teardown(void)
{
    unlink(TEST_STORAGE_DIR"/storage.txt");
    rmdir(TEST_STORAGE_DIR);
}

MU_TEST(test_storage)
{
    storage_init(TEST_STORAGE_DIR);

    storage_set("test", "Hello");
    assert_string("Hello", storage_get("test"));

    storage_set("test", "Another");
    assert_string("Another", storage_get("test"));

    storage_set("test2", "More");
    storage_set("test3", "EvenMore");

    storage_delete("test2");

    mu_assert(NULL == storage_get("test2"), "Item is not removed");
    mu_assert(!storage_empty(), "Storage is empty");

    storage_delete("test");
    storage_delete("test3");
    mu_assert(storage_empty(), "Storage is not empty");

    storage_destroy();
}

MU_TEST(test_persistance)
{
    storage_init(TEST_STORAGE_DIR);

    storage_set("User", "user");
    storage_set("User2", "user_2");

    storage_dump();
    storage_destroy();

    storage_init(TEST_STORAGE_DIR);

    assert_string("user", storage_get("User"));

    storage_clear();
    mu_assert(NULL == storage_get("User2"), "Storage is not cleared");

    storage_destroy();
}


void run_storage_test(void)
{
    puts("STORAGE TEST");

    MU_SUITE_CONFIGURE(NULL, storage_teardown);
    MU_RUN_TEST(test_storage);
    MU_RUN_TEST(test_persistance);

    MU_REPORT();
}


