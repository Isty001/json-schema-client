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

Iterator *iterator_init_from_stack(Stack *stack)
{
    return iterator_init(stack_items(stack), stack_count(stack));
}

Iterator *iterator_init_from_stack_destroy(Stack *stack)
{
    Iterator *iterator = iterator_init_from_stack(stack);
    stack_destroy(stack);

    return iterator;
}

void iterator_walk(Iterator *iterator, IteratorWalkCallback callback)
{
    for (int i = 0; i < iterator->count; i++) {
        callback(iterator->items[i]);
    }
}

Iterator *iterator_map(Iterator *iterator, IteratorMapCallback callback)
{
    Iterator *new = malloc(sizeof(Iterator));
    new->count = iterator->count;
    new->items = malloc(new->count * sizeof(void *));

    for (int i = 0; i < new->count; i++) {
        new->items[i] = callback(iterator->items[i]);
    }
    iterator_destroy(iterator);

    return new;
}

size_t iterator_count(Iterator *iterator)
{
    return iterator->count;
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
