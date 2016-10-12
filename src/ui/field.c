#include "field.h"
#include "../util.h"


char *field_read(FIELD *field)
{
#ifdef TEST
    return "test_buffer";
#endif
    char *buffer = field_buffer(field, 0);

    if (is_input(field) && !is_string_empty(buffer)) {
        return buffer;
    }
    return NULL;
}

FieldAttributes *field_create_attributes(FieldType type)
{
    FieldAttributes *attr = malloc(sizeof(FieldAttributes));
    attr->type = type;

    return attr;
}

void field_destroy(FIELD *field)
{
    FieldAttributes *attr = field_userptr(field);

    if (attr) {
        free(attr);
    }
    free_field(field);
}

