/*
 * This file is part of the parse-cmd library
 *
 * parse-cmd is free software: you can redistribute it and/or modify
 * it under the terms of the Lesser General Public License as published by
 * the Free Software Foundation, either version 2.1 of the License, or
 * (at your option) any later version.
 * 
 * parse-cmd is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the Lesser General Public License
 * along with c-lib.  If not, see <http://www.gnu.org/licenses/>
 */


#include <stdio.h>
#include <stdlib.h>
#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>
#include "../src/parse_cmd.h"

int g_verbose = 0;
int g_silent = 0;

cmd_option failure_opts[] = {
    {'c', "compiler",       OPT_STR},
    {'v', "virtual-machine",OPT_FLAG},
    {'x', "x-coor",         OPT_INT},
    {'y', "y-coor",         OPT_FLOAT}
};

size_t failure_opts_sz = sizeof(failure_opts)/sizeof(failure_opts[0]);

/*
 * This test creates the circumstances in which we expect that
 * a function of libparse-cmd fails.
 */
void failure_test()
{
    // should fail, must be null.
    option_context* options = (void*) 0x3141592654;
    int argc, ret;

    const char* args1[] = {
        "failure_test1"
    };
    argc = sizeof(args1)/sizeof(args1[0]);

    ret = options_parse(&options, argc, args1, failure_opts, failure_opts_sz);
    CU_ASSERT_EQUAL(ret, OPTION_INVALID_ARGUMENT);
    CU_ASSERT_PTR_NOT_NULL_FATAL(options);

    options = NULL;
    ret = options_parse(&options, argc, args1, failure_opts, failure_opts_sz);
    CU_ASSERT( ! option_context_have_option(options, "compiler"));
    CU_ASSERT( ! option_context_have_option(options, "virtual-machine"));
    CU_ASSERT( ! option_context_have_option(options, "x-coor"));
    CU_ASSERT( ! option_context_have_option(options, "y-coor"));
    CU_ASSERT( ! option_context_have_option(options, "some stupid value"));

    option_context_free(options);
    options = NULL;

    // oops x-coor must be int.
    const char* args2[] = {
        "failure_test2",
        "-x", "Hello, World!"
    };
    argc = sizeof(args2)/sizeof(args2[0]);
    ret = options_parse(&options, argc, args2, failure_opts, failure_opts_sz);
    CU_ASSERT_EQUAL(ret, OPTION_PARSE_ERROR);
    CU_ASSERT_PTR_NULL(options);
    
    const char* args3[] = {
        "failure_test3",
        "-x", "3.1415" //its a float not an int.
    };
    argc = sizeof(args3)/sizeof(args3[0]);
    ret = options_parse(&options, argc, args3, failure_opts, failure_opts_sz);
    CU_ASSERT_EQUAL(ret, OPTION_PARSE_ERROR);
    CU_ASSERT_PTR_NULL(options);
}

cmd_option string_opts[] = {
    {'p', "preprocessor",   OPT_STR},
    {'l', "linker",         OPT_STR},
    {'c', "compiler",       OPT_STR},
    {'v', "virtual-machine",OPT_STR},
    {'a', "another-cc",     OPT_STR}
};

size_t string_opts_sz = sizeof(string_opts)/sizeof(string_opts[0]);

/*
 * Tests whether string options seem to work.
 */
void string_test()
{
    option_context* options = NULL;
    int argc, ret;
    const char* val = NULL;
    char* compiler = "gnu C compiler";
    char* linker   = "ld";

    const char* args_combined[] = {
        "string-test",
        "-ppreprocessor",
        "--compiler", compiler,
        "-l=ld",
        "--virtual-machine=llvm",
        "-a" , "clang"
    };

    argc = sizeof(args_combined)/sizeof(args_combined[0]);

    ret = options_parse(
            &options, argc, args_combined, string_opts, string_opts_sz
            );
    CU_ASSERT(ret == OPTION_OK);
    if (ret != OPTION_OK) {
        return;
    }

    CU_ASSERT(option_context_have_option(options, "compiler"));
    CU_ASSERT(option_context_have_option(options, "preprocessor"));
    CU_ASSERT(option_context_have_option(options, "linker"));
    CU_ASSERT(option_context_have_option(options, "another-cc"));
    CU_ASSERT(option_context_have_option(options, "virtual-machine"));

    option_context_str_value(options, "compiler" , &val);
    CU_ASSERT_STRING_EQUAL(val, compiler);
    val = NULL;
    option_context_str_value(options, "linker" , &val);
    CU_ASSERT_STRING_EQUAL(val, linker);
    val = NULL;
    option_context_str_value(options, "preprocessor", &val);
    CU_ASSERT_STRING_EQUAL(val, "preprocessor");
    val = NULL;
    option_context_str_value(options, "virtual-machine", &val);
    CU_ASSERT_STRING_EQUAL(val, "llvm");
    val = NULL;
    option_context_str_value(options, "another-cc", &val);
    CU_ASSERT_STRING_EQUAL(val, "clang");
    val = NULL;

    option_context_free(options);
}

cmd_option floating_opts[] = {
    {'p', "pi",         OPT_FLOAT},
    {'e', "e",          OPT_FLOAT},
    {'a', "avogadro",   OPT_FLOAT}
};

size_t floating_opts_sz = sizeof(floating_opts)/sizeof(floating_opts[0]);

void floating_point_test()
{
    option_context* options = NULL;
    int argc, ret;
    double val;
    const double pi = 3.141592954;
    const double e  = 2.7182818;
    const double avogadro  = 6.022e23;
    const double eps = 0.000001;

    const char* args_short[] = {
        "float-test",
        "-p", "3.141592654",
        "-e", "2.7182818",
        "-a", "6.022e23"
    };

    argc = sizeof(args_short)/sizeof(args_short[0]);

    ret = options_parse(
            &options, argc, args_short, floating_opts, floating_opts_sz
            );
    CU_ASSERT(ret == OPTION_OK);
    if (ret != OPTION_OK) {
        return;
    }

    CU_ASSERT(option_context_have_option(options, "pi"));
    CU_ASSERT(option_context_have_option(options, "e"));
    CU_ASSERT(option_context_have_option(options, "avogadro"));

    option_context_float_value(options, "pi" , &val);
    CU_ASSERT_DOUBLE_EQUAL(val, pi, eps);
    option_context_float_value(options, "e" , &val);
    CU_ASSERT_DOUBLE_EQUAL(val, e, eps);
    option_context_float_value(options, "avogadro", &val);
    CU_ASSERT_DOUBLE_EQUAL(val, avogadro, eps);
    
    option_context_free(options);
    options = NULL;
    
    const char* args_long[] = {
        "float-test",
        "--pi",         "3.141592654",
        "--e",          "2.7182818",
        "--avogadro",   "6.022e23"
    };

    ret = options_parse(
            &options, argc, args_long, floating_opts, floating_opts_sz
            );

    CU_ASSERT(ret == OPTION_OK);
    if (ret != OPTION_OK) {
        return;
    }
    
    CU_ASSERT(option_context_have_option(options, "pi"));
    CU_ASSERT(option_context_have_option(options, "e"));
    CU_ASSERT(option_context_have_option(options, "avogadro"));

    option_context_float_value(options, "pi" , &val);
    CU_ASSERT_DOUBLE_EQUAL(val, pi, eps);
    option_context_float_value(options, "e" , &val);
    CU_ASSERT_DOUBLE_EQUAL(val, e, eps);
    option_context_float_value(options, "avogadro", &val);
    CU_ASSERT_DOUBLE_EQUAL(val, avogadro, eps);

    option_context_free(options);
    options = NULL;
}

cmd_option integer_opts[] = {
    {'o', "one",    OPT_INT},
    {'t', "two",    OPT_INT},
    {'T', "three",  OPT_INT}
};

size_t integer_opts_sz = sizeof(integer_opts)/sizeof(integer_opts[0]);

void integer_test()
{
    option_context* options = NULL;
    int argc, ret, val;
    const char* args_short[] = {
        "integer-test",
        "-o", "1",
        "-t", "2",
        "-T", "3"
    };

    argc = sizeof(args_short)/sizeof(args_short[0]);

    ret = options_parse(
            &options, argc, args_short, integer_opts, integer_opts_sz
            );
    CU_ASSERT(ret == OPTION_OK);
    if (ret != OPTION_OK) {
        return;
    }

    CU_ASSERT(option_context_have_option(options, "one"));
    CU_ASSERT(option_context_have_option(options, "two"));
    CU_ASSERT(option_context_have_option(options, "three"));

    option_context_int_value(options, "one" , &val);
    CU_ASSERT_EQUAL(val, 1);
    option_context_int_value(options, "two" , &val);
    CU_ASSERT_EQUAL(val, 2);
    option_context_int_value(options, "three", &val);
    CU_ASSERT_EQUAL(val, 3);
    option_context_free(options);
    options = NULL;
    
    const char* args_long[] = {
        "integer-test",
        "--one", "1",
        "--two", "2",
        "--three", "3"
    };

    ret = options_parse(
            &options, argc, args_long, integer_opts, integer_opts_sz
            );
    CU_ASSERT(ret == OPTION_OK);
    if (ret != OPTION_OK) {
        return;
    }
    
    CU_ASSERT(option_context_have_option(options, "one"));
    CU_ASSERT(option_context_have_option(options, "two"));
    CU_ASSERT(option_context_have_option(options, "three"));

    option_context_int_value(options, "one" , &val);
    option_context_int_value(options, "two" , &val);
    CU_ASSERT_EQUAL(val, 2);
    option_context_int_value(options, "three", &val);
    CU_ASSERT_EQUAL(val, 3);

    option_context_free(options);
    options = NULL;
}

void integer_combined_test()
{
    option_context* options = NULL;
    int argc, ret, val;
    const char* args_short[] = {
        "integer-test",
        "-o1",
        "-t=2",
        "-T3"
    };

    argc = sizeof(args_short)/sizeof(args_short[0]);

    ret = options_parse(
            &options, argc, args_short, integer_opts, integer_opts_sz
            );
    CU_ASSERT(ret == OPTION_OK);
    if (ret != OPTION_OK) {
        return;
    }

    CU_ASSERT(option_context_have_option(options, "one"));
    CU_ASSERT(option_context_have_option(options, "two"));
    CU_ASSERT(option_context_have_option(options, "three"));

    option_context_int_value(options, "one" , &val);
    CU_ASSERT(val == 1);
    option_context_int_value(options, "two" , &val);
    CU_ASSERT(val == 2);
    option_context_int_value(options, "three", &val);
    CU_ASSERT(val == 3);
    option_context_free(options);
    options = NULL;
    
    const char* args_long[] = {
        "integer-test",
        "--one=1",
        "--two=2",
        "--three=3"
    };

    ret = options_parse(
            &options, argc, args_long, integer_opts, integer_opts_sz
            );
    CU_ASSERT_EQUAL(ret, OPTION_OK);
    if (ret != OPTION_OK) {
        return;
    }
    
    CU_ASSERT(option_context_have_option(options, "one"));
    CU_ASSERT(option_context_have_option(options, "two"));
    CU_ASSERT(option_context_have_option(options, "three"));

    option_context_int_value(options, "one" , &val);
    CU_ASSERT_EQUAL(val, 1);
    option_context_int_value(options, "two" , &val);
    CU_ASSERT_EQUAL(val, 2);
    option_context_int_value(options, "three", &val);
    CU_ASSERT_EQUAL(val, 3);
    
    option_context_free(options);
    options = NULL;
}

void short_help_test()
{
    option_context* options = NULL;
    int argc, ret;
    char* help = NULL;
    cmd_option predef_opts[] = {
        {'i', "int",        OPT_INT,    },
        {'f', "float",      OPT_FLOAT,  },
        {'s', "string",     OPT_STR,    },
        {'b', "bool",       OPT_FLAG,   },
        {0  , "nint",       OPT_INT,    },
        {0  , "nfloat",     OPT_FLOAT,  },
        {0  , "nstring",    OPT_STR,    },
        {0  , "nbool",      OPT_FLAG,   },
    };

    size_t opt_size = sizeof(predef_opts)/sizeof(predef_opts[0]);
    
    const char* expected_output = "Usage short-help-test: "
            "[-i|--int <int>] "
            "[-f|--float <float>] "
            "[-s|--string <string>] "
            "[-b|--bool] "
            "[--nint <int>] "
            "[--nfloat <float>] "
            "[--nstring <string>] "
            "[--nbool]"
            ;

    const char* argv[] = {
        "short-help-test",
    };

    argc = sizeof(argv)/sizeof(argv[0]);

    ret = options_parse(
            &options, argc, argv, predef_opts, opt_size
            );

    CU_ASSERT(ret == OPTION_OK);
    if (ret != OPTION_OK) {
        return;
    }

    option_context_short_help(options, &help);
    CU_ASSERT_STRING_EQUAL(help, expected_output);
    if (strcmp(help, expected_output) != 0) {
        fprintf(stderr, "Got     : \"%s\"\n"
                        "expected: \"%s\"\n",
                        help,
                        expected_output
                        );
    }

    free(help);

    option_context_free(options);
    options = NULL;
}


/**
 * returns an error code of the CUnit framework
 */
int add_suites()
{
    CU_pSuite suite = CU_add_suite("parse_cmd-test", NULL, NULL);
    if (!suite) {
        fprintf(stderr,
                "unable to create test suite: %s\n",
                CU_get_error_msg()
               );
        return CU_get_error();

    }

    CU_pTest test = CU_add_test(suite, "integers", integer_test);
    if (!test) {
        fprintf(stderr,
                "unable to create integer suite: %s\n",
                CU_get_error_msg()
               );
        return CU_get_error();
    }
    
    test = CU_add_test(
            suite, "integers-arg-value-combined", integer_combined_test
            );
    if (!test) {
        fprintf(stderr,
                "unable to create integer argument and value combined suite: %s\n",
                CU_get_error_msg()
               );
        return CU_get_error();
    }
    
    test = CU_add_test(
            suite, "floatingpoints", floating_point_test
            );
    if (!test) {
        fprintf(stderr,
                "unable to create floating point suite: %s\n",
                CU_get_error_msg()
               );
        return CU_get_error();
    }
    
    test = CU_add_test(
            suite, "strings", string_test
            );
    if (!test) {
        fprintf(stderr,
                "unable to create test suite: %s\n",
                CU_get_error_msg()
               );
        return CU_get_error();
    }
    
    test = CU_add_test(
            suite, "expected-failure", failure_test 
            );
    if (!test) {
        fprintf(stderr,
                "unable to create test suite: %s\n",
                CU_get_error_msg()
               );
        return CU_get_error();
    }
    
    test = CU_add_test(
            suite, "short-help-test", short_help_test
            );
    if (!test) {
        fprintf(stderr,
                "unable to create test suite: %s\n",
                CU_get_error_msg()
               );
        return CU_get_error();
    }

    return 0;
}

/* The options to the unit test self. */
cmd_option prog_opts[] = {
    {'v', "verbose", OPT_FLAG, {0}},
    {'s', "silent" , OPT_FLAG, {0}}
};

int parse_opts(int argc, const char* const* argv) {

    int ret;
    option_context* context = NULL;

    ret = options_parse(
            &context,
            argc,
            argv,
            prog_opts,
            sizeof(prog_opts)/sizeof(prog_opts[0])
            );

    if (ret != OPTION_OK) {
        fprintf(stderr, ": unable to parse options.\n");
        return 1;
    }

    if (option_context_have_option(context, "verbose")) {
        fprintf(stderr, "verbose specified.\n");
        g_verbose = 1;
    }
    if (option_context_have_option(context, "silent")) {
        fprintf(stderr, "silent specified.\n");
        g_silent = 1;
    }

    if (g_verbose && g_silent) //cancel them.
        g_verbose = g_silent = 0;

    option_context_free(context);

    return 0;
}

int main(int argc, char** argv) {
    
    parse_opts(argc, argv);

    if(CU_initialize_registry() != CUE_SUCCESS) {
        fprintf(stderr, "Unable to initialze the unittesting frame work\n");
        return EXIT_FAILURE;
    }
    
    add_suites();

    if(g_verbose)
        CU_basic_set_mode(CU_BRM_VERBOSE);
    if(g_silent)
        CU_basic_set_mode(CU_BRM_SILENT);

    if (CU_basic_run_tests() != CUE_SUCCESS) {
        fprintf(stderr, "unable to run tests.\n");
        return EXIT_FAILURE;
    }

    CU_cleanup_registry();
}
