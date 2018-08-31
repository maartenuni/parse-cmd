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
 * \file parse_cmd_help.c
 */

#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>

#include "parse_cmd.h"

/**
 * A text buffer is used to build some strings.
 */
typedef struct text_buffer {
    char* buffer;       ///< the '\0' delimited text buffer.
    size_t size;        ///< size of the text buffer.
    size_t capacity;    ///< capacity of the buffer.
} text_buffer_t;

/** A typedef to a pointe to text_buffer_t */
typedef text_buffer_t* text_buffer_ptr;

static int text_buffer_new_capacity(text_buffer_ptr buf, size_t cap);

static int
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

static int
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

static int
text_buffer_append_char(text_buffer_ptr buf, char c)
{
    char tempstr[2] = {c,'\0'};
    return text_buffer_append(buf, tempstr);
}

static int
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

static int
text_buffer_shrink(text_buffer_ptr buf, size_t size)
{
    if(!buf)
        return -1;
    size = size < buf->size ? size : buf->size;
    buf->size -= size;
    buf->buffer[buf->size] = '\0';
}

static int
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

int option_context_help(const option_context* options, char **help)
{
    int ret;
    text_buffer_t buffer;

    if (options == NULL || help == NULL)
        return OPTION_INVALID_ARGUMENT;
    if (*help != NULL)
        return OPTION_INVALID_ARGUMENT;

    if (text_buffer_init(&buffer, 1024) != 0)
        return OPTION_OUT_OF_MEM;

    char tempstr[BUFSIZ];
    char* short_help = NULL;

    ret = option_context_short_help(options, &short_help);
    if (ret) {
        return OPTION_OUT_OF_MEM;
    }
    ret = text_buffer_append(&buffer, short_help);
    if (ret)
        return OPTION_OUT_OF_MEM;
    free(short_help);
    short_help = NULL;

    ret = text_buffer_append(&buffer, "\n\ndescription:\n");
    if (ret) {
        free(buffer.buffer);
        return OPTION_OUT_OF_MEM;
    }

    const char* description = option_context_get_description(options);
    if (description) {
        ret = text_buffer_append(&buffer, description);
        if (ret) {
            free(buffer.buffer);
            return OPTION_OUT_OF_MEM;
        }
    }

    ret = text_buffer_append(&buffer, "\n\n");
    if (ret) {
        free(buffer.buffer);
        return OPTION_OUT_OF_MEM;
    }

    const cmd_option* predef_opts = option_context_get_predef_options(options);

    for (size_t i = 0; i < option_context_num_predef_options(options); i++) {
        if (predef_opts[i].short_opt != '\0') {
           switch(predef_opts[i].option_type) {
               case OPT_FLAG:
                   sprintf(tempstr, "[-%c|--%.256s] ",
                           predef_opts[i].short_opt,
                           predef_opts[i].long_opt
                           );
                   break;
               case OPT_INT:
                   sprintf(tempstr, "[-%c|--%.256s <int>] ",
                           predef_opts[i].short_opt,
                           predef_opts[i].long_opt
                           );
                   break;
               case OPT_FLOAT:
                   sprintf(tempstr, "[-%c|--%.256s <float>] ",
                           predef_opts[i].short_opt,
                           predef_opts[i].long_opt
                           );
                   break;
               case OPT_STR:
                   sprintf(tempstr, "[-%c|--%.256s <string>] ",
                           predef_opts[i].short_opt,
                           predef_opts[i].long_opt
                           );
                   break;
               default:
                   tempstr[0] = '\0'; // empty tempstring
                   assert(0==1); //unimplemented option type
           }
        }
        else {
           switch(predef_opts[i].option_type) {
               case OPT_FLAG:
                   sprintf(tempstr, "[--%.256s] ",
                           predef_opts[i].long_opt
                           );
                   break;
               case OPT_INT:
                   sprintf(tempstr, "[--%.256s <int>] ",
                           predef_opts[i].long_opt
                           );
                   break;
               case OPT_FLOAT:
                   sprintf(tempstr, "[--%.256s <float>] ",
                           predef_opts[i].long_opt
                           );
                   break;
               case OPT_STR:
                   sprintf(tempstr, "[--%.256s <string>] ",
                           predef_opts[i].long_opt
                           );
                   break;
               default:
                   tempstr[0] = '\0'; // empty temp string
                   assert(0==1); //unimplemented option type
           }
        }
        text_buffer_append(&buffer, tempstr);
    }
    
    text_buffer_shrink_to_size(&buffer);
    *help = buffer.buffer;
    return OPTION_OK;
}

int option_context_short_help(const option_context* options, char **help)
{
    text_buffer_t buffer;

    if (options == NULL || help == NULL)
        return OPTION_INVALID_ARGUMENT;
    if (*help != NULL)
        return OPTION_INVALID_ARGUMENT;

    if (text_buffer_init(&buffer, 1024) != 0)
        return OPTION_OUT_OF_MEM;

    const char* header_fmt = "Usage %.100s: ";
    char tempstr[BUFSIZ];
    sprintf(tempstr, header_fmt, option_context_prog_name(options));

    if (text_buffer_append(&buffer, tempstr) != 0) {
        free(buffer.buffer);
        return OPTION_OUT_OF_MEM;
    }

    const cmd_option* predef_opts = option_context_get_predef_options(options);

    for (size_t i = 0; i < option_context_num_predef_options(options); i++) {
        if (predef_opts[i].short_opt != '\0') {
           switch(predef_opts[i].option_type) {
               case OPT_FLAG:
                   sprintf(tempstr, "[-%c|--%.256s] ",
                           predef_opts[i].short_opt,
                           predef_opts[i].long_opt
                           );
                   break;
               case OPT_INT:
                   sprintf(tempstr, "[-%c|--%.256s <int>] ",
                           predef_opts[i].short_opt,
                           predef_opts[i].long_opt
                           );
                   break;
               case OPT_FLOAT:
                   sprintf(tempstr, "[-%c|--%.256s <float>] ",
                           predef_opts[i].short_opt,
                           predef_opts[i].long_opt
                           );
                   break;
               case OPT_STR:
                   sprintf(tempstr, "[-%c|--%.256s <string>] ",
                           predef_opts[i].short_opt,
                           predef_opts[i].long_opt
                           );
                   break;
               default:
                   tempstr[0] = '\0'; // empty tempstring
                   assert(0==1); //un inplemented option type
           }
        }
        else {
           switch(predef_opts[i].option_type) {
               case OPT_FLAG:
                   sprintf(tempstr, "[--%.256s] ",
                           predef_opts[i].long_opt
                           );
                   break;
               case OPT_INT:
                   sprintf(tempstr, "[--%.256s <int>] ",
                           predef_opts[i].long_opt
                           );
                   break;
               case OPT_FLOAT:
                   sprintf(tempstr, "[--%.256s <float>] ",
                           predef_opts[i].long_opt
                           );
                   break;
               case OPT_STR:
                   sprintf(tempstr, "[--%.256s <string>] ",
                           predef_opts[i].long_opt
                           );
                   break;
               default:
                   tempstr[0] = '\0'; // empty tempstring
                   assert(0==1); //un inplemented option type
           }
        }
        text_buffer_append(&buffer, tempstr);
    }

    if (option_context_num_predef_options(options) > 0) {
        // eat last printed space.
        text_buffer_shrink(&buffer, 1);
    }

    text_buffer_shrink_to_size(&buffer);
    *help = buffer.buffer;
    return OPTION_OK;
}
