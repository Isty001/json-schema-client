#include <string.h>
#include "field.h"


char *field_read(FIELD *field)
{
    char *buffer = field_buffer(field, 0);

    if (is_input(field) && !is_string_empty(buffer)) {
        return remove_trailing_spaces(buffer);
    }

    return NULL;
}

FieldAttributes *field_attributes_create(FieldType type, char *str)
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
        _free(attr->id);
        free(attr);
    }
    free_field(field);
}

static FIELD *create_base_field(int y, int x, int width, int height, int chars_min)
{
    int off_screen = 0;

    if (width < chars_min) {
        off_screen = chars_min - width;
    }

    return new_field(height, width, y, x, off_screen, 0);
}

void field_set_add_label(FieldSet *set, char *text, int y, int x)
{
    FIELD *label = create_base_field(y, x, set->width, 1, set->chars_min);

    field_opts_off(label, O_ACTIVE);
    set_field_buffer(label, 0, text);

    array_push(set->stack, label);
}

static FIELD *create_default_field(int y, int x, int height, FieldSet *set)
{
    FIELD *field = create_base_field(y, x, set->width, height, set->chars_min);

    field_opts_off(field, O_AUTOSKIP);
    field_opts_on(field, O_ACTIVE);
    set_field_back(field, A_UNDERLINE);

    return field;
}

void field_set_add_field_without_attr(FieldSet *set, int y, int x, int height)
{
    array_push(set->stack, create_default_field(y, x, height, set));
}

void field_set_add_field(FieldSet *set, int y, int x, int height, FieldType type, char *id)
{
    FIELD *field = create_default_field(y, x, height, set);
    set_field_userptr(field, field_attributes_create(type, id));

    array_push(set->stack, field);
}

FieldSet *field_set_init(int width, int chars_min)
{
    FieldSet *set = malloc(sizeof(FieldSet));
    set->stack = array_init();
    set->width = width;
    set->chars_min = chars_min;

    return set;
}

void field_set_finalize(FieldSet *set)
{
    array_push(set->stack, NULL);
    set->array = (FIELD **) array_values(set->stack);
    array_pop(set->stack);
}

void field_set_destroy(FieldSet *set)
{
    array_destroy_callback(set->stack, (Free) field_destroy);
    free_multi(2, set->array, set);
}
