#include <malloc.h>
#include <stdlib.h>
#include "stack.h"
#include "util.h"


typedef struct item
{
    void *ptr;
} StackItem;

struct stack
{
    StackItem **items;
    int count;
    size_t allocated;
    StackFilter filter;
};


static StackItem *create_item(void *ptr)
{
    StackItem *item = malloc(sizeof(StackItem));
    item->ptr = ptr;

    return item;
}

Stack *stack_init(void)
{
    Stack *stack = malloc(sizeof(Stack));
    stack->allocated = sizeof(StackItem);
    stack->items = malloc(stack->allocated);
    stack->count = 0;
    stack->filter = NULL;

    return stack;
}

Stack *stack_init_filtered(StackFilter filter)
{
    Stack *stack = stack_init();
    stack->filter = filter;

    return stack;
}

void *stack_pop(Stack *stack)
{
    if (!stack_is_empty(stack)) {
        StackItem *item = stack->items[--stack->count];
        void *ptr = item->ptr;

        free(item);
        decrease(stack, StackItem *);

        return ptr;
    }
    return NULL;
}

static void push_item(Stack *stack, void *ptr)
{
    increase(stack, StackItem *);
    stack->items[stack->count++] = create_item(ptr);
}

static void push_item_filtered(Stack *stack, void *ptr)
{
    if (stack->filter(ptr)) {
        push_item(stack, ptr);
    }
}

void stack_push(Stack *stack, void *ptr)
{
    if (NULL == stack->filter) {
        push_item(stack, ptr);
    } else {
        push_item_filtered(stack, ptr);
    }
}

bool stack_is_empty(Stack *stack)
{
    return 0 == stack->count;
}

void *stack_last(Stack *stack)
{
    if (!stack_is_empty(stack)) {
        return stack->items[stack->count - 1]->ptr;
    }
    return NULL;
}

void **stack_items(Stack *stack)
{
    void **pointers = malloc(stack->count * sizeof(void *));

    for (int i = 0; i < stack->count; i++) {
        pointers[i] = stack->items[i]->ptr;
    }
    return pointers;
}

size_t stack_count(Stack *stack)
{
    return stack->count;
}

void stack_destroy(Stack *stack)
{
    stack_destroy_callback(stack, free);
}

void stack_destroy_callback(Stack *stack, Free callback)
{
    for (int i = 0; i < stack->count; i++) {
        callback(stack->items[i]);
    }

    free(stack->items);
    free(stack);
}
