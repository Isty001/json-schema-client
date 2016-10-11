#ifndef API_TEST_ITERATOR_H
#define API_TEST_ITERATOR_H


#include "stack.h"


typedef void (*IteratorWalkCallback)(void *item);

typedef void *(*IteratorMapCallback)(void *item);

typedef struct iterator Iterator;


Iterator *iterator_init(void **items, int count);

Iterator *iterator_init_from_stack(Stack *stack);

Iterator *iterator_init_from_stack_destroy(Stack *stack);

void iterator_walk(Iterator *iterator, IteratorWalkCallback callback);

Iterator *iterator_map(Iterator *iterator, IteratorMapCallback callback);

void *iterator_seek(Iterator *iterator, int offset);

size_t iterator_count(Iterator *iterator);

void iterator_destroy(Iterator *iterator);


#endif
