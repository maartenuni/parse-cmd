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

#ifndef TERMINAL_UTILS_H
#define TERMINAL_UTILS_H

/**
 * \internal
 * \file terminal_utils.h
 *
 * This file contains some private utilities in order to obtain some
 * information about the terminal we are outputting to.
 */

/**
 * \internal
 * Try to deduce the terminal size.
 *
 * \returns the obtained width of the terminal. If it can't be obtained
 *          a default of 80 is assumed.
 */
int get_terminal_width();

#endif
