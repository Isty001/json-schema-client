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


void request_save_form(FIELD **fields, Link *link);

void request_load_form(FIELD **fields, Link *link);

Request *request_create_from_form(Iterator *fields, Link *link);

void request_destroy(Request *request);


#endif
