#ifndef API_TEST_MENU_H
#define API_TEST_MENU_H


#include <ncurses.h>
#include <menu.h>
#include "../schema.h"


void list_init(WINDOW *window);

void list_event(int input);

void list_show_description(void);

Link *list_current_link(void);

Schema *list_current_schema(void);

void list_destroy(void);


#endif
