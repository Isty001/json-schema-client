#include <string.h>
#include <ncurses.h>
#include "response.h"
#include "ui.h"


#define SCROLL_SPEED 4


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
    if (!str) {
        return;
    }

    _free(response);
    first_line = 0;

    char *pretty = json_prettify(str);

    response = pretty ? pretty : str;
    response_refresh();
}

void scroll_down(void)
{
    if (last_line != height) {
        wscrl(window, -SCROLL_SPEED);
        first_line -= SCROLL_SPEED;
        last_line -= SCROLL_SPEED;
    }
}

void scroll_up(void)
{
    first_line += SCROLL_SPEED;
    last_line += SCROLL_SPEED;
    wscrl(window, SCROLL_SPEED);
}

void response_scroll(int input)
{
    if (input == 'k') {
        scroll_down();
    } else if (input == 'l') {
        scroll_up();
    }

    response_refresh();
}
