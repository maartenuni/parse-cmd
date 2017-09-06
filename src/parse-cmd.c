/*
 * This file is part of lib parse cmd a utility that helps parsing arguments
 * from the command line. Copyright (C) 2017 Maarten Duijndam
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
 * \file parse-cmd.c
 */

#if defined(_MSC_VER)

// disable some VisualStudio warnings in order to make
// the important ones stand out.

/*Try to make visual studio stop nagging about POSIX functions.*/
#define _CRT_NONSTDC_NO_DEPRECATE
/*although sscanf can be unsafe when used well it is alright*/
#define _CRT_SECURE_NO_WARNINGS

#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "parse-cmd.h"

/**
 * Stores the encountered options and arguments.
 * \internal
 */
struct option_context {
    const char*     program_name;       ///< Stores the name of the program.
    cmd_option**    options;            ///< The options specified.
    const char**    args;               ///< The arguments specified.
    int             n_options;          ///< Number of options specified.
    int             options_capacity;   ///< Capacity of options storage.
    int             n_args;             ///< Number of arguments specified.
    int             arguments_capacity; ///< Capacity of the arguments.
};


void option_context_free(option_context* context)
{
    free(context->options);
    free(context->args);
    free(context);
}

/*
 * All options but flags take a value.
 */
static int option_takes_value(cmd_option* option)
{
    return option->option_type != OPT_FLAG ? 1 : 0;
}

/*
 * An option is a short option when it starts with a dash ('-').
 * multiple short options can be specified after the dash.
 */
static int is_short_opt(const char* opt)
{
    int len = strlen(opt);
    if (len < 2)
        return 0;
    return (opt[0] == '-' && opt[1] != '-') ? 1 : 0;
}

/*
 * Options are long opts when the start wit two dashes "--", only
 * one options can than be specified.
 */
static int is_long_opt(const char* opt)
{
    const char* prefix = "--";
    return strncmp(prefix, opt, strlen(prefix)) == 0;
}

/*
 * Adds the name off the program to the  to the option context.
 */
static int options_add_name(option_context* options, const char* name)
{
    assert(options && name);
    if (!options || !name)
        return OPTION_PARSE_ERROR;

    options->program_name = name;
    return OPTION_OK;
}

/*
 * Utility to find an equals sign in a string.
 */
static int find_equals(const char* str)
{
    const char* p = str;
    int ret = -1;
    while (*p != '\0') {
        if (*p == '=') {
            ret = (int)(p - str);
            break;
        }
        p++;
    }
    return ret;
}

static int long_opt_contains_value(const char* str)
{
    int pos = find_equals(str);
    return pos >= 0 ? 1 : 0;
}

/*
 * Looks between options whether opt is specified
 *
 * Note make sure not to include the leading "--" of an option.
 *
 * @ return a value >= 0 when it is found and -1 when it isn't found.
 */
static int find_long_option(const char* opt,
                            cmd_option* opts,
                            unsigned    nopts)
{
    char buffer[BUFSIZ];
    unsigned i = 0;
    int ret = -1;

    // capture piece of string without =
    for (i = 0; opt[i] != '\0' && i < BUFSIZ - 1; ++i) {
        if (opt[i] == '=')
            break;
        buffer[i] = opt[i];
    }
    buffer[i] = '\0';

    for (i = 0; i < nopts; ++i) {
        if (strcmp(buffer, opts[i].long_opt) == 0) {
            ret = (int)i;
            break;
        }
    }
    return ret;
}

/*
 * Looks between options whether opt is specified
 *
 * @ return a value >= 0 when it is found and -1 when it isn't found.
 */
static int find_short_option(char opt,
                             cmd_option* opts,
                             unsigned nopts)
{
    unsigned i = 0;
    int ret = -1;
    for (i = 0; i < nopts; ++i) {
        if (opt == opts[i].short_opt) {
            ret = (int)i;
            break;
        }
    }
    return ret;
}

/**
 * \internal
 * When an option is specified to the program, it makes itself
 * know to the option_context with this function.
 *
 * \param[in,out] options the option structure to and argument to.
 * \param[in]     argument the argument to store.
 *
 * returns OPTION_OK when successful.
 */
static int options_add_parsed_argument(
        option_context* options,
        const char*     argument
        )
{
    int ret = OPTION_OK;
    /* Increase array size. */
    if (options->n_args + 1 >= options->arguments_capacity) {
        options->arguments_capacity *= 2;
        if (options->arguments_capacity == 0) {
            // can only occur when completely empty
            assert(options->n_args == 0);
            options->arguments_capacity = 1;
            options->args = malloc(sizeof(const char*));
            if (!options->args)
                return OPTION_OUT_OF_MEM;
        }
        else {
            options->args = realloc(
                    options->args,
                    options->arguments_capacity* sizeof(const char*)
                    );
            if (!options->args)
                return OPTION_OUT_OF_MEM;
        }
    }
    options->args[options->n_args++] = argument;
    return ret;
}

/**
 * \internal
 * When an option is specified to the program, it makes itself
 * know to the option_context with this function.
 *
 * \param [in, out] options the option context to add this option to.
 * \param [in]      option the option to add.
 * \param [in]      The string value of its options.
 */
static int
options_add_parsed_option(option_context* options,
                          cmd_option* option,
                          const char* value
                          )
{
    int ret;
    /* Increase array size. */
    if (options->n_options + 1 >= options->options_capacity) {
        options->options_capacity *= 2;
        if (options->options_capacity == 0) {
            // can only occur when completely empty
            assert(options->n_options == 0);
            options->options_capacity = 1;
            options->options = malloc(sizeof(cmd_option*));
            if (!options->options)
                return OPTION_OUT_OF_MEM;
        }
        else {
            options->options = realloc(
                options->options,
                options->options_capacity * sizeof(cmd_option*)
                );
            if (!options->options)
                return OPTION_OUT_OF_MEM;
        }
    }
    options->options[options->n_options++] = option;
    int intval;
    double floatval;

    // Check whether a value is required and specified.
    if (option_takes_value(option) && !value) {
        fprintf(stderr, "option --%s -%c: takes a value.\n",
            option->long_opt,
            option->short_opt
            );
        return OPTION_PARSE_ERROR;
    }

    // Parses the option value if necessary. Or specifies a one
    // when a flag is found in the integer value.
    switch (option->option_type) {
    case OPT_STR:
        option->value.string_value = value;
        break;
    case OPT_INT:
        {
            char trailing_garbage[2];
            ret = sscanf(value, "%d%1s", &intval, trailing_garbage);
            if (ret != 1) {
                fprintf(stderr, "option --%s -%c: expected int argument\n",
                        option->long_opt,
                        option->short_opt
                        );
                return OPTION_PARSE_ERROR;
            }
        }
        option->value.integer_value = intval;
        break;
    case OPT_FLOAT:
        ret = sscanf(value, "%lf", &floatval);
        if (ret != 1) {
            fprintf(stderr, "option --%s -%c: expected float argument\n",
                option->long_opt,
                option->short_opt
            );
            return OPTION_PARSE_ERROR;
        }
        option->value.floating_value = floatval;
        break;
    case OPT_FLAG:
        option->value.integer_value = 1;
        break;
    default:
        assert(0 == 1); // invalid value specified.
        return OPTION_WRONG_OPTION_TYPE;
    }

    return OPTION_OK;
}

int options_parse(option_context**  ppoptions,
                  int               argc,
                  const char* const* argv,
                  cmd_option*       predef_opts,
                  unsigned          nargs
                  )
{
    int i, n, ret =  OPTION_OK;
    option_context* options = NULL;

    // Check whether arguments are specified
    if (argc < 1 || !argv) {
        fprintf(stderr, "argc and argv should reflect a program name\n");
        return OPTION_INVALID_ARGUMENT;
    }

    if (*ppoptions) {
        fprintf(stderr, "parse_options: *options != NULL\n");
        return OPTION_INVALID_ARGUMENT;
    }

    // Allocate the necessary memory.
    *ppoptions = malloc(sizeof(option_context));
    if (!(*ppoptions)) {
        free(*ppoptions);
        fprintf(stderr, "parse_options: out of memory\n");
        return OPTION_OUT_OF_MEM;
    }

    options = *ppoptions;
    memset(options, 0, sizeof(option_context));

    options_add_name(options, argv[0]);

    for (i = 1; i < argc; i++) {

        const char *opt_value;
        cmd_option* option;

        /* We typically first test whether it is an long or short option
         * Then we test whether we know it. If it isn't an option. It
         * is an argument to the program.
         */
        if (is_long_opt(argv[i]) && long_opt_contains_value(argv[i])) {
            const char* opt = argv[i];
            opt += 2; // skip leading "--"
            n = find_long_option(opt, predef_opts, nargs);
            if (n < 0) {
                fprintf(stderr, "Unknown option \"%s\"\n", argv[i]);
                ret = OPTION_UNKNOWN;
                break;
            }
            option = &predef_opts[n];
            opt_value = argv[i];
            opt_value += find_equals(opt_value) + 1;
            ret = options_add_parsed_option(options, option, opt_value);
            if (ret)
                break;
        }
        else if (is_long_opt(argv[i])) {
            // skip "--"
            const char* opt_start = argv[i] + 2;
            if (i + 1 < argc)
                opt_value = argv[i + 1];
            else
                opt_value = NULL;

            n = find_long_option(opt_start, predef_opts, nargs);
            if (n < 0) {
                fprintf(stderr, "Unknown option \"%s\"", argv[i]);
                ret = OPTION_UNKNOWN;
                break;
            }
            option = &predef_opts[n];
            if (option_takes_value(option))
            {
                ret = options_add_parsed_option(options, option, opt_value);
                i++;
            }
            else {
                ret = options_add_parsed_option(options, option, NULL);
            }
        } 
        else if (is_short_opt(argv[i])) {
            // skip '-'
            const char* opt_start = argv[i] + 1;
            while (*opt_start != '\0' && *opt_start != '=') {
                char c = *opt_start;
                n = find_short_option(c, predef_opts, nargs);
                if (n < 0) {
                    fprintf(stderr, "Unknown option -%c\n", c);
                    ret = OPTION_UNKNOWN;
                    break;
                }
                option = &predef_opts[n];
                if (option_takes_value(option)) {
                    // by default the rest of the options are the argument.
                    opt_value = opt_start + 1;
                    // If there are no more characters, the next item in argv
                    // is the argument.
                    if (*opt_value == '\0') {
                        i++;
                        if (i< argc)
                            opt_value = argv[i];
                        else{
                            fprintf(stderr,
                                    "Option -%c expected an argument\n",
                                    c
                                    );
                            return OPTION_PARSE_ERROR;
                        }
                    }
                    // If the argument starts with '=', the next character is
                    // the argument.
                    else if (*opt_value == '=')
                        opt_value++;

                    ret = options_add_parsed_option(
                            options,
                            option,
                            opt_value
                            );
                    break;
                } else {
                    ret = options_add_parsed_option(
                            options,
                            option,
                            NULL
                            );
                }

                opt_start++;
            }
            
            if (ret != OPTION_OK)
                break;
        }
        else { // is an argument not an option with optional value
            ret = options_add_parsed_argument(options, argv[i]);
            if (ret != OPTION_OK)
                break;
        }
    }

    // Error parsing options clean the rubbish.
    if (ret != OPTION_OK) {
        option_context_free(options);
        *ppoptions = NULL;
    }

    return ret;
}


int option_context_str_value(
        option_context* context,
        const char*     name,
        const char**    value
        )
{
    cmd_option* option;
    assert(context && name && value);

    if (!context || !name || !value)
        return OPTION_INVALID_ARGUMENT;

    option = option_context_find_option(context, name);

    if (!option)
        return OPTION_NOT_SPECIFIED;
    else {
        if (option->option_type != OPT_STR)
            return OPTION_WRONG_OPTION_TYPE;
        *value = option->value.string_value;
    }

    return OPTION_OK;
}

int option_context_int_value(option_context* context,
                             const char*     name,
                             int*            value
                             )
{
    cmd_option* option;
    assert(context && name && value);

    if (!context || !name || !value)
        return OPTION_INVALID_ARGUMENT;

    option = option_context_find_option(context, name);

    if (!option)
        return OPTION_NOT_SPECIFIED;
    else {
        if (option->option_type != OPT_INT)
            return OPTION_WRONG_OPTION_TYPE;
        *value = option->value.integer_value;
    }

    return OPTION_OK;
}

int option_context_float_value(option_context* context,
                               const char*     name,
                               double*         value
                               )
{
    cmd_option* option;
    assert(context && name && value);

    if (!context || !name || !value)
        return OPTION_INVALID_ARGUMENT;

    option = option_context_find_option(context, name);

    if (!option)
        return OPTION_NOT_SPECIFIED;
    else {
        if (option->option_type != OPT_FLOAT)
            return OPTION_WRONG_OPTION_TYPE;
        *value = option->value.floating_value;
    }

    return OPTION_OK;
}

cmd_option* option_context_find_option(option_context* context, const char* name)
{
    int i, ret = -1;
    for (i = 0; i < context->n_options; i++) {
        if (strcmp(name, context->options[i]->long_opt) == 0) {
            ret = i;
            break;
        }
    }
    if (ret >= 0)
        return context->options[ret];
    else
        return NULL;
}

int option_context_have_option(option_context* context,
                               const char* name)
{
    assert(context && name);
    return option_context_find_option(context, name) != NULL;
}

