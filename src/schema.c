#include <malloc.h>
#include <string.h>
#include <stdlib.h>
#include "curl.h"
#include "../lib/parson.h"
#include "storage.h"
#include "schema_storage.h"


static Array *schemas;
static int link_count = 0;


static char *object_get(JSON_Object *obj, char *key)
{
    if (json_object_dothas_value(obj, key)) {
        return strdup(json_object_get_string(obj, key));
    }

    return NULL;
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

static Schema *create_schema(char *name, char *url, char *api_url, Array *links)
{
    Schema *schema = malloc(sizeof(Schema));
    schema->name = strdup(name);
    schema->url = strdup(url);
    schema->api_url = api_url ? strdup(api_url) : NULL;
    schema->links = iterator_init_from_stack_destroy(links);

    return schema;
}

static void load_links(JSON_Array *links, Array *link_stack, char *api_url)
{
    JSON_Object *link_schema;

    for (size_t i = 0; i < json_array_get_count(links); i++) {
        link_schema = json_array_get_object(links, i);
        array_push(link_stack, create_link(link_schema, api_url));
        link_count++;
    }
}

static char *load_json(char *url)
{
    char *content = file_read(url);

    if (NULL == content) {
        return curl_get(url);
    }

    return content;
}

static void load_links_and_destroy_root(JSON_Array *links, Array *link_stack, char *api_url, JSON_Value *root)
{
    load_links(links, link_stack, api_url);
    json_value_free(root);
}

static JSON_Array *parser_links_from_schema(JSON_Value *schema_root)
{
    JSON_Object *schema = json_object(schema_root);
    JSON_Value *links_root = json_object_get_value(schema, "links");

    return json_value_get_array(links_root);
}

static void load_links_from_url(char *url, char *api_url, Array *link_stack, char *storage_key)
{
    char *json = load_json(url);
    JSON_Value *schema_root = json_parse_string(json);
    JSON_Array *links = parser_links_from_schema(schema_root);
    char *serialized = json_serialize_to_string(schema_root);

    storage_set(storage_key, serialized);
    load_links_and_destroy_root(links, link_stack, api_url, schema_root);

    free_multi(2, serialized, json);
}

static void load_links_from_storage(char *schema_json, char *api_url, Array *link_stack)
{
    JSON_Value *schema_root = json_parse_string(schema_json);
    JSON_Array *links = parser_links_from_schema(schema_root);

    load_links_and_destroy_root(links, link_stack, api_url, schema_root);
}

void schema_load(char *name, char *url, char *api_url)
{
    char *schema_json, *storage_key = schema_storage_key_of(name);
    Array *link_stack = array_init();

    if (NULL != (schema_json = storage_get(storage_key))) {
        load_links_from_storage(schema_json, api_url, link_stack);
    } else {
        load_links_from_url(url, api_url, link_stack, storage_key);
    }

    free(storage_key);
    array_push(schemas, create_schema(name, url, api_url, link_stack));
}

Iterator *schema_iterator(void)
{
    return iterator_init_from_array(schemas);
}

int schema_count_links(void)
{
    return link_count;
}

void schema_init(void)
{
    schemas = array_init();
}

static void destroy_link(Link *link)
{
    link_count--;
    free_multi(8, link->href, link->method, link->rel, link->description, link->title, link->display, link->url, link);
}

static void destroy_schema(Schema *schema)
{
    iterator_walk(schema->links, (Free) destroy_link);
    iterator_destroy(schema->links);

    if (schema->api_url) {
        free(schema->api_url);
    }

    free_multi(3, schema->name, schema->url, schema);
}

void schema_destroy(void)
{
    Iterator *iterator = schema_iterator();

    iterator_walk(iterator, (Free) destroy_schema);
    iterator_destroy(iterator);
    array_destroy(schemas);
}

void schema_remove(char *name)
{
    array_filter(schemas, (ArrayFilter) function(bool, (Schema *schema){
        if (0 == strcmp(name, schema->name)) {
            destroy_schema(schema);

            return false;
        };
        return true;
    }));
}
