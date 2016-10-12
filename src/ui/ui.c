#include <assert.h>
#include <string.h>
#include <malloc.h>
#include "ui.h"
#include "list.h"
#include "../util.h"


#define DESCRIPTION_WIDTH full_width * 0.3


static int full_height, full_width;
static WINDOW *container, *list, *description, *message, *response;
static Popup *popup;


static void refresh_all(void)
{
    refresh();
    wrefresh(container);
    wrefresh(list);
    wrefresh(description);
    wrefresh(message);
    wrefresh(response);
}

static void create_sub_windows(void)
{
    list = ui_sub_window(full_height * 0.4, full_width * 0.3, 0, 0);
    description = ui_sub_window(full_height * 0.6, DESCRIPTION_WIDTH, full_height * 0.4, 0);

    message = ui_sub_window(full_height * 0.1, full_width * 0.7, 0, full_width * 0.3);
    response = ui_sub_window(full_height * 0.9, full_width * 0.7, full_height * 0.1, full_width * 0.3);
}

static void init_ncurses(void)
{
    initscr();
    start_color();
    use_default_colors();
    getmaxyx(stdscr, full_height, full_width);

    noecho();
    cbreak();
    keypad(stdscr, TRUE);
}

void ui_init(void)
{
    init_ncurses();

    container = newwin(full_height, full_width, 0, 0);
    box(container, 0, 0);
    assert(NULL != container);

    create_sub_windows();
    list_init(list);
    refresh_all();
}

WINDOW *ui_sub_window(double height, double width, double y, double x)
{
    WINDOW *sub_window = derwin(container, height, width, y, x);
    assert(NULL != sub_window);
    box(sub_window, 0, 0);

    return sub_window;
}

Popup *ui_popup_open(double height_rate, double width_rate)
{
    popup = malloc(sizeof(Popup));

    popup->height = full_height * height_rate;
    popup->width = full_width * width_rate;

    double y = (full_height - popup->height) / 2;
    double x = (full_width - popup->width) / 2;

    popup->window = ui_sub_window(popup->height, popup->width, y, x);

    return popup;
}

void ui_popup_close(void)
{
    delwin(popup->window);
    free(popup);
    ui_redraw();
}

void ui_destroy(void)
{
    list_destroy();
    delwin(container);
    delwin(list);
    delwin(description);
    delwin(message);
    delwin(response);
    endwin();
}

void ui_redraw(void)
{
    ui_destroy();
    ui_init();
}

static void print_bold_at(int y, int x, char *text)
{
    wattron(description, A_BOLD);
    mvwprintw(description, y, x, text);
    wattroff(description, A_BOLD);
}

static void print_with_title(int y, char *title, char *text)
{
    print_bold_at(y, 2, title);

    if (!text) {
        text = "(null)";
    }
    mvwprintw(description, y, 2 + strlen(title), text);
}

void ui_show_description(Link *link)
{
    wclear(description);

    print_bold_at(2, 2, link->display);

    print_with_title(6, "Title: ", link->title);
    print_with_title(8, "Href: ", link->href);
    print_with_title(10, "Method: ", link->method);
    print_with_title(12, "Rel: ", link->rel);
    print_with_title(14, "Description: ", link->description);

    box(description, 0, 0);
    wrefresh(description);
}

void ui_show_response(char *str)
{
    mvwprintw(response, PADDING, PADDING, str);
    wrefresh(response);
    refresh();
    free(str);
}
