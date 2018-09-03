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
 * \file text_buffer.h
 *
 * \internal
 * This file export a text_buffer that is a helper to build strings of
 * which the length is yet unknown.
 */

#include<stdlib.h>

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
    char* buffer;       ///< the '\0' delimited text buffer.
    size_t size;        ///< size of the text buffer.
    size_t capacity;    ///< capacity of the buffer.
} text_buffer_t;

/**
 * A typedef to a pointe to text_buffer_tr
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
 * @param buf   the buffer you want to append to
 * @param txt   the '\0' delimted string to append to the current buffer.
 *
 * \returns 0 when successful
 * \internal
 */
int
text_buffer_append(text_buffer_ptr buf, const char* txt);

/**
 * Append a character to the current buffer.
 *
 * Appends a new charcter to the buffer.
 * If the string inside the buffer would overflow the current buffer capacity 
 * the text_buffer will try to update it's capacity. The buf->buffer should
 * remain null terminated.
 *
 * @param buf   the buffer you want to append to
 * @param txt   a character to append to the text inside the buffer.
 *
 * \returns 0 when successful
 * \internal
 */
int
text_buffer_append_char(text_buffer_ptr buf, char c);

/**
 * Increase the capacity of the given buffer.
 * \internal
 */
int
text_buffer_new_capacity(text_buffer_ptr buf, size_t cap);

/**
 * Shrink the size of the buffer.
 *
 * The buffer shrinks with size. This function effectively only inserts
 * a null byte given the new size, it doesn't change its capacity.
 *
 * \internal
 */
int
text_buffer_shrink(text_buffer_ptr buf, size_t size);

/**
 * Minimalize the capacity of the buffer
 *
 * Calling this function on a text_buffer_t in
 * reduces the buffer in such a way that the buffer is a valid
 * C '\0' terminated string. The string should than only use precisely enough
 * bytes to match the string length plus the terminating null byte.
 *
 * \internal
 */
int
text_buffer_shrink_to_size(text_buffer_ptr buf);

