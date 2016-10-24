#ifndef API_TEST_FIELD_H
#define API_TEST_FIELD_H


#include <form.h>
#include "../stack.h"


#define is_input(field) flag_exists(field_opts(field), O_ACTIVE)


typedef enum
{
    FIELD_HEADER,
    FIELD_USER,
    FIELD_PASSWORD,
    FIELD_DATA,
    FIELD_HREF,
    FIELD_QUERY
} FieldType;

typedef struct
{
    FieldType type;
    char *id;
} FieldAttributes;


typedef struct
{
    int width;
    int chars_min;
    Stack *stack;
    FIELD **array;
} FieldSet;


FieldAttributes *field_attributes_create(FieldType type, char *str);


char *field_read(FIELD *filed);

void field_destroy(FIELD *field);


FieldSet *field_set_init(int width, int chars_min);

void field_set_add_label(FieldSet *set, char *text, int y, int x);

void field_set_add_field_without_attr(FieldSet *set, int y, int x, int height);

void field_set_add_field(FieldSet *set, int y, int x, int height, FieldType type, char *id);

void field_set_finalize(FieldSet *set);

void field_set_destroy(FieldSet *set);


#endif
