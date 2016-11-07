#include <stdio.h>
#include <string.h>
#include "field.h"
#include "request_field_storage.h"
#include "../storage.h"


#define BASE_STORAGE_KEY "request.form.%s.field.%d"

#define key_size(link) strlen(link->url) + strlen(link->method) + strlen(BASE_STORAGE_KEY) + 1


static void create_storage_key(char *key, Link *link, FieldAttributes *attr)
{
    sprintf(key, "request.form.%s.%s.field.%s", link->method, link->url, attr->id);
}

void request_fields_save(Iterator *fields, Link *link)
{
    char *buffer;
    char key[key_size(link)];

    iterator_walk(fields, (WalkCallback )function(void, (FIELD *field) {
        buffer = field_read(field);

        if (buffer) {
            create_storage_key(key, link, field_userptr(field));
            storage_set(key, buffer);
        }
    }));

    storage_dump();
}

void request_fields_load(Iterator *fields, Link *link)
{
    char *buffer;
    char key[key_size(link)];

    iterator_walk(fields, (WalkCallback )function(void, (FIELD *field) {
        if (is_input(field)) {
            create_storage_key(key, link, field_userptr(field));

            if ((buffer = storage_get(key))) {
                set_field_buffer(field, 0, buffer);
            }
        }
    }));
}
