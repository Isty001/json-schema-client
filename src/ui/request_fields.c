#include <form.h>
#include <form.h>
#include <pcre.h>
#include <string.h>
#include "request_fields.h"
#include "ui.h"
#include "field.h"
#include "../href.h"


#define FIELD_CHARS_MIN 80


typedef struct
{
    int default_width;
    Stack *stack;
    Iterator *iterator;
    FIELD **fields;
} FieldSet;


static FieldSet *field_set;


static FIELD *create_field(int y, int x, int lines)
{
    int off_screen = 0;
    int width = field_set->default_width - PADDING;

    if (width < FIELD_CHARS_MIN) {
        off_screen = FIELD_CHARS_MIN - width;
    }

    return new_field(lines, width, y, x, off_screen, 0);
}

static void add_label(char *text, int y, int x)
{
    FIELD *label = create_field(y, x, 1);

    field_opts_off(label, O_ACTIVE);
    set_field_buffer(label, 0, text);

    stack_push(field_set->stack, label);
}

static void add_field(int y, int x, int lines, FieldType type, char *id)
{
    FIELD *field = create_field(y, x, lines);

    field_opts_off(field, O_AUTOSKIP);
    field_opts_on(field, O_ACTIVE);
    set_field_back(field, A_UNDERLINE);
    set_field_userptr(field, field_create_attributes(type, id));

    stack_push(field_set->stack, field);
}

static void add_href_field(char **regex_matches, int y)
{
    add_label(regex_matches[0], y, PADDING);
    add_field(y, strlen(regex_matches[0]) + PADDING, 1, FIELD_HREF, regex_matches[0]);

    pcre_free_substring_list((const char **) regex_matches);
}

static void add_href_fields(Link *link, int y)
{
    Iterator *arguments = href_arguments(link->href);

    add_label("Href arguments: ", y, PADDING);

    iterator_walk(arguments, (WalkCallback) function(void, (char **matches){
            add_href_field(matches, ++y);
    }));

    iterator_destroy(arguments);
}

static void add_headers(void)
{
    add_label("Headers: ", 2, PADDING);

    add_field(4, PADDING, 1, FIELD_HEADER, "header.1");
    add_field(5, PADDING, 1, FIELD_HEADER, "header.2");
    add_field(6, PADDING, 1, FIELD_HEADER, "header.3");
}

static void add_authentication(void)
{
    add_label("Basic auth: ", 8, PADDING);

    add_field(10, PADDING, 1, FIELD_USER, "user");
    add_field(11, PADDING, 1, FIELD_PASSWORD, "password");
}

static void build_left_side(Link *link)
{
    add_headers();
    add_authentication();

    add_label("Query string: ", 14, PADDING);
    add_field(16, PADDING, 1, FIELD_QUERY, "query");

    add_href_fields(link, 18);
}

static void build_right_side(void)
{
    int x = field_set->default_width + PADDING;

    add_label("Data:", PADDING, x);
    add_field(PADDING * 2, x, 20, FIELD_DATA, "data");
}

static void fields_to_array(void)
{
    stack_push(field_set->stack, NULL);
    field_set->fields = (FIELD **) stack_to_array(field_set->stack);
    stack_pop(field_set->stack);
}

static void build_form(Link *link)
{
    build_left_side(link);
    build_right_side();
    field_set->iterator = iterator_init_from_stack(field_set->stack);
    fields_to_array();
}

Iterator *request_field_iterator(void)
{
    return field_set->iterator;
}

FIELD **request_fields_to_array(void)
{
    return field_set->fields;
}

void request_fields_init(Link *link, int width)
{
    field_set = malloc(sizeof(FieldSet));
    field_set->default_width = width;
    field_set->stack = stack_init();

    build_form(link);
}

void request_fields_destroy(void)
{
    stack_destroy_callback(field_set->stack, (Free) field_destroy);
    iterator_destroy(field_set->iterator);
    free_multi(2, field_set, field_set->fields);
}
