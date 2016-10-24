#ifndef API_TEST_FORM_H
#define API_TEST_FORM_H


#include <ncurses.h>
#include "field.h"


FORM *form_create(WINDOW *window, FieldSet *set);

void form_default_actions(FORM *form, int input);


#endif
