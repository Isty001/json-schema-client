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
        if (attr->id) {
            free(attr->id);
        }
        free(attr);
    }
    free_field(field);
}

static FIELD *create_field(int y, int x, int width, int height, int chars_min)
{
    int off_screen = 0;

    if (width < chars_min) {
        off_screen = chars_min - width;
    }

    return new_field(height, width, y, x, off_screen, 0);
}

void field_set_add_label(FieldSet *set, char *text, int y, int x)
{
    FIELD *label = create_field(y, x, set->width, 1, set->chars_min);

    field_opts_off(label, O_ACTIVE);
    set_field_buffer(label, 0, text);

    stack_push(set->stack, label);
}

void field_set_add_field(FieldSet *set, int y, int x, int lines, FieldType type, char *id)
{
    FIELD *field = create_field(y, x, set->width, lines, set->chars_min);

    field_opts_off(field, O_AUTOSKIP);
    field_opts_on(field, O_ACTIVE);
    set_field_back(field, A_UNDERLINE);
    set_field_userptr(field, field_attributes_create(type, id));

    stack_push(set->stack, field);
}

FieldSet *field_set_init(int width, int chars_min)
{
    FieldSet *set = malloc(sizeof(FieldSet));
    set->stack = stack_init();
    set->width = width;
    set->chars_min = chars_min;

    return set;
}

void field_set_array(FieldSet *set)
{
    stack_push(set->stack, NULL);
    set->array = (FIELD **) stack_to_array(set->stack);
    stack_pop(set->stack);
}

void field_set_destroy(FieldSet *set)
{
    stack_destroy_callback(set->stack, (Free) field_destroy);
    free_multi(2, set->array, set);
}
