#include <string.h>
#include <curl/curl.h>
#include "request.h"
#include "storage.h"
#include "util.h"
#include "ui/field.h"
#include "ui/popup.h"


#define BASE_FIELD_storage_KEY "request.form.%s.field.%d"

#define key_size(link) strlen(link->href) + strlen(BASE_FIELD_storage_KEY) + 1


static void create_storage_key(char *key, Link *link, int field_offset)
{
    sprintf(key, "request.form.%s.field.%d", link->href, field_offset);
}

void request_save_form(FIELD **fields, Link *link)
{
    char *buffer;
    char key[key_size(link)];

    for (int i = 0; i < REQ_FIELD_COUNT; i++) {
        buffer = field_read(fields[i]);

        if (buffer) {
            create_storage_key(key, link, i);
            storage_set(key, buffer);
        }
    }
}

void request_load_form(FIELD **fields, Link *link)
{
    char *buffer;
    char key[key_size(link)];

    for (int i = 0; i < REQ_FIELD_COUNT; i++) {
        create_storage_key(key, link, i);

        if (is_input(fields[i]) && (buffer = storage_get(key))) {
            set_field_buffer(fields[i], 0, buffer);
        }
    }
}

static void apply_field_on_request(FIELD *field, Request *request)
{
    char *buffer = field_read(field);
    FieldAttributes *attr = field_userptr(field);

    if (!buffer) {
        return;
    }
    switch (attr->type) {
        case FIELD_HEADER:
            request->headers = curl_slist_append(request->headers, buffer);
            break;
        case FIELD_DATA:
            request->data = strdup(buffer);
            break;
        case FIELD_USER:
            request->user = strdup(buffer);
            break;
        case FIELD_PASSWORD:
            request->password = strdup(buffer);
            break;
    }
}

static Request *create_request(Link *link)
{
    Request *request = malloc(sizeof(Request));
    request->url = strdup(link->url);
    request->method = link->method ? strdup(link->method) : NULL;
    request->headers = NULL;
    request->password = NULL;
    request->user = NULL;
    request->data = NULL;

    return request;
}

Request *request_create_from_form(Iterator *fields, Link *link)
{
    Request *request = create_request(link);

    iterator_walk(fields, (IteratorWalkCallback) function(void, (FIELD * field)
            {
                apply_field_on_request(field, request);
            }));

#ifndef TEST
    iterator_destroy(fields);
#endif

    return request;
}

void request_destroy(Request *request)
{
    curl_slist_free_all(request->headers);

    free_multi(6, request->method, request->url, request->data, request->user, request->password, request);
}
