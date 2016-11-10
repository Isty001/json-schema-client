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

void help_popup(void)
{
    Popup *popup = ui_popup_open(0.5, 0.4);

    underline(print_help(popup, 1, "General"));
    print_help(popup, 2, "h - Help");
    print_help(popup, 3, "Enter - Send request");
    print_help(popup, 4, "F2 - Close popup/Quit");
    print_help(popup, 5, "k/l - Scroll response up/down");
    print_help(popup, 6, "Up/Down - Change link");
    print_help(popup, 7, "i - Inspect selected schema");
    print_help(popup, 8, "d - Remove selected schema");

    underline(print_help(popup, 10, "Editing a link request:"));
    print_help(popup, 11, "e - Edit link request attributes");
    print_help(popup, 12, "Page down/up - Change field");
    print_help(popup, 13, "Up/Down - Change line");
    print_help(popup, 14, "F4 - Save form");
    print_help(popup, 15, "Enter - Send request");

    underline(print_help(popup, 17, "Adding new schema"));
    print_help(popup, 18, "s - New schema window");
    print_help(popup, 19, "Enter - load schema");

    wrefresh(popup->window);

    while (wgetch(popup->window) != KEY_F(2));

    ui_popup_close();
}
