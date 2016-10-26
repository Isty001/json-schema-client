#include "src/storage.h"
#include "src/curl.h"
#include "src/ui/ui.h"
#include "src/ui/list.h"
#include "src/ui/popup.h"
#include "src/ui/message.h"
#include "src/ui/response.h"
#include "src/schema_storage.h"


static void init(void)
{
    storage_init(STORAGE_DIR);
    curl_init();
    schema_init();
    schema_storage_unserialize();
    ui_init();
    message_init();
}

static void destroy(void)
{
    curl_destroy();
    schema_storage_serialize();
    schema_destroy();
    storage_dump();
    storage_destroy();
    response_destroy();
    ui_destroy();
}

static void send_hidden_form()
{
    Link *link = list_current_link();

    if (!link) {
        return;
    }

    Request *request = request_create_from_hidden_form(link);
    char *response = curl_send_request(request);

    response_show(response);
}

static void handle_input(int input)
{
    switch (input) {
        case 'k':
        case 'l':
            response_scroll(input);
            break;
        case KEY_RESIZE:
            ui_redraw();
        case KEY_DOWN:
            list_event(REQ_DOWN_ITEM);
            list_show_description();
            break;
        case KEY_UP:
            list_event(REQ_UP_ITEM);
            list_show_description();
            break;
        case 'h':
            help_popup();
            break;
        case 's':
            schema_popup();
            break;
        case 'e':
            request_popup();
            break;
        case '\n':
            send_hidden_form();
            break;
    }
}

int main(void)
{
    init();

    int input;
    while ((input = wgetch(ui_container())) != KEY_F(2)) {
        handle_input(input);
    }
    destroy();

    return 0;
}
