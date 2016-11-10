#include <malloc.h>
#include "iterator.h"


struct iterator
{
    void **items;
    int count;
};


Iterator *iterator_init(void **items, int count)
{
    Iterator *iterator = malloc(sizeof(Iterator));
    iterator->count = count;
    iterator->items = items;

    return iterator;
}

Iterator *iterator_init_from_array(Array *array)
{
    return iterator_init(array_values(array), array_count(array));
}

Iterator *iterator_init_from_stack_destroy(Array *array)
{
    Iterator *iterator = iterator_init_from_array(array);
    array_destroy(array);

    return iterator;
}

void iterator_walk(Iterator *iterator, WalkCallback callback)
{
    for (int i = 0; i < iterator->count; i++) {
        callback(iterator->items[i]);
    }
}

void iterator_walk_index(Iterator *iterator, WalkIndexCallback callback)
{
    for (int i = 0; i < iterator->count; i++) {
        callback(iterator->items[i], i);
    }
}

Iterator *iterator_map(Iterator *iterator, MapCallback callback)
{
    Iterator *new = malloc(sizeof(Iterator));
    new->count = iterator->count;
    new->items = malloc(new->count * sizeof(void *));

    for (int i = 0; i < new->count; i++) {
        new->items[i] = callback(iterator->items[i]);
    }

    return new;
}

size_t iterator_count(Iterator *iterator)
{
    return (size_t) iterator->count;
}

void iterator_destroy(Iterator *iterator)
{
    free(iterator->items);
    free(iterator);
}

void *iterator_seek(Iterator *iterator, int offset)
{
    return iterator->items[offset];
}
