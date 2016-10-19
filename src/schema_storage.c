#include <parson.h>
#include "iterator.h"
#include "schema.h"
#include "storage.h"


#define json_object_get_char(o, key) (char *) json_object_dotget_string(schema, key)


JSON_Array *serialized_array;


static void serialize_api_url(Schema *schema, JSON_Object *object)
{
    if (schema->api_url) {
        json_object_dotset_string(object, "api_url", schema->api_url);
    } else {
        json_object_dotset_null(object, "api_url");
    }
}

static void serialize_schema(Schema *schema)
{
    JSON_Value *object_root = json_value_init_object();
    JSON_Object *object = json_object(object_root);

    json_object_dotset_string(object, "name", schema->name);
    json_object_dotset_string(object, "url", schema->url);

    serialize_api_url(schema, object);

    json_array_append_value(serialized_array, object_root);
}

void schema_storage_serialize(void)
{
    JSON_Value *array_root = json_value_init_array();
    char *serialized;
    serialized_array = json_array(array_root);

    Iterator *iterator = schema_iterator();

    iterator_walk(iterator, (WalkCallback) serialize_schema);
    serialized = json_serialize_to_string(array_root);

    iterator_destroy(iterator);
    json_value_free(array_root);

    storage_set(SCHEMA_LIST_KEY, serialized);
    free(serialized);
}

static void load_schemas(JSON_Array *schemas)
{
    char *name, *url, *api_url;
    JSON_Object *schema;

    for (size_t i = 0; i < json_array_get_count(schemas); i++) {
        schema = json_array_get_object(schemas, i);

        name = json_object_get_char(schema, "name");
        url = json_object_get_char(schema, "url");
        api_url = json_object_get_char(schema, "api_url");

        schema_load(name, url, api_url);
    }
}

void schema_storage_unserialize(void)
{
    char *serialized = storage_get(SCHEMA_LIST_KEY);
    if (!serialized) {
        return;
    }

    JSON_Value *array_root = json_parse_string(serialized);

    load_schemas(json_array(array_root));

    json_value_free(array_root);
}
