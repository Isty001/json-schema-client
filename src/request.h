#ifndef API_TEST_REQUEST_H
#define API_TEST_REQUEST_H


#include <form.h>
#include <curl/curl.h>
#include "schema.h"


typedef struct
{
    char *user;
    char *password;
    struct curl_slist *headers;
    char *data;
    char *method;
    char *url;
} Request;

/** For testability */
typedef char *(*FieldReader)(FIELD *field);


Request *request_create_from_fields(Iterator *fields, Link *link, FieldReader reader);

Request *request_create_from_hidden_form(Link *link);

void request_destroy(Request *request);


#endif
