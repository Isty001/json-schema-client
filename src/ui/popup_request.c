#include <form.h>
#include <malloc.h>
#include <string.h>
#include "ui.h"
#include "list.h"
#include "popup.h"
#include "../request.h"
#include "field.h"
#include "../curl.h"
#include "../href.h"


#define FIELD_CHARS_MIN 60


#define on_input(key, action) \
        case key: \
            action; \
            break; \

#define on_input_form(key, event) \
        case key: \
            form_driver(form, event); \
            break; \

#define on_input_form_multiple(key, event1, event2) \
        case key: \
            form_driver(form, event1); \
            form_driver(form, event2); \
            break; \

typedef struct
{
    Stack *fields;
    FORM *form;
    Popup *popup;
    int field_width;
    Iterator *field_iterator;
} RequestPopup;


static RequestPopup *request_popup;


static void init_request_popup(void)
{
    request_popup = malloc(sizeof(RequestPopup));
    request_popup->popup = ui_popup_open(0.75, 0.8);
    request_popup->fields = stack_init();
    request_popup->field_width = request_popup->popup->width * 0.47;
}

static FIELD *create_field(int y, int x, int lines)
{
    int off_screen = 0;

    if (request_popup->field_width < FIELD_CHARS_MIN) {
        off_screen = FIELD_CHARS_MIN - request_popup->field_width;
    }

    return new_field(lines, request_popup->field_width, y, x, off_screen, 0);
}

static void add_label(char *text, int y, int x)
{
    FIELD *label = create_field(y, x, 1);

    field_opts_off(label, O_ACTIVE);
    set_field_buffer(label, 0, text);

    stack_push(request_popup->fields, label);
}

static void add_field(int y, int x, int lines, FieldType type, char *id)
{
    FIELD *field = create_field(y, x, lines);

    field_opts_off(field, O_AUTOSKIP);
    set_field_back(field, A_UNDERLINE);
    set_field_userptr(field, field_create_attributes(type, id));

    stack_push(request_popup->fields, field);
}

static void add_href_field(char **regex_matches)
{
    static int from = 28;

    add_label(regex_matches[0], from += PADDING, PADDING);
    add_field(from, strlen(regex_matches[0]) + PADDING, 1, FIELD_HREF, regex_matches[0]);
}

static void add_href_fields(Link *link)
{
    Iterator *arguments = href_arguments(link->href);

    add_label("Href arguments: ", 26, PADDING);

    iterator_walk(arguments, (WalkCallback) add_href_field);

    iterator_destroy(arguments);
}

static void build_left_side(Link *link)
{
    add_label("Headers: ", 2, PADDING);

    add_field(4, PADDING, 1, FIELD_HEADER, "header.1");
    add_field(6, PADDING, 1, FIELD_HEADER, "header.2");
    add_field(8, PADDING, 1, FIELD_HEADER, "header.3");

    add_label("Basic auth: ", 15, PADDING);

    add_field(17, PADDING, 1, FIELD_USER, "user");
    add_field(19, PADDING, 1, FIELD_PASSWORD, "password");

    add_label("Query string: ", 21, PADDING);

    add_field(23, PADDING, 1, FIELD_QUERY, "query");

    add_href_fields(link);
}

static void build_right_side(void)
{
    int y = request_popup->field_width + 2 * PADDING;

    add_label("Data:", 2, y);
    add_field(4, y, 20, FIELD_DATA, "data");
}

static void build_form(Link *link)
{
    build_left_side(link);
    build_right_side();
    request_popup->field_iterator = iterator_init_from_stack(request_popup->fields);
    stack_push(request_popup->fields, NULL);
}

static void create_form_window(void)
{
    int rows, cols;

    FORM *form = new_form((FIELD **) stack_to_array(request_popup->fields));

    scale_form(form, &rows, &cols);
    keypad(request_popup->popup->window, TRUE);

    set_form_win(form, request_popup->popup->window);
    set_form_sub(form, derwin(request_popup->popup->window, rows, cols, 2, 2));

    post_form(form);
    request_popup->form = form;
}

static void destroy_request_popup(void)
{
    ui_popup_close();

    unpost_form(request_popup->form);
    free_form(request_popup->form);

    stack_pop(request_popup->fields);
    stack_destroy_callback(request_popup->fields, (Free) free_field);

    iterator_destroy(request_popup->field_iterator);
    free(request_popup);
}

static void save_form(Link *link)
{
    request_save_form(request_popup->field_iterator, link);
}

static void send_request(Link *link)
{
    char *response;
    Request *request;

    request = request_create_from_form(request_popup->field_iterator, link);
    response = curl_send_request(request);

    save_form(link);
    destroy_request_popup();
    ui_show_response(response);
}

static void handle_input(Link *link)
{
    int ch;
    bool stop = false;
    FORM *form = request_popup->form;

    while (false == stop) {
        ch = wgetch(request_popup->popup->window);

        switch (ch) {
            on_input('\n', form_driver(form, REQ_VALIDATION); send_request(link); stop = true;)
            on_input(KEY_F(4), form_driver(form, REQ_VALIDATION); save_form(link))
            on_input(KEY_F(2), destroy_request_popup(); stop = true)
            on_input_form(KEY_DOWN, REQ_NEXT_LINE)
            on_input_form(KEY_UP, REQ_PREV_LINE)
            on_input_form(KEY_LEFT, REQ_PREV_CHAR)
            on_input_form(KEY_RIGHT, REQ_NEXT_CHAR)
            on_input_form(127, REQ_DEL_PREV)
            on_input_form(KEY_DC, REQ_DEL_CHAR)
            on_input_form(KEY_HOME, REQ_BEG_LINE)
            on_input_form(KEY_END, REQ_END_LINE)
            on_input_form_multiple(KEY_PPAGE, REQ_PREV_FIELD, REQ_END_LINE)
            on_input_form_multiple(KEY_NPAGE, REQ_NEXT_FIELD, REQ_END_LINE)
            default:
                form_driver(form, ch);
                break;
        }
        if (stop) break;
    }
}

void popup_request(void)
{
    Link *link = list_current_link();

    if (!link) {
        return;
    }

    init_request_popup();
    build_form(link);

    request_load_form(request_popup->field_iterator, list_current_link());

    create_form_window();
    wrefresh(request_popup->popup->window);

    handle_input(link);
}
