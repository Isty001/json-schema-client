#ifndef API_TEST_CURL_H
#define API_TEST_CURL_H


#include "request.h"


void curl_init(void);

char *curl_get(char *url);

char *curl_send_request(Request *request);

void curl_destroy(void);


#endif
