#include "schema_fields.h"
#include "ui.h"


FieldSet *set;


FieldSet *schema_fields_init(int width)
{
    set = field_set_init(width, width - PADDING * 2);

    field_set_add_label(set, "Name:", PADDING, PADDING);
    field_set_add_field_without_attr(set, 3, PADDING, 1);

    field_set_add_label(set, "URL:", 5, PADDING);
    field_set_add_field_without_attr(set, 6, PADDING, 1);

    field_set_add_label(set, "API URL:", 8, PADDING);
    field_set_add_field_without_attr(set, 9, PADDING, 1);

    field_set_finalize(set);

    return set;
}

void schema_fields_destroy(void)
{
    field_set_destroy(set);
}
