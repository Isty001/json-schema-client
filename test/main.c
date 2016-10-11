void run_schema_test(void);
void run_iterator_test(void);
void run_href_test(void);
void run_stack_test(void);
void run_storage_test(void);
void run_util_tests(void);
void run_request_test(void);


typedef void * FIELD;


int main(void)
{
    run_schema_test();
    run_iterator_test();
    run_href_test();
    run_stack_test();
    run_storage_test();
    run_util_tests();
    run_request_test();
}
