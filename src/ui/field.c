#include <string.h>
#include "field.h"
#include "../util.h"


char *field_read(FIELD *field)
{
    char *buffer = field_buffer(field, 0);

    if (is_input(field) && !is_string_empty(buffer)) {
        return buffer;
    }
    return NULL;
}

FieldAttributes *field_create_attributes(FieldType type, char *str)
{
    FieldAttributes *attr = malloc(sizeof(FieldAttributes));
    attr->type = type;
    attr->id = str ? strdup(str) : NULL;

    return attr;
}

void field_destroy(FIELD *field)
{
    FieldAttributes *attr = field_userptr(field);

    if (attr) {
        if (attr->id) {
            free(attr->id);
        }
        free(attr);
    }
    free_field(field);
}

