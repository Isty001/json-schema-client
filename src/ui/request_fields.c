#include <form.h>
#include <pcre.h>
#include <string.h>
#include "request_fields.h"
#include "ui.h"
#include "field.h"
#include "../href.h"


typedef struct
{
    FieldSet *set;
    Iterator *iterator;
} RequestFieldSet;


static RequestFieldSet *fields;


static void add_href_field(char **regex_matches, int y)
{
    field_set_add_label(fields->set, regex_matches[0], y, PADDING);
    field_set_add_field(fields->set, y, strlen(regex_matches[0]) + PADDING, 1, FIELD_HREF, regex_matches[0]);

    pcre_free_substring_list((const char **) regex_matches);
}

static void add_href_fields(Link *link, int y)
{
    Iterator *arguments = href_arguments(link->href);

    field_set_add_label(fields->set, "Href arguments: ", y, PADDING);

    iterator_walk(arguments, (WalkCallback) function(void, (char **matches){
            add_href_field(matches, ++y);
    }));

    iterator_destroy(arguments);
}

static void add_headers(void)
{
    field_set_add_label(fields->set, "Headers: ", 2, PADDING);

    field_set_add_field(fields->set, 4, PADDING, 1, FIELD_HEADER, "header.1");
    field_set_add_field(fields->set, 5, PADDING, 1, FIELD_HEADER, "header.2");
    field_set_add_field(fields->set, 6, PADDING, 1, FIELD_HEADER, "header.3");
}

static void add_authentication(void)
{
    field_set_add_label(fields->set, "Basic auth: ", 8, PADDING);

    field_set_add_field(fields->set, 10, PADDING, 1, FIELD_USER, "user");
    field_set_add_field(fields->set, 11, PADDING, 1, FIELD_PASSWORD, "password");
}

static void build_left_side(Link *link)
{
    add_headers();
    add_authentication();

    field_set_add_label(fields->set, "Query string: ", 14, PADDING);
    field_set_add_field(fields->set, 16, PADDING, 1, FIELD_QUERY, "query");

    add_href_fields(link, 18);
}

static void build_right_side(void)
{
    int x = fields->set->width + PADDING * 2;

    field_set_add_label(fields->set, "Data:", PADDING, x);
    field_set_add_field(fields->set, PADDING * 2, x, 20, FIELD_DATA, "data");
}

static void build_form(Link *link)
{
    build_left_side(link);
    build_right_side();
    fields->iterator = iterator_init_from_array(fields->set->stack);

    field_set_finalize(fields->set);
}

Iterator *request_field_iterator(void)
{
    return fields->iterator;
}

FieldSet *request_field_set(void)
{
    return fields->set;
}

void request_fields_init(Link *link, int width)
{
    fields = malloc(sizeof(RequestFieldSet));
    fields->set = field_set_init(width, 80);

    build_form(link);
}

void request_fields_destroy(void)
{
    iterator_destroy(fields->iterator);
    field_set_destroy(fields->set);
    free(fields);
}
