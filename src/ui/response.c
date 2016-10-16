#include <string.h>
#include <ncurses.h>
#include "response.h"
#include "ui.h"


#define SCROLL_SPEED 2


static WINDOW *window;
static char *response = NULL;
static int first_line, last_line, height, width;


void response_init_window(WINDOW *win)
{
    window = win;
    getmaxyx(win, height, width);
    last_line = height;
}

void response_destroy(void)
{
    _free(response);
}

static int show_line(int i, int y, char *line)
{
    if (i >= first_line) {
        mvwprintw(window, y, PADDING, line);

        return y + (strlen(line) / width) + 1;
    }

    return y;
}

void response_refresh(void)
{
    if (!response) {
        return;
    }
    wclear(window);

    int i = 0, y = PADDING;
    size_t size = strlen(response);
    char *line, buff[size];

    strncpy(buff, response, size);
    line = strtok(buff, "\n");

    while (NULL != line && i++ <= last_line) {
        y = show_line(i, y, line);
        line = strtok(NULL, "\n");
    }

    wrefresh(window);
}

void response_show(char *str)
{
    _free(response);
    first_line = 0;
    response = strdup(str);
    response_refresh();
}

void response_scroll(int input)
{
    if (input == 'k') {
        wscrl(window, -SCROLL_SPEED);
        first_line -= SCROLL_SPEED;
        last_line -= SCROLL_SPEED;
    } else if (input == 'l') {
        first_line += SCROLL_SPEED;
        last_line += SCROLL_SPEED;
        wscrl(window, SCROLL_SPEED);
    }

    response_refresh();
}
