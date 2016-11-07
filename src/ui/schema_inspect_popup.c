#include "../schema.h"
#include "list.h"
#include "../storage.h"
#include "scroll.h"
#include "ui.h"


static void handle_input(Scroll *scroll, int ch)
{
    switch (ch) {
        case 'k':
            scroll_up(scroll);
            scroll_display(scroll);
            break;
        case 'l':
            scroll_down(scroll);
            scroll_display(scroll);
            break;
    }
}

void schema_inspect_popup(void)
{
    Schema *schema = list_current_schema();
    if (!schema) {
        return;
    }
    char *key = schema_storage_key(schema->name);
    char *json = json_prettify(storage_get(key));
    Popup *popup = ui_popup_open(0.80, 0.70);
    Scroll *scroll = scroll_init(popup->window, 4);

    scroll_replace_content(scroll, json);
    scroll_display(scroll);

    int ch;
    while (KEY_F(2) != (ch = wgetch(popup->window))) {
        handle_input(scroll, ch);
    }

    scroll_destroy(scroll);
    ui_popup_close();
}
