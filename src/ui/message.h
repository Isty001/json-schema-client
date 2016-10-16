#ifndef API_TEST_MESSAGE_H
#define API_TEST_MESSAGE_H


#include <ncurses.h>


void message_init(void);

void message_init_window(WINDOW *container);

void message_success(char *msg);

void message_error(char *msg);

void message_refresh(void);


#endif
