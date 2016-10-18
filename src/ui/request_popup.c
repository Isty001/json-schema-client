#include <form.h>
#include <malloc.h>
#include "ui.h"
#include "list.h"
#include "popup.h"
#include "../request.h"
#include "../curl.h"
#include "response.h"
#include "request_fields.h"
#include "request_field_storage.h"
#include "field.h"


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
    FORM *form;
    Popup *popup;
} RequestPopup;


static RequestPopup *popup;


static void init_request_popup(void)
{
    popup = malloc(sizeof(RequestPopup));
    popup->popup = ui_popup_open(0.85, 0.95);
}

static void create_form_window(void)
{
    int rows, cols;

    FIELD **fields = request_fields_to_array();
    FORM *form = new_form(fields);

    scale_form(form, &rows, &cols);
    keypad(popup->popup->window, TRUE);

    set_form_win(form, popup->popup->window);
    set_form_sub(form, derwin(popup->popup->window, rows, cols, 2, 2));

    post_form(form);
    popup->form = form;
}

static void destroy_request_popup(void)
{
    ui_popup_close();

    unpost_form(popup->form);
    free_form(popup->form);

    request_fields_destroy();
    free(popup);
}

static void save_form(Link *link)
{
    request_fields_save(request_field_iterator(), link);
}

static void send_request(Link *link)
{
    char *response;
    Request *request;

    request = request_create_from_fields(request_field_iterator(), link, field_read);
    response = curl_send_request(request);

    save_form(link);
    destroy_request_popup();
    response_show(response);
}

static void handle_input(Link *link)
{
    int ch;
    bool stop = false;
    FORM *form = popup->form;

    while (false == stop) {
        ch = wgetch(popup->popup->window);

        switch (ch) {
            on_input('\n', form_driver(form, REQ_VALIDATION); send_request(link); stop = true;)
            on_input(KEY_F(4), form_driver(form, REQ_VALIDATION); save_form(link))
            on_input(KEY_F(2), destroy_request_popup(); stop = true)
            on_input_form(KEY_DOWN, REQ_NEXT_LINE)
            on_input_form(KEY_UP, REQ_PREV_LINE)
            on_input_form(KEY_LEFT, REQ_PREV_CHAR)
            on_input_form(KEY_RIGHT, REQ_NEXT_CHAR)
            on_input_form(127, REQ_DEL_PREV)
            on_input_form(KEY_BACKSPACE, REQ_DEL_PREV)
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

void request_popup(void)
{
    Link *link = list_current_link();

    if (!link) {
        return;
    }

    init_request_popup();

    request_fields_init(link, popup->popup->width * 0.47);
    request_fields_load(request_field_iterator(), link);

    create_form_window();
    wrefresh(popup->popup->window);

    handle_input(link);
}
