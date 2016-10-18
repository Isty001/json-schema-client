#ifndef API_TEST_FORM_REQUEST_H
#define API_TEST_FORM_REQUEST_H


#include "../schema.h"


void request_fields_init(Link *link, int width);

Iterator *request_field_iterator(void);

FIELD **request_fields_to_array(void);

void request_fields_destroy(void);


#endif
