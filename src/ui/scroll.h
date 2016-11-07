#ifndef CLIENT_SCROLL_H
#define CLIENT_SCROLL_H


#include <curses.h>


typedef struct scroll Scroll;


Scroll *scroll_init(WINDOW *window, int speed);

void scroll_replace_content(Scroll *scroll, char *content);

void scroll_up(Scroll *scroll);

void scroll_down(Scroll *scroll);

void scroll_display(Scroll *scroll);

void scroll_destroy(Scroll *scroll);


#endif
