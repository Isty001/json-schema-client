#ifndef API_TEST_HREF_H
#define API_TEST_HREF_H


#include "iterator.h"


Iterator *href_arguments(char *href);

void href_replace(char **url, char *placeholder, char *value);


#endif
