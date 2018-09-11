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

#include <wchar.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include "parse_cmd.h"
#include "string_utils.h"

int num_characters(const char* mb_string, size_t* count)
{
    size_t cnt = 0;
    mbstate_t state = {0};
    const char* begin = mb_string;
    const char* end = mb_string + strlen(mb_string);
    size_t nbytes = end - begin;

    if (mb_string == NULL || count == NULL)
        return OPTION_INVALID_ARGUMENT;

    while(begin < end) {
        size_t width = mbrlen(begin, nbytes, &state);
        if (width == (size_t) - 1)
            return OPTION_INVALID_ENCODING;
        cnt++;
        nbytes-= width;
        begin += width;
    }
    assert(end == begin);
    *count = cnt;

    return OPTION_OK;
}

const char* 
find_token(const char* haystack, const char* needle)
{
    const char* start;
    const char* found   = NULL;
    size_t      nlen    = strlen(needle);

    for (start = haystack; *start != '\0'; start++) {
        if (strncmp(start, needle, nlen) == 0) {
            found = start;
            break;
        }
    }

    return found;
}

