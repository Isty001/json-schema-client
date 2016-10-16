#ifndef API_TEST_RESPONSE_H
#define API_TEST_RESPONSE_H


#include <ncurses.h>


void response_init_window(WINDOW *win);

void response_refresh(void);

void response_show(char *response);

void response_scroll(void);

void response_destroy(void);


#endif
