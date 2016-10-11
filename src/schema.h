#ifndef API_TEST_SCHEMA_H
#define API_TEST_SCHEMA_H


#include "iterator.h"


typedef struct
{
    char *display;
    char *url;
    char *method;
    char *rel;
    char *href;
    char *description;
    char *title;
} Link;

typedef struct
{
    char *name;
    Iterator *links;
} Schema;


void schema_init(void);

void schema_load(char *name, char *url, char *api_url);

Iterator *schema_iterator(void);

int schema_count_links(void);

void schema_destroy(void);


#endif
