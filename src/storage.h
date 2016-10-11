#ifndef API_TEST_storage_H
#define API_TEST_storage_H


#include <stdbool.h>


void storage_init(char *dir);

char *storage_get(char *key);

void storage_set(char *key, char *value);

void storage_delete(char *key);

bool storage_empty(void);

void storage_dump(void);

void storage_clear(void);

void storage_destroy(void);


#endif
