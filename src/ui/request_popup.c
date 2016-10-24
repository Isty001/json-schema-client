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
#include "form.h"


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
    FORM *form = popup->form;

    while ((ch = wgetch(popup->popup->window)) != KEY_F(2)) {
        switch (ch) {
            case KEY_ENTER:
            case '\n':
                form_driver(form, REQ_VALIDATION);
                send_request(link);
                return;
            case KEY_F(4):
                form_driver(form, REQ_VALIDATION);
                save_form(link);
                break;
            default:
                form_default_actions(form, ch);
                break;
        }
    }

    destroy_request_popup();
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

    popup->form = form_create(popup->popup->window, request_field_set());
    wrefresh(popup->popup->window);

    handle_input(link);
}
