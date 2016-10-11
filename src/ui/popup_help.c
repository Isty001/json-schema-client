#include <form.h>
#include "popup.h"
#include "ui.h"


void popup_help(void)
{
    Popup *popup = ui_popup_open(0.2, 0.5);

    mvwprintw(popup->window, 1, PADDING, "h - Help");
    mvwprintw(popup->window, 2, PADDING, "F2 - Close popup/Quit");
    mvwprintw(popup->window, 3, PADDING, "r - Reload");

    wrefresh(popup->window);

    while (getch() != KEY_F(2));

    ui_popup_close();
}
