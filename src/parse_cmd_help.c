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
#include "terminal_utils.h"

/**
 * Append the short documentation of one option to the current text buffer.
 *
 * \Internal
 * The short option is added to the short doc. It tries to put the doc
 * of one option on one line. If there are more options than fit on one line
 * the new options are added to the new line. The new line will start with
 * 8 spaces. The documentation regarding the new option will be appended
 * after the 8 spaces. No new lines are are added after an option.
 *
 * \param buf[in,out]       The textbuffer to wich we append the new option
 * \param opt[in]           The option we are currently documenting
 * \param maxwidth[in]      The maximum line length.
 * \param llenght[in,out]   The length of the current line before and after
 *                          this function call. 
 */
static int
format_short_opt(
        text_buffer_ptr buf,
        cmd_option opt,
        size_t maxwidth,
        size_t* llength
        )
{
    int ret;
    char tempstr [512] = "";
    int append_length  = 0; 

    switch(opt.option_type) {
        case OPT_FLAG:
            if (opt.short_opt)
                sprintf(tempstr, "[-%c|--%.256s] ", opt.short_opt, opt.long_opt);
            else
                sprintf(tempstr, "[--%.256s] ",opt.long_opt);

            break;
        case OPT_INT:
            if (opt.short_opt)
                sprintf(tempstr, "[-%c|--%.256s <int>] ",
                        opt.short_opt,
                        opt.long_opt
                        );
            else
                sprintf(tempstr, "[--%.256s <int>] ",opt.long_opt);
            break;
        case OPT_FLOAT:
            if (opt.short_opt)
                sprintf(tempstr, "[-%c|--%.256s <float>] ",
                        opt.short_opt,
                        opt.long_opt
                        );
            else
                sprintf(tempstr, "[--%.256s <float>] ", opt.long_opt);
            break;
        case OPT_STR:
            if (opt.short_opt)
                sprintf(tempstr, "[-%c|--%.256s <string>] ",
                        opt.short_opt,
                        opt.long_opt
                        );
            else
                sprintf(tempstr, "[--%.256s <string>] ", opt.long_opt);
            break;
        default:
            tempstr[0] = '\0'; // empty tempstring
            assert(0==1); //un inplemented option type
    }

    append_length = strlen(tempstr);

    // Handle the case that the new option doc would overflow the maximum
    // line length.
    if (*llength + append_length >= maxwidth) {
        ret = text_buffer_append(buf, "\n        ");
        if (ret)
            return ret;
        *llength = 8;
    }

    ret = text_buffer_append(buf, tempstr);
    *llength += append_length;
    return ret;
}

int option_context_short_help(const option_context* options, char **help)
{
    text_buffer_t buffer;
    size_t linelength       = 0;
    const size_t maxlength  = get_terminal_width();

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
    linelength += strlen(tempstr);

    const cmd_option* predef_opts = option_context_get_predef_options(options);

    for (size_t i = 0; i < option_context_num_predef_options(options); i++) {
        format_short_opt(&buffer, predef_opts[i], maxlength, &linelength);
    }

    if (option_context_num_predef_options(options) > 0) {
        // eat last printed space.
        text_buffer_shrink(&buffer, 1);
    }

    text_buffer_shrink_to_size(&buffer);
    *help = buffer.buffer;
    return OPTION_OK;
}

int option_context_help(const option_context* options, char **help)
{
    int ret;                    // Variable for (un-)successful function call
    text_buffer_t buffer;       // Store the generated help is stored here
    char tempstr[BUFSIZ];
    char* short_help = NULL;    // The short help will be stored here.
    const int term_width =  get_terminal_width();
    int line_length = 0;

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
           const char* fmt       = "";
           const char* fmt_flag  = "    [-%c|--%.256s]\n  %.1024s\n";
           const char* fmt_int   = "    [-%c|--%.256s <int>]\n  %.1024s\n";
           const char* fmt_float = "    [-%c|--%.256s <float>]\n  %.1024s\n";
           const char* fmt_str   = "    [-%c|--%.256s <string>]\n  %.1024s\n";
           switch(predef_opts[i].option_type) {
               case OPT_FLAG:
                   fmt = fmt_flag;
                   break;
               case OPT_INT:
                   fmt = fmt_int;
                   break;
               case OPT_FLOAT:
                   fmt = fmt_float;
                   break;
               case OPT_STR:
                   fmt = fmt_str;
                   break;
               default:
                   tempstr[0] = '\0';   // empty tempstring
                   assert(0==1);        // unimplemented option type
           }
           sprintf(tempstr, fmt,
                   predef_opts[i].short_opt,
                   predef_opts[i].long_opt,
                   predef_opts[i].help != NULL ? predef_opts[i].help : ""
                   );

        }
        else {
           const char* fmt       = "";
           const char* fmt_flag  = "    [--%.256s]\n  %.1024s\n";
           const char* fmt_int   = "    [--%.256s <int>]\n  %.1024s\n";
           const char* fmt_float = "    [--%.256s <float>]\n  %.1024s\n";
           const char* fmt_str   = "    [--%.256s <string>]\n  %.1024s\n";
           switch(predef_opts[i].option_type) {
               case OPT_FLAG:
                   fmt = fmt_flag;
                   break;
               case OPT_INT:
                   fmt = fmt_int;
                   break;
               case OPT_FLOAT:
                   fmt = fmt_float;
                   break;
               case OPT_STR:
                   fmt = fmt_str;
                   break;
               default:
                   tempstr[0] = '\0'; // empty temp string
                   assert(0==1); //unimplemented option type
           }
           sprintf(tempstr, fmt,
                   predef_opts[i].long_opt,
                   predef_opts[i].help != NULL ? predef_opts[i].help : ""
                   );
        }
        text_buffer_append(&buffer, tempstr);
    }
    
    text_buffer_shrink_to_size(&buffer);
    *help = buffer.buffer;
    return OPTION_OK;
}

