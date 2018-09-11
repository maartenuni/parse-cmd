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

#ifndef STRING_UTILS_H
#define STRING_UTILS_H

/**
 * \file string_utils.h
 * \internal
 *
 * This file contains some utilities to work with a unified character type.
 * When a wide character is available, we can define a character type that
 * can deal with unicode and have support for functions like strtok 
 */

/**
 * \internal
 * \brief Counts the number of characters in a (multi) byte '\0' terminated
 *        string.
 *
 * This function can help with outlining multi byte characters. In contrast
 * to strlen, it will return a error when the string is NULL.
 *
 * \returns OPTION_OK when the number of characters in the string can be 
 *          OPTION_INVALID_ENCODING when the mb_str isn't encoded correctly.
 */
int
num_characters(const char* mb_str, size_t* character_count);


/**
 * Find a token in a string
 *
 * Iterate throughout a string to find a token.
 * 
 * \param [in] haystack  The string in which we are looking for a token
 * \param [in] needle    The token we are looking for.
 *
 * returns NULL if the token wasn't found
 */
const char*
find_token(const char* haystack, const char* needle);

#endif
