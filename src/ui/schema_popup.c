#include "ui.h"


void schema_popup(void)
{
    Popup *popup = ui_popup_open(0.30, 0.70);

    while (wgetch(popup->window) != KEY_F(2))

    ui_popup_close();
}
