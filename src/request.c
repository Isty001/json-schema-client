#include <string.h>
#include <curl/curl.h>
#include "request.h"
#include "ui/field.h"
#include "href.h"
#include "ui/request_fields.h"
#include "ui/request_field_storage.h"


static void append_query(Request *request, char *query)
{
    char *url = request->url;

    asprintf(&url, "%s%s", url, remove_trailing_spaces(query));
    free(request->url);

    request->url = url;
}

static void apply_field_on_request(FIELD *field, Request *request, FieldReader read)
{
    char *buffer = read(field);

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

Request *request_create_from_fields(Iterator *fields, Link *link, FieldReader reader)
{
    Request *request = create_request(link);

    iterator_walk(fields, (WalkCallback) function(void, (FIELD * field) {
        apply_field_on_request(field, request, reader);
    }));

    return request;
}

Request *request_create_from_hidden_form(Link *link)
{
    request_fields_init(link, 20);

    Iterator *iterator = request_field_iterator();
    request_fields_load(iterator, link);
    FORM *form = new_form(request_fields_to_array());
    post_form(form);

    Request *request = request_create_from_fields(iterator, link, field_read);

    request_fields_destroy();
    unpost_form(form);
    free_form(form);

    return request;
}

void request_destroy(Request *request)
{
    curl_slist_free_all(request->headers);

    free_multi(6, request->method, request->url, request->data, request->user, request->password, request);
}
