#include <curl/curl.h>
#include <malloc.h>
#include <string.h>
#include <stdlib.h>
#include "curl.h"
#include "request.h"
#include "util.h"


void curl_init(void)
{
    curl_global_init(CURL_GLOBAL_ALL);
}

void curl_destroy(void)
{
    curl_global_cleanup();
}

size_t write_callback(void *ptr, size_t unused(size), size_t unused(nmemb), char **s)
{
    *s = strdup(ptr);

    return strlen(*s);
}

CURL *create_curl_handle(char *url, char **buff)
{
    CURL *curl = curl_easy_init();

    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, false);
    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, buff);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10L);

    return curl;
}

char *curl_get(char *url)
{
    char **buff = malloc(1);

    CURL *curl = create_curl_handle(url, buff);
    curl_easy_perform(curl);
    curl_easy_cleanup(curl);

    return *buff;
}

static void set_authentication(Request *request, CURL *curl)
{
    if (request->user && request->password) {
        curl_easy_setopt(curl, CURLOPT_HTTPAUTH, (long)CURLAUTH_BASIC);
        curl_easy_setopt(curl, CURLOPT_USERNAME, request->user);
        curl_easy_setopt(curl, CURLOPT_PASSWORD, request->password);
    }
}

static void set_method(Request *request, CURL *curl)
{
    if (request->method) {
        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, request->method);
    }
}

static void set_data(Request *request, CURL *curl)
{
    if (request->data) {
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, request->data);
    }
}

static void set_headers(Request *request, CURL *curl)
{
    if (request->data) {
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, request->headers);
    }
}

char *curl_send_request(Request *request)
{
    char **buff = malloc(1);
    CURL *curl = create_curl_handle(request->url, buff);

    set_authentication(request, curl);
    set_method(request, curl);
    set_data(request, curl);
    set_headers(request, curl);

    curl_easy_perform(curl);
    curl_easy_cleanup(curl);

    request_destroy(request);

    return *buff;
}
