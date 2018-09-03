/*
 * This file is part of lib parse cmd a utility that helps parsing arguments
 * from the command line. Copyright (C) 2018 Maarten Duijndam
 *
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 2.1 of the License, or
 * (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public
 * License for more details.
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation,
 * Inc. , 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA 
 */

/**
 * \file text_buffer.c
 */


#include <string.h>
#include <assert.h>
#include "text_buffer.h"

int
text_buffer_init(text_buffer_ptr buf, size_t desired_capacity)
{
    if (!buf)
        return -1;

    if (desired_capacity < 2)
        desired_capacity = 2;
    
    buf->buffer = calloc(desired_capacity, sizeof(char));
    buf->size = 0;
    buf->capacity = desired_capacity;

    return buf->buffer != 0 ? 0 : -2;
}

int
text_buffer_append(text_buffer_ptr buf, const char* txt)
{
    size_t length;
    int ret;

    if (!buf || !txt)
        return -1;

    length = strlen(txt);

    // Make sure that we have enough space in the buffer to store the old string
    // + the new + a terminating '\0'.
    if(buf->size + length + 1 > buf->capacity) {
        size_t new_cap = buf->size + length + 1;
        if (new_cap < buf->capacity * 2)
            new_cap = buf->capacity * 2;

        ret = text_buffer_new_capacity(buf, new_cap);
        if (ret)
            return ret;
    }

    strcat(buf->buffer + buf->size, txt);
    buf->size += length;
    return 0;
}

int
text_buffer_append_char(text_buffer_ptr buf, char c)
{
    char tempstr[2] = {c,'\0'};
    return text_buffer_append(buf, tempstr);
}

int
text_buffer_new_capacity(text_buffer_ptr buf, size_t cap)
{
    if (!buf)
        return -1;

    char* oldptr = buf->buffer;
    buf->buffer = realloc(buf->buffer, cap);

    if (!buf->buffer) {
        buf->buffer = oldptr;
        return -2;
    }
    buf->capacity = cap;
    return 0;
}

int
text_buffer_shrink(text_buffer_ptr buf, size_t size)
{
    if(!buf)
        return -1;
    size = size < buf->size ? size : buf->size;
    buf->size -= size;
    buf->buffer[buf->size] = '\0';
    return 0;
}

int
text_buffer_shrink_to_size(text_buffer_ptr buf)
{
    size_t desired_cap = buf->size + 1;
    if (buf->capacity > desired_cap) {
        char* old = buf->buffer;
        buf->buffer = realloc(buf->buffer, desired_cap);
        assert(buf->buffer[buf->size] == '\0');
        if (!buf->buffer) {
            buf->buffer = old;
            return -1;
        }
        buf->capacity = desired_cap;
    }
    return 0;
}
