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
 * \file parse_cmd.h
 */

#ifndef PARSE_CMD_H
#define PARSE_CMD_H

#include "parse_cmd_export.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Determines what kind of parameter the option expects.
 */
enum OPTION_VALUE_TYPES {
    OPT_STR,    ///< Option value is a string (requires argument).
    OPT_INT,    ///< Option value is an int (requires argument).
    OPT_FLOAT,  ///< Option value is a floating point number (req arg).
    OPT_FLAG    ///< Option is use as a flag don't specify a argument.
};

/**
 * Possible outcomes of an operation. OPTION_OK tells that the operation
 * was successful, otherwise it indicates an error.
 */
enum OPTION_RET_VAL {
    OPTION_OK = 0,           ///< Operation was successful
    OPTION_OUT_OF_MEM,       ///< Operation didn't succeed due memory shortage.
    OPTION_WRONG_OPTION_TYPE,///< Unable to understand option value.
    OPTION_WITHOUT_VALUE,    ///< Value specified by a flag.
    OPTION_PARSE_ERROR,      ///< Unable to parse the command line arguments
    OPTION_INVALID_ARGUMENT, ///< An invalid argument to a function was specified.
    OPTION_UNKNOWN,          ///< An unknown option encountered.
    OPTION_NOT_SPECIFIED     ///< asking for the value of a unspecified option.
};

/**
 * reserves space for string, integer or double precision floating point
 * values. OPTION_VALUE_TYPES enum determines which of the three should
 * be valid.
 */
typedef union option_value {
    const char* string_value;   ///< String value for an option
    int         integer_value;  ///< Integral value for an option.
    double      floating_value; ///< the floating point value for an option.
}option_value;

/**
 * cmd_option describes how an command line option or value is expected.
 */
typedef struct cmd_option {
    char            short_opt;  ///< The short option variant.
    const char*     long_opt;   ///< The long option variant.
    int             option_type;///< This determines how the option
                                //  argument should be interpreted.
    option_value    value;      ///< The actual value specified.
    const char*     help;       ///< Describes the option.
} cmd_option;

/// typedef for struct option_context
typedef struct option_context option_context;

/**
 * Parses the command line.
 *
 * This function parses the command line and add found option with their
 * values to the context. The options will be initialized when
 * this function returns OPTION_OK.
 *
 * \param[in,out] options The context to initialize. options can't be
 *                        NULL and *options must be NULL.
 * \param[in]     argc    Matches argc of main function.
 * \param[in]     argv    Matches argv of main function.
 * \param[in]     predef_opts This is an array to pre defined
 *                        options that the program specifies. This
 *                        array defines the options the program knows
 *                        about and what type of options there are.
 *                        It is possible to specify a default value
 *                        for the option values here.
 * \param [in]    nargs   The number of predefined options.
 *
 * \returns OPTION_OK when successful.
 */
PARSE_CMD_EXPORT int
options_parse(
        option_context** options,
        int argc,
        const char* const* argv,
        cmd_option* predef_opts,
        unsigned nargs
        );

/**
 * Frees an option context and its allocated resources.
 */
PARSE_CMD_EXPORT void
option_context_free(option_context* options);

/**
 * Finds and returns an options specified at the command line.
 *
 * \returns A pointer to the option or NULL if it wasn't found.
 */
PARSE_CMD_EXPORT cmd_option*
option_context_find_option(option_context* option, const char* name);

/**
 * Returns whether a option was specified at the command line.
 *
 * \param[in] options the option context.
 * \param[in] name the name of the long option without "--" prefix.
 * 
 * \return non zero value when option was found.
 */
PARSE_CMD_EXPORT int
option_context_have_option(
        option_context* options,
        const char*     name
        );

/**
 * Obtain a string value specified at the command line.
 * 
 * \param[in]   options the option_context.
 * \param[in]   opt_name specify the name of the option without "-"or "--".
 * \param[out]  opt_value the value of the string will be returned here.
 *
 * \returns OPTION_OK when successful or another OPTION_RET_VAL when
 *          it is not successful.
 */
PARSE_CMD_EXPORT int
option_context_str_value(
        option_context* options,
        const char*     opt_name,
        const char**    opt_value
        );

/**
 * Obtain an integer value specified at the command line.
 *
 * \param[in]   options the option_context.
 * \param[in]   opt_name specify the name of the option without "-"or "--".
 * \param[out]  opt_value The value of the integer will be returned here.
 *
 * \returns OPTION_OK when successful or another OPTION_RET_VAL when
 *          it is not successful.
 */
PARSE_CMD_EXPORT int
option_context_int_value(
        option_context* options,
        const char*     opt_name,
        int*            opt_value
        );

/**
 * Obtain a floating point value specified at the command line.
 *
 * \param[in]   options     the option_context.
 * \param[in]   opt_name    specify the name of the option without
 *                          to retrieve "-"or "--".
 * \param[out]  opt_value   The value of the float will be returned here.
 *
 * \returns OPTION_OK when successful or another OPTION_RET_VAL when
 *          it is not successful.
 */
PARSE_CMD_EXPORT int
option_context_float_value(
        option_context* options,
        const char*     opt_name,
        double*         opt_value
        );

/**
 * Get the number of non option arguments
 */
PARSE_CMD_EXPORT int
option_context_nargs(const option_context* options);

/**
 * Get the number of options that are specified on the command line
 *
 * @param [in] options The parsed option context.
 * 
 * @returns the number of defined/applicable options. Or -1
 *          when options == NULL
 */
PARSE_CMD_EXPORT int
option_context_num_options(const option_context* options);

/**
 * Get the number of program defined options.
 *
 * @param [in] options The option context as returned by options_parse
 * @returns The number of program predefined options.
 */
PARSE_CMD_EXPORT int
option_context_num_predef_options(const option_context* options);

/**
 * Obtain a pointer to the predefined options
 */
PARSE_CMD_EXPORT const cmd_option*
option_context_get_predef_options(const option_context* options);

/**
 * Get the nth argument that was specified.
 *
 * @param [in] options an initialized option context
 * @param [in] nth retrieve the nth_argument
 *
 * \returns a pointer to an argument or NULL when it is not available.
 */
PARSE_CMD_EXPORT const char*
option_context_get_argument(const option_context* options, int nth);

/**
 * Retrieve the program name
 *
 * @param [in]  options the parsed option context.
 *
 * @returns The name of the program.
 */
PARSE_CMD_EXPORT const char* 
option_context_prog_name(const option_context* options);

/**
 * Set the program description
 *
 * The program description will be used when generating the help from the
 * input options.
 */
PARSE_CMD_EXPORT void
option_context_set_description(option_context* options,
                               const char* description);

/**
 * Obtains the program description
 */
PARSE_CMD_EXPORT const char*
option_context_get_description(const option_context* options);

/**
 * Get a short help for the that describes the options that can be given
 * to the program.
 *
 * @param [in]  options the option context to describe
 * @param [out] help a pointer to a char* that should be NULL, if successful
 *              a string describing the help should be return to *help. This
 *              string is owned by the caller and hence should be freed after
 *              use.
 */
PARSE_CMD_EXPORT int
option_context_short_help(const option_context* options, char** help);

/**
 * Get a long help for the that describes the program.
 *
 * @param [in]  options  the option context to describe
 * @param [out] help  a pointer to a char* that should be NULL, if successful
 *              a string describing the help should be return to *help. This
 *              string is owned by the caller and hence should be freed after
 *              use.
 * @return      OPTION_OK if successful another OPTION_x when not.
 */
PARSE_CMD_EXPORT int
option_context_help(const option_context* options, char**help);

#ifdef __cplusplus
}
#endif

#endif // !PARSE_CMD_H

