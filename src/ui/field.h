#ifndef API_TEST_FIELD_H
#define API_TEST_FIELD_H


#include <form.h>


#define is_input(field) flag_exists(field_opts(field), O_ACTIVE)


typedef enum
{
    FIELD_HEADER,
    FIELD_USER,
    FIELD_PASSWORD,
    FIELD_DATA,
    FIELD_HREF,
    FIELD_QUERY,
} FieldType;

typedef struct
{
    FieldType type;
    char *id;
} FieldAttributes;


char *field_read(FIELD *filed);

FieldAttributes *field_create_attributes(FieldType type, char *str);

void field_destroy(FIELD *field);


#endif
