#ifndef API_TEST_UNIT_TEST_H
#define API_TEST_UNIT_TEST_H


#include <string.h>
#include "minunit.h"


static inline void assert_string(char *expected, char *actual)
{
    char buff[1000];
    sprintf(buff, "Strings are not equal. Expected %s got %s", expected, actual);

    mu_assert(0 == strcmp(expected, actual), buff);
}


#endif
