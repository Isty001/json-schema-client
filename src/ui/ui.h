#ifndef API_TEST_UI_H
#define API_TEST_UI_H


#include <ncurses.h>
#include <form.h>
#include "../schema.h"


#define PADDING 2


typedef struct
{
    WINDOW *window;
    double width;
    double height;
} Popup;


void ui_init(void);

void ui_redraw(void);

WINDOW *ui_sub_window(double height, double width, double y, double x);

Popup *ui_popup_open(double height_rate, double width_rate);

void ui_popup_close(void);

void ui_show_response(char *response);

void ui_show_description(Link *link);

void ui_destroy(void);


#endif
