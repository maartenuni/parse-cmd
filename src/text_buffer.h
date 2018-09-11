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

#ifndef TEXT_BUFFER_H
#define TEXT_BUFFER_H

#include "parse_cmd_config.h"
#include<stdlib.h>

#if defined (HAVE_WCHAR_H)
#include<wchar.h>
#endif

/**
 * \file text_buffer.h
 *
 * \internal
 * This file export a text_buffer that is a helper to build strings of
 * which the length is yet unknown.
 */

/**
 * A text buffer is used to build some strings.
 *
 * TextBuffer are typically created on the stack. Then the user should
 * call text_buffer_init() on them to initialize the variable.
 * They have a buffer that can be used as a return value for a function that
 * builds a string as an output value. The user/caller is then responsible
 * to free the buffer.
 *
 * \internal
 */
typedef struct text_buffer {
    char*   buffer;      ///< the '\0' delimited text buffer.
    size_t  size;        ///< size of the text buffer.
    size_t  capacity;    ///< capacity of the buffer.
} text_buffer_t;

/**
 * A typedef to a pointer to text_buffer_t
 * \internal
 */
typedef text_buffer_t* text_buffer_ptr;


/**
 * Initialize a text buffer .
 *
 * Text buffer init initializes a text_buffer_t instance. The struct
 * is initialized in such way that a members have a valid value.
 *
 * @param buf               the buffer you want to initialize
 * @param desired_capacity  the desired capacity of the buffer buf->buffer.
 *
 * \returns 0 when successful
 * \internal
 */
int
text_buffer_init(text_buffer_ptr buf, size_t desired_capacity);

/**
 * Append a new '\0' delimited string to the buffer
 *
 * Appends a new string. If the string would overflow the current buffer size
 * the text_buffer will try to update it's capacity. 
 *
 * @param [in, out] buf the buffer you want to append to
 * @param [in]      txt the '\0' delimited string to append to the current buffer.
 *
 * \returns 0 when successful
 * \internal
 */
int
text_buffer_append(text_buffer_ptr buf, const char* txt);

/**
 * Appends size bytes of a string to the buffer
 *
 * A number of bytes(size) will be written from the txt string to the buffer
 * if the txt string contains terminating '\0' bytes they will be added
 * to the buffer.
 *
 * @param [in,out] buf  A text buffer to write the string to
 * @param [in]     txt  A string to write to the buffer
 * @param [in]     size Size number of bytes (including '\0' bytes
 *                      will be written tot the output.
 */
int
text_buffer_append_size(text_buffer_ptr buf, const char* txt, size_t size);

/**
 * \internal
 * \brief Append a character to the current buffer.
 *
 * Appends a new charcter to the buffer.
 * If the string inside the buffer would overflow the current buffer capacity 
 * the text_buffer will try to update it's capacity. The buf->buffer should
 * remain null terminated.
 *
 * @param [in, out] buf   the buffer you want to append to
 * @param [in]      c     a character to append to the text inside the buffer.
 *
 * \returns 0 when successful
 */
int
text_buffer_append_char(text_buffer_ptr buf, char c);

/**
 * \internal
 * \brief Increase the capacity of the given buffer.
 */
int
text_buffer_new_capacity(text_buffer_ptr buf, size_t cap);

/**
 * \internal
 *
 * \brief Shrink the size of the buffer.
 *
 * The buffer shrinks with size. This function effectively only inserts
 * a null byte given the new size, it doesn't change its capacity.
 *
 * @param [in, out] buf the buffer to shrink.
 * @param [in]      size the number of bytes to shrink the buffer. Shrinking
 *                  a buffer with its current size is effectively the same
 *                  as clearing the buffer.
 */
int
text_buffer_shrink(text_buffer_ptr buf, size_t size);

/**
 * \internal
 *
 * \brief Clears the text_buffer setting the new size to 0.
 *
 * After calling this function the buf->buffer contains a valid empty
 * string. The size of the string is 0, however the capacity remains
 * unaffected.
 */
void
text_buffer_clear(text_buffer_ptr buf);

/**
 * \internal
 * \brief Minimalize the capacity of the buffer
 *
 * Calling this function on a text_buffer_t in
 * reduces the buffer in such a way that the buffer is a valid
 * C '\0' terminated string. The string should than only use precisely enough
 * bytes to match the string length plus the terminating null byte.
 */
int
text_buffer_shrink_to_size(text_buffer_ptr buf);

#endif
