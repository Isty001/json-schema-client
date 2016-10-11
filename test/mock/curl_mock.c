#include <string.h>
#include "../../src/util.h"
#include "../../src/request.h"


static char *content =
        "{"
        "    \"links\": ["
        "        {"
        "            \"rel\": \"list\","
        "            \"href\": \"/user/{id}\""
        "        },"
        "        {"
        "            \"rel\": \"show\","
        "            \"method\": \"GET\","
        "            \"description\": \"Desc\","
        "            \"title\": \"Title\","
        "            \"href\": \"/user/{birthDate}\""
        "        }"
        "    ]"
        "}\0";


char *curl_get(unused(char *url))
{
    return strdup(content);
}

char *curl_send_request(Request unused(*request))
{
    return "";
}
