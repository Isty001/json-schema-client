#ifndef API_TEST_UTIL_H
#define API_TEST_UTIL_H


#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>


#define resize(structure, item, operation)\
         structure->allocated operation sizeof(item); \
         structure->items = realloc(structure->items, structure->allocated); \

#define increase(structure, item) resize(structure, item, +=)

#define decrease(structure, item) resize(structure, item, -=)

#define function(return_type, function_body) ({ return_type __fn__ function_body __fn__; })

#define die(msg, ...) fprintf(stderr, msg, __VA_ARGS__); exit(EXIT_FAILURE);

#define unused(x) __attribute__((__unused__)) x

#define flag_exists(bitmask, flag) ((bitmask & flag) == flag)

#define _free(x) if (NULL != x) free(x)

#define dup_buffer(buff) strdup(remove_trailing_spaces(buff))


typedef void (*Free)(void *);

void free_multi(int count, ...);

size_t file_size(FILE *file);

char *json_prettify(char *json);

bool is_string_empty(char *buffer);

char *remove_trailing_spaces(char *buffer);


static inline void print_int(int i)
{
    printf("%d\n", i);
}

#endif
