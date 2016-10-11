#ifndef API_TEST_FIELD_H
#define API_TEST_FIELD_H


#include <form.h>


#define is_input(field) flag_exists(field_opts(field), O_ACTIVE)


typedef enum
{
    FIELD_HEADER = 0,
    FIELD_USER = 1,
    FIELD_PASSWORD = 3,
    FIELD_DATA = 4
} FieldType;

typedef struct
{
    FieldType type;
} FieldAttributes;


char *field_read(FIELD *filed);

FieldAttributes *field_create_attributes(FieldType type);

void field_destroy(FIELD *field);


#endif
