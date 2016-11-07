#include <malloc.h>
#include <string.h>
#include "scroll.h"
#include "ui.h"


struct scroll
{
    WINDOW *window;
    int height;
    int width;
    int speed;
    int first;
    int last;
    char *content;
};


Scroll *scroll_init(WINDOW *window, int speed)
{
    Scroll *scroll = malloc(sizeof(Scroll));
    scroll->window = window;
    scroll->speed = speed;
    getmaxyx(window, scroll->height, scroll->width);
    scroll->first = 0;
    scroll->last = scroll->height;
    scroll->content = NULL;

    return scroll;
}

void scroll_replace_content(Scroll *scroll, char *content)
{
    _free(scroll->content);
    scroll->content = content;
}

void scroll_up(Scroll *scroll)
{
    scroll->first += scroll->speed;
    scroll->last += scroll->speed;
    wscrl(scroll->window, scroll->speed);
}

void scroll_down(Scroll *scroll)
{
    if (scroll->last != scroll->height) {
        wscrl(scroll->window, -scroll->speed);
        scroll->first -= scroll->speed;
        scroll->last -= scroll->speed;
    }
}

static int show_line(Scroll *scroll, int i, int y, char *line)
{
    if (i >= scroll->first) {
        mvwprintw(scroll->window, y, PADDING, line);

        return y + (strlen(line) / scroll->width) + 1;
    }

    return y;
}

void scroll_display(Scroll *scroll)
{
    if (!scroll->content) {
        return;
    }
    clear_window(scroll->window);

    int i = 0, y = PADDING / 2;
    size_t size = strlen(scroll->content);
    char *line, buff[size];

    strncpy(buff, scroll->content, size);
    line = strtok(buff, "\n");

    while (NULL != line && i++ <= scroll->last) {
        y = show_line(scroll, i, y, line);
        line = strtok(NULL, "\n");
    }

    wrefresh(scroll->window);
}

void scroll_destroy(Scroll *scroll)
{
    _free(scroll->content);
    free(scroll);
}
