/*
 * MIT License
 *
 * Copyright (c) 2022 yunsik oh
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define JSMN_COPY_BUF_SIZE 32

/**
 * @brief get object with key in object
 * 
 * @param json json string
 * @param tok value token
 * @param key object key 
 * @return jsmntok_t* 
 */
JSMN_API jsmntok_t* jsmn_object_get(const char* json, jsmntok_t* tok, const char* key);

/**
 * @brief get object in array object
 *
 * @param json json string
 * @param tok value token
 * @param index index of array
 * @return jsmntok_t*
 */
JSMN_API jsmntok_t* jsmn_array_get(const char* json, jsmntok_t* tok, int index);


/**
 * @brief array object size get
 *
 * @param json json string
 * @param tok value token
 * @param index index of array
 * @return -1 if tok->type is not JSMN_ARRAY, otherwise size of array
 */
JSMN_API int jsmn_array_get_size(jsmntok_t* tok);

/**
 * @brief get value object to boolean
 * 
 * @param json json string
 * @param tok value token
 * @param value value out
 * @return int 0 if success, otherwise -1
 */
JSMN_API int jsmn_get_bool(const char* json, jsmntok_t* tok, bool* value);


/**
 * @brief get value object to integer
 * 
 * @param json json string
 * @param tok value token
 * @param value value out
 * @return int 0 if success, otherwise -1
 */
JSMN_API int jsmn_get_integer(const char* json, jsmntok_t* tok, int* value);


/**
 * @brief get value object to float
 * 
 * @param json json string
 * @param tok value token
 * @param value value out
 * @return int 0 if success, otherwise -1
 */
JSMN_API int jsmn_get_float(const char* json, jsmntok_t* tok, float* value);


/**
 * @brief get value object to string
 * 
 * @param json json string
 * @param tok value token
 * @param buf string buffer
 * @param buf_len string buffer size
 * @return int string length if success, otherwise -1
 */
JSMN_API int jsmn_get_string(const char* json, jsmntok_t* tok, char* buf, int buf_len);



/* helper function implementations */

#ifndef JSMN_HEADER

static inline const char* jsmntok_get_content(jsmntok_t* tok, const char* json)
{
    return json + tok->start;
}

static inline int jsmntok_get_content_len(jsmntok_t* tok)
{
    return tok->end - tok->start;
}

JSMN_API bool jsmntok_compare_string(const char* json, jsmntok_t* tok, const char* compare)
{
    const char* content = jsmntok_get_content(tok, json);
    int content_len = jsmntok_get_content_len(tok);

    if (tok->type == JSMN_STRING &&
        strncmp(content, compare, content_len) == 0) 
    {
        return true;
    }

    return false;
}

JSMN_API int jsmntok_copy_content(jsmntok_t* tok, const char* json, char* buf, int buf_len)
{
    const char* content = jsmntok_get_content(tok, json);
    int content_len = jsmntok_get_content_len(tok);
    if (content_len > buf_len)
        return -1;

    memcpy(buf, content, content_len);
    buf[content_len] = '\0';

    return content_len;
}

JSMN_API jsmntok_t* jsmntok_next(jsmntok_t* tok)
{
    jsmntok_t* next = tok + 1;
    for (int i = 0; i < tok->size; i ++)
        next = jsmntok_next(next);

    return next;
}

JSMN_API jsmntok_t* jsmntok_child(jsmntok_t* tok)
{
    if (tok->size)
        return tok + 1;
    return NULL;
}

JSMN_API jsmntok_t* jsmn_object_get(const char* json, jsmntok_t* tok, const char* key)
{
    if (!tok || tok->type != JSMN_OBJECT)
        return NULL;

    jsmntok_t* iter = jsmntok_child(tok);
    for (int i = 0; i < tok->size; i ++)
    {
        if (iter->size > 0 &&
            jsmntok_compare_string(json, iter, key))
        {
            return jsmntok_child(iter);
        }

        iter = jsmntok_next(iter);
    }

    return NULL;
}

JSMN_API jsmntok_t* jsmn_array_get(const char* json, jsmntok_t* tok, int index)
{
    if (!tok || tok->type != JSMN_ARRAY || index > tok->size)
        return NULL;

    jsmntok_t* obj = jsmntok_child(tok);
    for (int i = 0; i < index; i ++)
        obj = jsmntok_next(obj);

    return obj;
}

JSMN_API int jsmn_array_get_size(jsmntok_t* tok)
{
    if (tok->type == JSMN_ARRAY)
        return tok->size;

    return -1;
}

JSMN_API int jsmn_get_bool(const char* json, jsmntok_t* tok, bool* value)
{
    if (!tok || tok->type != JSMN_PRIMITIVE)
        return -1;

    if (jsmntok_compare_string(json, tok, "true"))
    {
        *value = true;
        return 0;
    }

    if (jsmntok_compare_string(json, tok, "false"))
    {
        *value = false;
        return 0;
    }

    return -1;
}

JSMN_API int jsmn_get_integer(const char* json, jsmntok_t* tok, int* value)
{
    if (!tok || tok->type != JSMN_PRIMITIVE)
        return -1;

    char buffer[JSMN_COPY_BUF_SIZE + 1];
    if (jsmntok_copy_content(tok, json, buffer, JSMN_COPY_BUF_SIZE) < 0)
        return -1;

    *value = atoi(buffer);
    return 0;
}

#if defined(JSMN_FLOAT)
JSMN_API int jsmn_get_float(const char* json, jsmntok_t* tok, float* value)
{
    if (!tok || tok->type != JSMN_PRIMITIVE)
        return -1;

    char buffer[JSMN_COPY_BUF_SIZE + 1];
    if (jsmntok_copy_content(tok, json, buffer, JSMN_COPY_BUF_SIZE) < 0)
        return -1;

    *value = atof(buffer);
    return 0;
}
#endif

JSMN_API int jsmn_get_string(const char* json, jsmntok_t* tok, char* buf, int buf_len)
{
    if (!tok || tok->type != JSMN_STRING || tok->size != 0)
        return -1;

    return jsmntok_copy_content(tok, json, buf, buf_len);
}

#endif // JSMN_HEADER

