#include "response.h"
#include "scroll.h"
#include "../util.h"


static Scroll *wscroll;


void response_init_window(WINDOW *win)
{
    wscroll = scroll_init(win, 4);
}

void response_destroy(void)
{
    scroll_destroy(wscroll);
}

void response_refresh(void)
{
    scroll_display(wscroll);
}

void response_show(char *str)
{
    if (!str) {
        return;
    }

    char *pretty = json_prettify(str);
    if (!pretty) {
        pretty = str;
    }

    scroll_replace_content(wscroll, pretty);

    response_refresh();
}


void response_scroll(int input)
{
    if (input == 'k') {
        scroll_down(wscroll);
    } else if (input == 'l') {
        scroll_up(wscroll);
    }

    response_refresh();
}
