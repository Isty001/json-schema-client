#include <string.h>
#include <curl/curl.h>
#include "request.h"
#include "storage.h"
#include "util.h"
#include "ui/field.h"
#include "ui/popup.h"
#include "href.h"


#define dup_buffer(buff) strdup(remove_trailing_spaces(buff))

#define BASE_REQ_FIELD_KEY "request.form.%s.field.%d"

#define key_size(link) strlen(link->url) + strlen(BASE_REQ_FIELD_KEY) + 1


static void create_storage_key(char *key, Link *link, FieldAttributes *attr)
{
    sprintf(key, "request.form.%s.field.%s", link->url, attr->id);
}

void request_save_form(Iterator *fields, Link *link)
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

void request_load_form(Iterator *fields, Link *link)
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

static void append_query(Request *request, char *query)
{
    char *url = request->url;

    asprintf(&url, "%s%s", url, remove_trailing_spaces(query));
    free(request->url);

    request->url = url;
}

static void apply_field_on_request(FIELD *field, Request *request)
{
    char *buffer = field_read(field);

    if (!buffer) {
        return;
    }

    FieldAttributes *attr = field_userptr(field);

    switch (attr->type) {
        case FIELD_HEADER:
            request->headers = curl_slist_append(request->headers, remove_trailing_spaces(buffer));
            break;
        case FIELD_DATA:
            request->data = dup_buffer(buffer);
            break;
        case FIELD_USER:
            request->user = dup_buffer(buffer);
            break;
        case FIELD_PASSWORD:
            request->password = dup_buffer(buffer);
            break;
        case FIELD_HREF:
            href_replace(&request->url, attr->id, remove_trailing_spaces(buffer));
            break;
        case FIELD_QUERY:
            append_query(request, buffer);
            break;
    }
#ifdef TEST
    free(buffer);
#endif
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

    iterator_walk(fields, (WalkCallback) function(void, (FIELD * field) {
        apply_field_on_request(field, request);
    }));

    return request;
}

void request_destroy(Request *request)
{
    curl_slist_free_all(request->headers);

    free_multi(6, request->method, request->url, request->data, request->user, request->password, request);
}
