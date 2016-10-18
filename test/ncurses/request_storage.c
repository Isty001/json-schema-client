#include "../unit_test.h"
#include "../../src/request.h"
#include "../../src/storage.h"
#include "../../src/ui/request_fields.h"
#include "../../src/ui/request_field_storage.h"
#include "../../src/ui/field.h"


static Link *create_link(void)
{
    Link *link = malloc(sizeof(Link));
    link->href = "/user/{id}";
    link->url = "example.com/user/{id}";
    link->method = "GET";

    return link;
}

MU_TEST(test_create_from_hidden)
{
    initscr();
    Link *link = create_link();

    storage_init("./fixtures");

    storage_set("request.form.GET.example.com/user/{id}.field.query", "?filter=10");

    Request *request = request_create_from_hidden_form(link);

    assert_string("GET", request->method);
    assert_string("example.com/user/{id}?filter=10", request->url);

    storage_destroy();
    free(link);
    endwin();
}

void run_request_ncurses_test(void)
{
    puts("NCURSES REQUEST STORAGE TEST");

    MU_RUN_TEST(test_create_from_hidden);

    MU_REPORT();
}
