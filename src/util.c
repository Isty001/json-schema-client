#include <stdarg.h>
#include <malloc.h>
#include <ctype.h>
#include <string.h>
#include "util.h"
#include "../lib/parson.h"


void free_multi(int count, ...)
{
    va_list items;
    va_start(items, count);

    for (int i = 0; i < count; i++) {
        void *item = va_arg(items, void *);

        _free(item);
    }
}

size_t file_size(FILE *file)
{
    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    fseek(file, 0, SEEK_SET);

    return (size_t) size;
}

char *file_read(char *path)
{
    FILE *file = fopen(path, "r");
    if (!file) {
        return NULL;
    }

    size_t size = file_size(file) + 1;
    char *buff = calloc(1, size);
    fread(buff, size, 1, file);
    fclose(file);

    return buff;
}

char *json_prettify(char *json)
{
    JSON_Value *parsed = json_parse_string(json);
    char *buffer = json_serialize_to_string_pretty(parsed);
    if (!buffer) {
        return NULL;
    }
    json_value_free(parsed);
    buffer[strlen(buffer)] = '\0';

    return buffer;
}

bool is_string_empty(char *buffer)
{
    if (!buffer) {
        return true;
    }

    while (*buffer != '\0') {
        if (!isspace(*buffer)) {
            return false;
        }
        buffer++;
    }
    return true;
}

char *remove_trailing_spaces(char *buffer)
{
    char *end = buffer + strlen(buffer) - 1;

    while (end > buffer && isspace(*end)) {
        end--;
    }
    *(end + 1) = '\0';

    return buffer;
}
