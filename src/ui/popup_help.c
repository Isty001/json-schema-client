#include <form.h>
#include "popup.h"
#include "ui.h"

#define underline(x) \
    wattron(popup->window, A_UNDERLINE); \
    x; \
    wattroff(popup->window, A_UNDERLINE);

static void print_help(Popup *popup, int y, char *text)
{
    mvwprintw(popup->window, y, PADDING, text);
}
void popup_help(void)
{
    Popup *popup = ui_popup_open(0.4, 0.3);

    underline(print_help(popup, 1, "General"));
    print_help(popup, 2, "h - Help");
    print_help(popup, 3, "F2 - Close popup/Quit");
    print_help(popup, 4, "k/l - Scroll response up/down");
    print_help(popup, 5, "e - Edit link request attributes");
    print_help(popup, 6, "Up/Down - Change link");
    underline(print_help(popup, 8, "In edit mode:"));
    print_help(popup, 9, "Page down/up - Change field");
    print_help(popup, 10, "Up/Down - Change line");
    print_help(popup, 11, "F4 - Save form");
    print_help(popup, 12, "Enter - Send request");

    wrefresh(popup->window);

    while (getch() != KEY_F(2));

    ui_popup_close();
}
