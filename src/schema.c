#include <malloc.h>
#include <string.h>
#include <stdlib.h>
#include "curl.h"
#include "stack.h"
#include "schema.h"
#include "../lib/parson.h"
#include "util.h"
#include "storage.h"


#define BASE_LINK_STORAGE_KEY "schema.%s.links"


static Stack *schemas;
static int link_count = 0;

static char *object_get(JSON_Object *obj, char *key)
{
    if (json_object_dothas_value(obj, key)) {
        return strdup(json_object_get_string(obj, key));
    }

    return NULL;
}

static void destroy_link(Link *link)
{
    free_multi(8, link->href, link->method, link->rel, link->description, link->title, link->display, link->url, link);
}

static void set_link_url(Link *link, char *api_url)
{
    if (NULL != api_url) {
        asprintf(&link->url, "%s%s", api_url, link->href);
    } else {
        link->url = strdup(link->href);
    }
}

static void set_link_href(JSON_Object *link_schema, Link *link)
{
    link->href = object_get(link_schema, "href");

    if (!link->href) {
        perror("Link href cant be empty");
        exit(EXIT_FAILURE);
    }
}

static Link *create_link(JSON_Object *link_schema, char *api_url)
{
    Link *link = malloc(sizeof(Link));

    set_link_href(link_schema, link);
    set_link_url(link, api_url);
    link->method = object_get(link_schema, "method");
    link->rel = object_get(link_schema, "rel");
    link->description = object_get(link_schema, "description");
    link->title = object_get(link_schema, "title");
    asprintf(&link->display, "%s - %s", link->method, link->href);

    return link;
}

static Schema *create_schema(char *name, Stack *links)
{
    Schema *schema = malloc(sizeof(Schema));
    schema->name = strdup(name);
    schema->links = iterator_init_from_stack_destroy(links);

    return schema;
}

static void load_links(JSON_Array *links, Stack *link_stack, char *api_url)
{
    JSON_Object *link_schema;

    for (size_t i = 0; i < json_array_get_count(links); i++) {
        link_schema = json_array_get_object(links, i);
        stack_push(link_stack, create_link(link_schema, api_url));
        link_count++;
    }
}

static char *load_from_file(FILE *file)
{
    size_t size = file_size(file) + 1;
    char *content = calloc(1, size);

    fread(content, size, 1, file);
    fclose(file);

    return content;
}

static char *load_json(char *url)
{
    char *content;

    FILE *file = fopen(url, "r");

    if (file) {
        content = load_from_file(file);
    } else {
        content = curl_get(url);
    }

    return content;
}

static char *create_storage_key(char *name)
{
    char *key = malloc(strlen(BASE_LINK_STORAGE_KEY) + strlen(name));

    sprintf(key, BASE_LINK_STORAGE_KEY, name);

    return key;
}

static void load_links_and_destroy_root(JSON_Array *links, Stack *link_stack, char *api_url, JSON_Value *root)
{
    load_links(links, link_stack, api_url);
    json_value_free(root);
}

static void load_links_from_url(char *url, char *api_url, Stack *link_stack, char *storage_key)
{
    char *json = load_json(url);

    JSON_Value *schema_root = json_parse_string(json);
    JSON_Object *schema = json_value_get_object(schema_root);
    JSON_Value *links_root = json_object_get_value(schema, "links");
    JSON_Array *links = json_value_get_array(links_root);
    char *serialized = json_serialize_to_string(links_root);

    storage_set(storage_key, serialized);
    load_links_and_destroy_root(links, link_stack, api_url, schema_root);

    free_multi(2, serialized, json);
}

static void load_links_from_storage(char *links_json, char *api_url, Stack *link_stack)
{
    JSON_Value *links_root = json_parse_string(links_json);
    JSON_Array *links = json_value_get_array(links_root);

    load_links_and_destroy_root(links, link_stack, api_url, links_root);
}

void schema_load(char *name, char *url, char *api_url)
{
    char *links_json, *storage_key = create_storage_key(name);
    Stack *link_stack = stack_init();

    if (NULL != (links_json = storage_get(storage_key))) {
        load_links_from_storage(links_json, api_url, link_stack);
    } else {
        load_links_from_url(url, api_url, link_stack, storage_key);
    }

    free(storage_key);
    stack_push(schemas, create_schema(name, link_stack));
}

Iterator *schema_iterator(void)
{
    return iterator_init_from_stack(schemas);
}

int schema_count_links(void)
{
    return link_count;
}

void schema_init(void)
{
    schemas = stack_init();
}

void destroy_schema(Schema *schema)
{
    iterator_walk(schema->links, (Free) destroy_link);
    iterator_destroy(schema->links);

    free(schema->name);
    free(schema);
}

void schema_destroy(void)
{
    Iterator *iterator = schema_iterator();

    iterator_walk(iterator, (Free) destroy_schema);
    iterator_destroy(iterator);
    stack_destroy(schemas);
}
