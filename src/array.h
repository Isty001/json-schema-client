#ifndef API_TEST_STACK_H
#define API_TEST_STACK_H


#include <stdbool.h>
#include "util.h"


typedef struct array Array;

typedef bool (*ArrayFilter)(void *ptr);


Array *array_init(void);

void *array_pop(Array *array);

void array_push(Array *array, void *ptr);

bool array_is_empty(Array *array);

void *array_last(Array *array);

void **array_values(Array *array);

size_t array_count(Array *stack);

void array_filter(Array *array, ArrayFilter filter);

void array_destroy(Array *stack);

void array_destroy_callback(Array *stack, Free callback);


#endif
