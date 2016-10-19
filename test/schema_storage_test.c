#include <stdio.h>
#include <sys/stat.h>
#include "unit_test.h"
#include "../src/schema.h"
#include "../src/storage.h"
#include "../src/schema_storage.h"


static char *serialized = "[{\"name\":\"test\",\"url\":\".\\/test\\/fixture\\/schema.json\",\"api_url\":\"api_url\"},{\"name\":\"test2\",\"url\":\".\\/test\\/fixture\\/schema2.json\",\"api_url\":null}]";


static void schema_storage_setup(void)
{
    storage_init("./test/fixture");
    schema_init();
}

static void schema_storage_teardown(void)
{
    storage_destroy();
    schema_destroy();
    unlink("./test/fixture/storage.txt");
}

MU_TEST(schema_storage_serialize_test)
{
    schema_load("test", "./test/fixture/schema.json", "api_url");
    schema_load("test2", "./test/fixture/schema2.json", NULL);

    schema_storage_serialize();

    assert_string(serialized, storage_get(SCHEMA_LIST_KEY));

    storage_clear();
}

MU_TEST(schema_storage_unserialize_test)
{
    storage_set(SCHEMA_LIST_KEY, serialized);

    schema_storage_unserialize();

    Iterator *iterator = schema_iterator();
    Schema *schema = iterator_seek(iterator, 1);

    assert_string("test2", schema->name);
    mu_assert(NULL == schema->api_url, "Api url is not NULL");

    iterator_destroy(iterator);
}


void run_schema_storage_test(void)
{
    puts("SCHEMA STORAGE TEST");

    MU_SUITE_CONFIGURE(schema_storage_setup, schema_storage_teardown);

    MU_RUN_TEST(schema_storage_serialize_test);
    MU_RUN_TEST(schema_storage_unserialize_test);

    MU_REPORT();
}
