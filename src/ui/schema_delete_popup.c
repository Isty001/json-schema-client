#include "../schema.h"
#include "list.h"
#include "ui.h"
#include "../schema_storage.h"
#include "../storage.h"
#include "message.h"


static void print_message(WINDOW *window, Schema *schema)
{
    mvwprintw(window, PADDING, PADDING, "Are you sure you want to remove %s schema?", schema->name);
    mvwprintw(window, PADDING + 1, PADDING, "Press ENTER to proceed, F2 to cancel");
    wrefresh(window);
}

static void remove_schema(Schema *schema)
{
    char buff[100];
    sprintf(buff, "Successfully removed \"%s\" schema", schema->name);

    schema_remove(schema->name);
    schema_storage_serialize();
    storage_dump();

    message_success(buff);
}

void schema_remove_popup(void)
{
    Schema *schema = list_current_schema();

    if (!schema) {
        return;
    }

    Popup *popup = ui_popup_open(0.25, 0.40);
    print_message(popup->window, schema);

    int ch;
    while (KEY_F(2) != (ch = wgetch(popup->window))) {
        if (ch == KEY_ENTER || ch == '\n') {
            remove_schema(schema);
            break;
        }
    }

    ui_popup_close();
}
