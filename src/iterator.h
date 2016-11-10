#ifndef API_TEST_ITERATOR_H
#define API_TEST_ITERATOR_H


#include "array.h"


typedef void (*WalkCallback)(void *item);

typedef void (*WalkIndexCallback)(void *item, int i);

typedef void *(*MapCallback)(void *item);

typedef struct iterator Iterator;


Iterator *iterator_init(void **items, int count);

Iterator *iterator_init_from_array(Array *array);

Iterator *iterator_init_from_stack_destroy(Array *array);

void iterator_walk(Iterator *iterator, WalkCallback callback);

void iterator_walk_index(Iterator *iterator, WalkIndexCallback callback);

Iterator *iterator_map(Iterator *iterator, MapCallback callback);

void *iterator_seek(Iterator *iterator, int offset);

size_t iterator_count(Iterator *iterator);

void iterator_destroy(Iterator *iterator);


#endif
