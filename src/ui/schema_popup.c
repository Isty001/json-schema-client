#include "schema_fields.h"
#include "form.h"
#include "message.h"
#include "../schema.h"
#include "ui.h"


void load_schema(char *name, char *url, char *api_url)
{
    char msg[100];

    schema_load(name, url, api_url);
    sprintf(msg, "Schema \"%s\" successfully loaded", name);

    message_success(msg);
}

static void load_schema_from_form(FieldSet *set)
{
    char *name = field_read(set->array[1]);
    char *url = field_read(set->array[3]);
    char *api_url = field_read(set->array[5]);

    if (!name || !url || !api_url) {
        message_error("Unable to load schema, because some values are missing.");
    } else {
        load_schema(name, url, api_url);
    }
}

void schema_popup(void)
{
    Popup *popup = ui_popup_open(0.5, 0.7);

    FieldSet *set = schema_fields_init(popup->width * 0.75);

    FORM *form = form_create(popup->window, set);
    wrefresh(popup->window);

    int ch;
    while ((ch = wgetch(popup->window)) != KEY_F(2)) {
        if (ch == KEY_ENTER || ch == '\n') {
            form_driver(form, REQ_VALIDATION);
            load_schema_from_form(set);
            break;
        } else {
            form_default_actions(form, ch);
        }
    }

    unpost_form(form);
    free_form(form);
    schema_fields_destroy();
    ui_popup_close();
}
