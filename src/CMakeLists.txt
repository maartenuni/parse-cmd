#
# This file is part of libparse-cmd
# 
# libparse-cmd is free software: you can redistribute it and/or modify
# it under the terms of the Lesser General Public License as published by
# the Free Software Foundation, either version 2.1 of the License, or
# (at your option) any later version.
# 
# libparse-cmd is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# Lesser General Public License for more details.
# 
# You should have received a copy of the Lesser General Public License
# along with libparse-cmd.  If not, see <http://www.gnu.org/licenses/>
#

cmake_minimum_required(VERSION 3.5)

## Create libraries

set (LIB_PARSE_CMD_SOURCES
    parse-cmd.c
    )

set (LIB_PARSE_CMD_HEADERS
    parse-cmd.h
    )

add_library(${PARSE_CMD_SHARED_LIB}
            SHARED
            ${LIB_PARSE_CMD_SOURCES}
            ${LIB_PARSE_CMD_HEADERS}
            )

add_library(${PARSE_CMD_STATIC_LIB}
            STATIC
            ${LIB_PARSE_CMD_SOURCES}
            ${LIB_PARSE_CMD_HEADERS}
            )

# Set include path (for e.g. export header)
include_directories(${PARSE_CMD_SHARED_LIB} ${CMAKE_CURRENT_BINARY_DIR})
include_directories(${PARSE_CMD_STATIC_LIB} ${CMAKE_CURRENT_BINARY_DIR})

#Make linking work for dynamic and shared libs
set_target_properties(${PARSE_CMD_SHARED_LIB} PROPERTIES
    COMPILE_FLAGS -DBUILD_LIB_PARSE_CMD_SHARED
    )
set_target_properties(${PARSE_CMD_STATIC_LIB} PROPERTIES
    COMPILE_FLAGS -DLIB_PARSE_CMD_STATIC_DEFINE
    )

#enable compiling with C99 standard
set_property(TARGET ${PARSE_CMD_SHARED_LIB} PROPERTY C_STANDARD 99)
set_property(TARGET ${PARSE_CMD_STATIC_LIB} PROPERTY C_STANDARD 99)

set_property(TARGET ${PARSE_CMD_SHARED_LIB} PROPERTY C_STANDARD_REQUIRED ON)
set_property(TARGET ${PARSE_CMD_STATIC_LIB} PROPERTY C_STANDARD_REQUIRED ON)

#Make linking work for dynamic and shared libs
generate_export_header(${PARSE_CMD_STATIC_LIB} BASE_NAME parse-cmd)

install (TARGETS ${PARSE_CMD_SHARED_LIB} LIBRARY DESTINATION lib)
install (TARGETS ${PARSE_CMD_STATIC_LIB} ARCHIVE DESTINATION lib)
install (FILES parse-cmd.h DESTINATION "${LIB_PARSE_CMD_INSTALL_HEADER_DIR}")
install (FILES "${CMAKE_CURRENT_BINARY_DIR}/parse-cmd_export.h"
         DESTINATION "${LIB_PARSE_CMD_INSTALL_HEADER_DIR}"
         )