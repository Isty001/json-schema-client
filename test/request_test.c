#include <form.h>
#include <malloc.h>
#include "unit_test.h"
#include "../src/ui/field.h"
#include "../src/iterator.h"
#include "../src/request.h"
#include "../src/util.h"
#include "../src/schema.h"
#include "../src/ui/request_fields.h"
#include "../src/storage.h"
#include "../src/ui/request_field_storage.h"


static FIELD *create_field(FieldType type)
{
    FIELD *field = new_field(1, 10, 20, 10, 0, 0);
    set_field_userptr(field, field_create_attributes(type, NULL));

    return field;
}

static FIELD **create_fields(void)
{
    FIELD **fields = malloc(5 * sizeof(FIELD *));

    fields[0] = create_field(FIELD_HEADER);
    fields[1] = create_field(FIELD_USER);
    fields[2] = create_field(FIELD_PASSWORD);
    fields[3] = create_field(FIELD_DATA);
    fields[4] = create_field(FIELD_QUERY);

    return fields;
}

static char *test_field_read(FIELD unused(*field))
{
    return strdup("test_buffer");
}

MU_TEST(test_request_create)
{
    Link *link = malloc(sizeof(Link));
    link->url = "url";
    link->method = "PATCH";

    FIELD **fields = create_fields();
    Iterator *iterator = iterator_init((void *) fields, 5);
    Request *request = request_create_from_fields(iterator, link, test_field_read);

    assert_string("test_buffer", request->headers->data);
    assert_string("test_buffer", request->password);
    assert_string("test_buffer", request->user);
    assert_string("test_buffer", request->data);
    assert_string("PATCH", request->method);
    assert_string("urltest_buffer", request->url);

    for (int i = 0; i < 5; i++) {
        field_destroy(fields[i]);
    }

    free_multi(3, fields, iterator, link);
    request_destroy(request);
}

void run_request_test(void)
{
    puts("REQUEST TEST");

    MU_RUN_TEST(test_request_create);

    MU_REPORT();
}
