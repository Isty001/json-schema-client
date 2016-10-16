#ifndef API_TEST_STACK_H
#define API_TEST_STACK_H


#include <stdbool.h>
#include "util.h"


typedef struct stack Stack;

typedef bool (*StackFilter)(void *ptr);


Stack *stack_init(void);

Stack *stack_init_filtered(StackFilter filter);

void *stack_pop(Stack *stack);

void stack_push(Stack *stack, void *ptr);

bool stack_is_empty(Stack *stack);

void *stack_last(Stack *stack);

void **stack_to_array(Stack *stack);

size_t stack_count(Stack *stack);

void stack_destroy(Stack *stack);

void stack_destroy_callback(Stack *stack, Free callback);

#endif
