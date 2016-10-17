#include "src/storage.h"
#include "src/curl.h"
#include "src/ui/ui.h"
#include "src/ui/list.h"
#include "src/ui/popup.h"
#include "src/ui/message.h"
#include "src/ui/response.h"
#include "sandbox/schema_list.h"


static void load_schemas(void)
{
//    schema_load("Post", "./test/fixture/schema2.json", "http://jsonplaceholder.typicode.com");
//    schema_load("Auth", "./test/fixture/auth_schema.json", "https://httpbin.org");
    LOAD_SCHEMAS;
}

static void init(void)
{
    storage_init("./storage");
    curl_init();
    schema_init();
    load_schemas();
    message_init();
    ui_init();
}

static void destroy(void)
{
    storage_dump();
    storage_destroy();
    curl_destroy();
    schema_destroy();
    response_destroy();
    ui_destroy();
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
        case KEY_NPAGE:
            list_event(REQ_SCR_DPAGE);
            break;
        case KEY_PPAGE:
            list_event(REQ_SCR_UPAGE);
            break;
        case 'h':
            popup_help();
            break;
        case 'e':
            popup_request();
            break;
    }
}

int main(void)
{
    init();

    int input;
    while ((input = getch()) != KEY_F(2)) {
        handle_input(input);
    }
    destroy();

    return 0;
}
