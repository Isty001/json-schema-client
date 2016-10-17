#include <stdio.h>
#include <string.h>
#include <pcre.h>
#include <ctype.h>
#include "href.h"


static const char *pattern = "{(.*?)}";


static void extract_matches(const char *subject, int exec_result, int *ovector, Stack *found)
{
    const char **match_list;

    if (pcre_get_substring_list(subject, ovector, exec_result, &match_list) >= 0) {
        stack_push(found, match_list);
    }
}

Iterator *href_arguments(char *href)
{
    const char *error;
    Stack *found = stack_init();
    int error_code, exec, ovector[100], offset = 0, length = strlen(href);

    pcre *pcre = pcre_compile(pattern, PCRE_MULTILINE, &error, &error_code, 0);

    while (offset < length && (exec = pcre_exec(pcre, 0, href, length, offset, 0, ovector, 100)) >= 0) {
        extract_matches(href, exec, ovector, found);
        offset = ovector[1];
    }

    pcre_free(pcre);

    return iterator_init_from_stack_destroy(found);
}

void href_replace(char **url, char *placeholder, char *value)
{
    if (is_string_empty(value)) {
        return;
    }

    char buffer[4096];
    char *p = strstr(*url, placeholder);

    strncpy(buffer, *url, p - *url);
    buffer[p - *url] = '\0';

    sprintf(buffer + (p - *url), "%s%s", value, p + strlen(placeholder));

    free(*url);
    *url = strdup(buffer);
}
