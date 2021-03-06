#include <malloc.h>
#include <assert.h>
#include <stdlib.h>
#include "list.h"
#include "ui.h"


typedef struct
{
    size_t count;
    int currently_built;
    int height;
    int width;
    ITEM **items;
    ITEM *currently_selected;
    WINDOW *window;
    MENU *menu;
} SchemaList;

typedef enum
{
    SCHEMA,
    LINK
} ItemType;

typedef struct
{
    ItemType type;
    void *ptr;
} ItemData;

static SchemaList *list;
static int last_selected_offset = 0;


static void *current(ItemType type)
{
    ItemData *data = item_userptr(current_item(list->menu));

    if (!data || type != data->type){
        return NULL;
    }
    return  data->ptr;
}

static void setup_list_menu(void)
{
    list->menu = new_menu(list->items);

    set_menu_win(list->menu, list->window);
    set_menu_sub(list->menu, derwin(list->window, list->height, list->width, 1, 1));
    set_menu_format(list->menu, list->height, 1);

    post_menu(list->menu);
}

static ItemData *item_data(ItemType type, void *ptr)
{
    ItemData *data = malloc(sizeof(ItemData));
    data->type = type;
    data->ptr = ptr;

    return data;
}

static ITEM *add_new_item(char *title)
{
    ITEM *item = new_item(title, NULL);
    list->items[list->currently_built++] = item;

    return item;
}

static void create_link_items(Schema *schema)
{
    iterator_walk(schema->links, (WalkCallback) function(void, (Link * link) {
        ITEM *item = add_new_item(link->display);

        set_item_userptr(item, item_data(LINK, link));
    }));
}

static void create_items(Iterator *schemas)
{
    iterator_walk(schemas, (WalkCallback) function(void, (Schema * schema) {
        ITEM *item = add_new_item(schema->name);
        item_opts_off(item, O_SELECTABLE);

        set_item_userptr(item, item_data(SCHEMA, schema));
        create_link_items(schema);
    }));

    list->items[list->count] = NULL;
}

static void set_sides(void)
{
    getmaxyx(list->window, list->height, list->width);

    list->height -= PADDING;
    list->width -= PADDING;
}

static void create_list(WINDOW *window, Iterator *schemas)
{
    list = malloc(sizeof(SchemaList));
    list->currently_built = 0;
    list->window = window;
    list->count = iterator_count(schemas) + schema_count_links();
    list->items = malloc(list->count * sizeof(ITEM));
}

void list_init(WINDOW *window)
{
    Iterator *schemas = schema_iterator();

    create_list(window, schemas);
    set_sides();
    create_items(schemas);
    assert(NULL != list->items);

    setup_list_menu();

    for (int i = last_selected_offset; i > 1; i--) {
        menu_driver(list->menu, REQ_DOWN_ITEM);
    }
    list_show_description();

    iterator_destroy(schemas);
}

void list_destroy()
{
    ITEM *item;
    unpost_menu(list->menu);
    free_menu(list->menu);

    for (size_t i = 0; i < list->count; ++i) {
        item = list->items[i];
        free(item_userptr(item));
        free_item(item);
    }

    free_multi(2, list, list->items);
}

void list_show_description(void)
{
    Link *link = list_current_link();

    if (link) {
        ui_show_description(link);
    }
}

static void set_last_selected_item(int input)
{
    int next = last_selected_offset + 1;

    if (input == REQ_UP_ITEM && next >= 1) {
        last_selected_offset--;
    } else if (input == REQ_DOWN_ITEM && next <= (int)list->count) {
        last_selected_offset++;
    }
}

void list_event(int input)
{
    set_last_selected_item(input);

    menu_driver(list->menu, input);
    wrefresh(list->window);
}

Link *list_current_link(void)
{
    return current(LINK);
}

Schema *list_current_schema(void)
{
    return current(SCHEMA);
}
