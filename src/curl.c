#include <curl/curl.h>
#include <malloc.h>
#include <string.h>
#include <stdlib.h>
#include "curl.h"
#include "request.h"
#include "util.h"
#include "ui/message.h"


typedef struct {
    char *content;
    size_t size;
} Buffer;

void curl_init(void)
{
    curl_global_init(CURL_GLOBAL_ALL);
}

void curl_destroy(void)
{
    curl_global_cleanup();
}

size_t write_callback(void *content, size_t unused(size), size_t unused(nmemb), Buffer *buffer)
{
    size_t realsize = size * nmemb;

    buffer->content = realloc(buffer->content, buffer->size + realsize + 1);

    memcpy(&(buffer->content[buffer->size]), content, realsize);
    buffer->size += realsize;
    buffer->content[buffer->size] = '\0';

    return realsize;
}

CURL *create_curl_handle(char *url, Buffer *buff)
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
    Buffer buffer;
    buffer.content = malloc(1);
    buffer.size = 0;

    CURL *curl = create_curl_handle(url, &buffer);
    curl_easy_perform(curl);
    curl_easy_cleanup(curl);

    return buffer.content;
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
    if (request->headers) {
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, request->headers);
    }
}

static void perform(CURL *curl)
{
    CURLcode code = curl_easy_perform(curl);
    char *msg = (char *)curl_easy_strerror(code);
    int status;

    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &status);
    asprintf(&msg, "%s | Status: %d", msg, status);

    if (CURLE_OK == code) {
        message_success(msg);
    } else {
        message_error(msg);
    }
}

char *curl_send_request(Request *request)
{
    Buffer buffer;
    buffer.content = malloc(1);
    buffer.size = 0;

    CURL *curl = create_curl_handle(request->url, &buffer);

    set_authentication(request, curl);
    set_method(request, curl);
    set_data(request, curl);
    set_headers(request, curl);

    perform(curl);
    curl_easy_cleanup(curl);

    request_destroy(request);

    return buffer.content;
}
