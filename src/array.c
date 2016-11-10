#include <malloc.h>
#include "array.h"


typedef struct
{
    void *value;
} ArrayItem;

struct array
{
    ArrayItem **items;
    size_t count;
    size_t allocated;
};


static ArrayItem *create_item(void *ptr)
{
    ArrayItem *item = malloc(sizeof(ArrayItem));
    item->value = ptr;

    return item;
}

Array *array_init(void)
{
    Array *stack = malloc(sizeof(Array));
    stack->allocated = sizeof(ArrayItem);
    stack->items = malloc(stack->allocated);
    stack->count = 0;

    return stack;
}

void *array_pop(Array *array)
{
    if (!array_is_empty(array)) {
        ArrayItem *item = array->items[--array->count];
        void *ptr = item->value;

        free(item);
        decrease(array, ArrayItem *);

        return ptr;
    }
    return NULL;
}

void array_push(Array *array, void *ptr)
{
    increase(array, ArrayItem *);
    array->items[array->count++] = create_item(ptr);
}

bool array_is_empty(Array *array)
{
    return 0 == array->count;
}

void *array_last(Array *array)
{
    if (!array_is_empty(array)) {
        return array->items[array->count - 1]->value;
    }
    return NULL;
}

void **array_values(Array *array)
{
    void **pointers = malloc(array->count * sizeof(void *));

    for (size_t i = 0; i < array->count; i++) {
        pointers[i] = array->items[i]->value;
    }
    return pointers;
}

static void resort_items(Array *array, size_t from)
{
    if (array_is_empty(array)) {
        return;
    }
    for (size_t i = from; i < array->count; i++) {
        array->items[i] = array->items[i + 1];
    }
}

void array_filter(Array *array, ArrayFilter filter)
{
    for (size_t i = 0; i < array->count; i++) {
        if (!filter(array->items[i]->value)) {
            free(array->items[i]);
            array->count--;
            resort_items(array, i);
            decrease(array, ArrayItem *);
        }
    }
}

size_t array_count(Array *stack)
{
    return stack->count;
}

static void free_array(Array *stack)
{
    free(stack->items);
    free(stack);
}

void array_destroy(Array *stack)
{
    for (size_t i = 0; i < stack->count; i++) {
        free(stack->items[i]);
    }
    free_array(stack);
}

void array_destroy_callback(Array *stack, Free callback)
{
    ArrayItem *item;

    for (size_t i = 0; i < stack->count; i++) {
        item = stack->items[i];
        callback(item->value);
        free(item);
    }
    free_array(stack);
}
