#include <string.h>
#include "message.h"
#include "ui.h"


#define COLOR_PAIR_SUCCESS 1
#define COLOR_PAIR_ERROR 2
#define COLOR_PAIR_DEFAULT 3


static char *message = NULL;
static WINDOW *window;
int color = COLOR_PAIR_DEFAULT;


void message_init(void)
{
    init_pair(COLOR_PAIR_SUCCESS, COLOR_GREEN, COLOR_BLACK);
    init_pair(COLOR_PAIR_ERROR, COLOR_RED, COLOR_BLACK);
    init_pair(COLOR_PAIR_DEFAULT, COLOR_WHITE, COLOR_BLACK);
}

void message_init_window(WINDOW *win)
{
    window = win;
}

void message_refresh(void)
{
    clear_window(window);

    if (!message) {
        return;
    }

    wattron(window, COLOR_PAIR(color));
    mvwprintw(window, PADDING, PADDING, message);
    wattroff(window, COLOR_PAIR(color));
    wrefresh(window);
}

static void update_message(char *msg, int col)
{
    _free(message);
    color = col;
    message = strdup(msg);
    message_refresh();
}

void message_success(char *text)
{
    update_message(text, COLOR_PAIR_SUCCESS);
}

void message_error(char *text)
{
    update_message(text, COLOR_PAIR_ERROR);
}
