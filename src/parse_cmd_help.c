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
 *
 * This file implements the documentation/help generating functions
 * of an option_context
 */

#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>

#include "parse_cmd.h"
#include "text_buffer.h"


int option_context_help(const option_context* options, char **help)
{
    int ret;                    // Variable for (un-)successful function call
    text_buffer_t buffer;       // Store the generated help is stored here
    char tempstr[BUFSIZ];
    char* short_help = NULL;    // The short help will be stored here.

    if (options == NULL || help == NULL || *help != NULL)
        return OPTION_INVALID_ARGUMENT;

    if (text_buffer_init(&buffer, 1024) != 0)
        return OPTION_OUT_OF_MEM;
    
    // obtain the short help to store inside of the current text buffer.
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
                   sprintf(tempstr, "    [-%c|--%.256s] %.1024s\n",
                           predef_opts[i].short_opt,
                           predef_opts[i].long_opt,
                           predef_opts[i].help
                           );
                   break;
               case OPT_INT:
                   sprintf(tempstr, "    [-%c|--%.256s <int>] %.1024s\n",
                           predef_opts[i].short_opt,
                           predef_opts[i].long_opt,
                           predef_opts[i].help
                           );
                   break;
               case OPT_FLOAT:
                   sprintf(tempstr, "    [-%c|--%.256s <float>] %.1024s\n",
                           predef_opts[i].short_opt,
                           predef_opts[i].long_opt,
                           predef_opts[i].help
                           );
                   break;
               case OPT_STR:
                   sprintf(tempstr, "    [-%c|--%.256s <string>] %.1024s\n",
                           predef_opts[i].short_opt,
                           predef_opts[i].long_opt,
                           predef_opts[i].help
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
                   sprintf(tempstr, "    [--%.256s] %.1024s\n",
                           predef_opts[i].long_opt,
                           predef_opts[i].help
                           );
                   break;
               case OPT_INT:
                   sprintf(tempstr, "    [--%.256s <int>] %.1024s\n",
                           predef_opts[i].long_opt,
                           predef_opts[i].help
                           );
                   break;
               case OPT_FLOAT:
                   sprintf(tempstr, "    [--%.256s <float>] %.1024s\n",
                           predef_opts[i].long_opt,
                           predef_opts[i].help
                           );
                   break;
               case OPT_STR:
                   sprintf(tempstr, "    [--%.256s <string>] %.1024s\n",
                           predef_opts[i].long_opt,
                           predef_opts[i].help
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
