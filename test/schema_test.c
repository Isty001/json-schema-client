#include "unit_test.h"
#include "../src/schema.h"
#include "../src/storage.h"


Iterator *iterator;


static void schema_setup(void)
{
    schema_init();
}

static void schema_teardown(void)
{
    schema_destroy();
    iterator_destroy(iterator);
}

static void assert_first_link(Link *link)
{
    assert_string("list", link->rel);
    assert_string("/user/{id}", link->href);
}

static void assert_second_link(Link *link)
{
    assert_string("show", link->rel);
    assert_string("/user/{birthDate}", link->href);
    assert_string("GET", link->method);
    assert_string("Desc", link->description);
    assert_string("Title", link->title);
    assert_string("GET - /user/{birthDate}", link->display);
    assert_string("example.com/user/{birthDate}", link->url);
}

static void assert_link(void *link)
{
    static int i = 0;

    if (i++ == 0) {
        assert_first_link(link);
    } else {
        assert_second_link(link);
    }
}

MU_TEST(test_schema_curl)
{
    schema_load("user", "URL", "example.com");
    iterator = schema_iterator();
    Schema *schema = iterator_seek(iterator, 0);

    assert_string("user", schema->name);
    assert_string("URL", schema->url);
    assert_string("example.com", schema->api_url);
    mu_assert_int_eq(2, iterator_count(schema->links));
    iterator_walk(schema->links, assert_link);

    schema_remove("user");
    iterator_destroy(iterator);
    iterator = schema_iterator();
    mu_assert_int_eq(0, iterator_count(iterator));

    storage_clear();
}

static void assert_first_schema(void)
{
    Schema *schema = iterator_seek(iterator, 0);
    iterator_walk(schema->links, (WalkCallback) assert_first_link);
}

MU_TEST(test_schema_file)
{
    schema_load("user", "./test/fixture/schema.json", NULL);
    iterator = schema_iterator();

    mu_assert_int_eq(1, iterator_count(iterator));
    assert_first_schema();
}

MU_TEST(test_schema_stored)
{
    schema_load("user", "non_existent_url_to_test_storage", "URL");
    iterator = schema_iterator();

    mu_assert_int_eq(1, iterator_count(iterator));
    assert_first_schema();
}

void run_schema_test(void)
{
    storage_init("./test/fixture");

    puts("SCHEMA TEST");

    MU_SUITE_CONFIGURE(schema_setup, schema_teardown);
    MU_RUN_TEST(test_schema_curl);
    MU_RUN_TEST(test_schema_file);
    MU_RUN_TEST(test_schema_stored);

    MU_REPORT();

    storage_destroy();
}

