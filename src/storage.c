#include <malloc.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <errno.h>
#include "util.h"
#include "storage.h"


#define matches(item, key) 0 == strcmp(item->key, key)

#define trim(str) strtok(str, "\n")


typedef struct
{
    char *key;
    char *value;
} StorageItem;


typedef struct
{
    char *file;
    char *dir;
    int count;
    StorageItem **items;
    size_t allocated;
} Storage;


Storage *storage;


static void check_dir(char *dir)
{
    struct stat stats;

    if (-1 == stat(dir, &stats)) {
        mkdir(dir, 0700);
    }
}

static void add_saved_items(FILE *file)
{
    size_t len;
    ssize_t read = 0;
    char *key = NULL, *value = NULL;

    while (-1 != read) {
        getline(&key, &len, file);
        read = getline(&value, &len, file);

        storage_set(trim(key), trim(value));
    }
    free_multi(2, key, value);
}

static void load_from_file(void)
{
    FILE *file = fopen(storage->file, "r");

    if (!file) {
        return;
    }
    add_saved_items(file);

    fclose(file);
}

void storage_init(char *dir)
{
    check_dir(dir);
    storage = malloc(sizeof(Storage));
    storage->count = 0;
    storage->allocated = sizeof(StorageItem);
    storage->items = malloc(storage->allocated);
    storage->dir = strdup(dir);
    asprintf(&storage->file, "%s/"STORAGE_FILE, dir);

    load_from_file();
}

static void destroy_item(StorageItem *item)
{
    free_multi(3, item->key, item->value, item);
}

void storage_destroy(void)
{
    for (int i = 0; i < storage->count; i++) {
        destroy_item(storage->items[i]);
    }
    free_multi(4, storage->items, storage->file, storage->dir, storage);
}

void storage_clear(void)
{
    char *dir = strdup(storage->dir);

    unlink(storage->file);
    storage_destroy();
    storage_init(dir);
    free(dir);
}

static StorageItem *find_item(char *key)
{
    for (int i = 0; i < storage->count; i++) {
        if (matches(storage->items[i], key)) {
            return storage->items[i];
        }
    }

    return NULL;
}

char *storage_get(char *key)
{
    StorageItem *found = find_item(key);

    if (found) {
        return found->value;
    }

    return NULL;
}

static void add_new_item(char *key, char *value)
{
    StorageItem *new = malloc(sizeof(StorageItem));

    new->key = strdup(key);
    new->value = strdup(value);

    storage->items[storage->count++] = new;

    increase(storage, StorageItem *);
}

void storage_set(char *key, char *value)
{
    StorageItem *found = find_item(key);

    if (found) {
        free(found->value);
        found->value = strdup(value);

        return;
    }
    add_new_item(key, value);
}

void resort_items(int from)
{
    if (storage_empty()) {
        return;
    }
    for (int i = from; i < storage->count; i++) {
        storage->items[i] = storage->items[i + 1];
    }
}

void storage_delete(char *key)
{
    for (int i = 0; i < storage->count; i++) {
        if (matches(storage->items[i], key)) {
            destroy_item(storage->items[i]);
            storage->count--;
            resort_items(i);
            decrease(storage, StorageItem *);
        }
    }
}

bool storage_empty(void)
{
    return 0 == storage->count;
}

static void persist_items(FILE *file)
{
    StorageItem *item;

    for (int i = 0; i < storage->count; i++) {
        item = storage->items[i];
        fprintf(file, "%s\n%s\n", item->key, item->value);
    }
}

void storage_dump(void)
{
    FILE *file = fopen(storage->file, "wb+");

    if (NULL == file) {
        die("Failed to open Storage file \"%s\"", storage->file);
    }

    persist_items(file);
    fclose(file);
}
