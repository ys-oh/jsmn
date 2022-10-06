#include "jsmn.h"
#include <stdio.h>
#include <stdlib.h>

#define RED     "31"
#define GREEN   "32"
#define BLUE    "34"
#define RESET   "0"
#define COLOR_CODE(x) x
#define COLOR(color_code, str) "\033[" color_code "m" str "\033[0m"

static const char* json_list[] = {
"{"
  "\"op\": \"publish\","
  "\"topic\": \"/chatter\","
  "\"msg\": {"
      "\"data\": \"hi\""
  "}"
"}",

"["
  "\"str0\","
  "\"str1\","
  "{},"
  "[],"
  "10"
"]",

"{"
  "\"msg\": {"
      "\"data\": \"hello\""
  "}"
"}",
};


const char* jsmn_type_to_string(jsmntype_t type)
{
    switch (type)
    {
    case JSMN_OBJECT: return "object";
    case JSMN_PRIMITIVE: return "primitive";
    case JSMN_STRING: return "string";
    case JSMN_ARRAY: return "array";
    default:
        break;
    }

    return "unknown";
}

void print_token(const char* json, jsmntok_t* tok)
{
    printf("token(0x%lx) type = %s, child = %d", (uintptr_t)tok, jsmn_type_to_string(tok->type), tok->size);

    if (tok->type == JSMN_STRING || tok->type == JSMN_PRIMITIVE)
    {
        int len = (tok->end - tok->start);
        printf (", value = %.*s", len, json + tok->start);
    }

    printf("\n");
}

int json_helper_check(const char* json)
{
    int i;
    int r;
    jsmn_parser p;
    jsmntok_t t[128];

    jsmn_init(&p);
    r = jsmn_parse(&p, json, strlen(json), t,
                   sizeof(t) / sizeof(t[0]));
    if (r < 0)
    {
      printf("Failed to parse JSON: %d\n", r);
      return -1;
    }

    if ((*t).type == JSMN_OBJECT)
    {
        const char* keys[] = {"op", "topic", "msg", "invaild-key"};
        for (int i = 0; i < sizeof(keys) / sizeof(const char*);  i ++)
        {
            jsmntok_t* value = jsmn_object_get(json, t, keys[i]);
            if (value)
            {
                char value_string[128];

                printf ("key " COLOR(RED,"%s")
                        " has " COLOR(BLUE, "%s")
                        " type object and content value is " COLOR(GREEN, "%.*s") "\n",
                        keys[i], jsmn_type_to_string(value->type),
                        jsmntok_get_content_len(value),
                        jsmntok_get_content(value, json));

            }
            else
            {
                printf ("key " COLOR(RED,"%s")" is not found in object\n", keys[i]);
            }
        }
    }
    else if ((*t).type == JSMN_ARRAY)
    {
        for (int i = 0; i < jsmn_array_get_size(t); i ++)
        {
            jsmntok_t* value = jsmn_array_get(json, t, i);
            printf ("index " COLOR(RED,"%d")
                    " has " COLOR(BLUE, "%s")
                    " type object and content value is " COLOR(GREEN, "%.*s") "\n",
                    i, jsmn_type_to_string(value->type),
                    jsmntok_get_content_len(value),
                    jsmntok_get_content(value, json));
        }
    }
    else
    {
        printf("first token is not object or array\n");
        return -1;
    }

    return 0;
}


int main (int argc, char** argv)
{

    for (int i = 0; i < sizeof (json_list) / sizeof (const char*); i ++)
    {
        printf ("\r\njson list[%d] => %s\r\n\r\n", i, json_list[i]);

        json_helper_check(json_list[i]);

        printf ("\r\n\r\n");
    }

    return 0;
}
