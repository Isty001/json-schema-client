#include <malloc.h>
#include "minunit.h"
#include "../src/stack.h"


MU_TEST(test_stack)
{
    Stack *stack = stack_init();

    mu_assert(NULL == stack_pop(stack), "Stack is not empty");
    mu_assert(NULL == stack_last(stack), "Stack is not empty");
    mu_assert(stack_is_empty(stack), "Stack is not empty");

    for (int i = 0; i < 10; i++) {
        stack_push(stack, (void *) i);
    }

    mu_assert(9 == (int) stack_pop(stack), "Last item is not 9");
    mu_assert(false == stack_is_empty(stack), "Stack is empty");
    mu_assert(8 == (int) stack_last(stack), "Last item is not 8");

    stack_destroy(stack);
}

static bool test_stack_filter(void *item)
{
    return (int)item == 1 ? true : false;
}

MU_TEST(test_unique_stack)
{
    Stack *stack = stack_init_filtered(test_stack_filter);

    stack_push(stack, (void *)1);
    stack_push(stack, (void *)2);
    stack_push(stack, (void *)1);

    mu_assert(2 == stack_count(stack), "Stack is not unique");

    stack_destroy(stack);
}

void run_stack_test(void)
{
    puts("STACK TEST");

    MU_RUN_TEST(test_stack);
    MU_RUN_TEST(test_unique_stack);

    MU_REPORT();
}

