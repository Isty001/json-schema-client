#ifndef API_TEST_HREF_H
#define API_TEST_HREF_H


#include "iterator.h"


Iterator *href_arguments(char *href);

char *href_fill(char *href, Iterator *arguments);

#endif
