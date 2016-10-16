#include <form.h>
#include <malloc.h>
#include "ui.h"
#include "list.h"
#include "popup.h"
#include "../request.h"
#include "field.h"
#include "../curl.h"


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
    int field_offset;
    int field_width;
    Iterator *field_iterator;
} RequestPopup;


static RequestPopup *request_popup;


static void init_request_popup(void)
{
    request_popup = malloc(sizeof(RequestPopup));
    request_popup->popup = ui_popup_open(0.75, 0.8);
    request_popup->fields = stack_init();
    request_popup->field_offset = 0;
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

static void add_field(int y, int x, int lines, FieldType type)
{
    FIELD *field = create_field(y, x, lines);

    field_opts_off(field, O_AUTOSKIP);
    set_field_back(field, A_UNDERLINE);
    set_field_userptr(field, field_create_attributes(type));

    stack_push(request_popup->fields, field);
}

static void build_left_side(void)
{
    add_label("Headers: ", 2, PADDING);

    add_field(4, PADDING, 1, FIELD_HEADER);
    add_field(7, PADDING, 1, FIELD_HEADER);
    add_field(10, PADDING, 1, FIELD_HEADER);
    add_field(13, PADDING, 1, FIELD_HEADER);
    add_field(16, PADDING, 1, FIELD_HEADER);

    add_label("Basic auth: ", 20, PADDING);

    add_field(22, PADDING, 1, FIELD_USER);
    add_field(25, PADDING, 1, FIELD_PASSWORD);
}

static void build_right_side(void)
{
    int y = request_popup->field_width + 2 * PADDING;

    add_label("Data:", 2, y);
    add_field(4, y, 20, FIELD_DATA);
}

static void build_form(void)
{
    build_left_side();
    build_right_side();
    request_popup->field_iterator = iterator_init((void *) request_popup->fields, request_popup->field_offset);
}

static void create_form_window(void)
{
    int rows, cols;
    FORM *form = new_form((FIELD **)stack_items(request_popup->fields));

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

    stack_destroy_callback(request_popup->fields, (Free) field_destroy);

    free(request_popup->field_iterator);
    free(request_popup);
}

static void save_form(void)
{
    request_save_form(request_popup->field_iterator, list_current_link());
}

static void send_request(void)
{
    char *response;
    Request *request;

    request = request_create_from_form(request_popup->field_iterator, list_current_link());
    response = curl_send_request(request);

    save_form();
    destroy_request_popup();
    ui_show_response(response);
}

static void handle_input(void)
{
    int ch;
    bool stop = false;
    FORM *form = request_popup->form;

    while (false == stop) {
        ch = wgetch(request_popup->popup->window);

        switch (ch) {
            on_input('\n', send_request();
                    stop = true;)
            on_input(KEY_F(4), save_form())
            on_input(KEY_F(2), destroy_request_popup();
                    stop = true)
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
    Link *current = list_current_link();

    if (!current) {
        return;
    }

    init_request_popup();
    build_form();

    request_load_form(request_popup->field_iterator, list_current_link());

    create_form_window();
    wrefresh(request_popup->popup->window);

    handle_input();
}
