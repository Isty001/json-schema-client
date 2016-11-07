#include "util.h"


char *request_data_load(char *path)
{
    if (path[0] == '@') {
        char *content = file_read(++path);
        free(--path);

        return content;
    }

    return path;
}
