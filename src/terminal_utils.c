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
 * \internal
 * \file terminal_utils.c
 *
 * Implements some utilities to obtain information about the terminal.
 */

#include <stdlib.h>
#include "terminal_utils.h"
#include "parse_cmd_config.h"

#if defined (HAVE_UNISTD_H)
#include <unistd.h>
#endif

#if defined (HAVE_SYS_IOCTL_H)
#include <sys/ioctl.h>
#endif

#if defined(HAVE_IOCTL) && defined(HAVE_UNISTD_H) && defined(HAVE_SYS_IOCTL_H)
static int
get_term_width_via_ioctl()
{
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    return w.ws_col;    
}
#endif


int
get_terminal_width()
{
    int ret_width = 0;
    char* char_width = getenv("COLUMNS");
    if (char_width)
        ret_width = atoi(char_width);

    if (ret_width > 0)
        return ret_width;

#if defined(HAVE_IOCTL) && defined(HAVE_UNISTD_H) && defined(HAVE_SYS_IOCTL_H)
    ret_width = get_term_width_via_ioctl();
    if (ret_width > 0)
        return ret_width;
#endif

    return 80;
}
