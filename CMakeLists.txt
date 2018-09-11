#
# This file is part of parse-cmd library
# 
# parse-cmd library is free software: you can redistribute it and/or modify
# it under the terms of the Lesser General Public License as published by
# the Free Software Foundation, either version 2.1 of the License, or
# (at your option) any later version.
# 
# The parse-cmd library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# Lesser General Public License for more details.
# 
# You should have received a copy of the Lesser General Public License
# along with libparse-cmd.  If not, see <http://www.gnu.org/licenses/>
#

cmake_minimum_required(VERSION 2.8)
project (parse_cmd C CXX)

include (CheckIncludeFiles)
include (CheckFunctionExists)
include (GenerateExportHeader)

#set versioning variables
set (PARSE_CMD_VERSION_MAJOR 0)
set (PARSE_CMD_VERSION_MINOR 0)
set (PARSE_CMD_VERSION_MICRO 0)
set (VERSION "${PARSE_CMD_VERSION_MAJOR}.${PARSE_CMD_VERSION_MINOR}.${PARSE_CMD_VERSION_MICRO}")
set (PARSE_CMD_VERSION "parse_cmd_${VERSION}")
set (PARSE_CMD_VERSION_STRING
    "\"${PARSE_CMD_VERSION}\""
    )
set (PARSE_CMD_INSTALL_HEADER_DIR
    "include/parse_cmd_${PARSE_CMD_VERSION_MAJOR}_${PARSE_CMD_VERSION_MINOR}"
    )

#set names for the targets
set (PARSE_CMD_SHARED_LIB
        parse_cmd_${PARSE_CMD_VERSION_MAJOR}_${PARSE_CMD_VERSION_MINOR}
        )
set (PARSE_CMD_STATIC_LIB
        parse_cmd_${PARSE_CMD_VERSION_MAJOR}_${PARSE_CMD_VERSION_MINOR}_static
        )

#Check for expected headers
CHECK_INCLUDE_FILES(stdlib.h        HAVE_STDLIB_H)
CHECK_INCLUDE_FILES(string.h        HAVE_STRING_H)
CHECK_INCLUDE_FILES(assert.h        HAVE_ASSERT_H)
CHECK_INCLUDE_FILES(stdio.h         HAVE_STDIO_H)
CHECK_INCLUDE_FILES(wchar.h         HAVE_WCHAR_H)
CHECK_INCLUDE_FILES(unistd.h        HAVE_UNISTD_H)
CHECK_INCLUDE_FILES("sys/ioctl.h"   HAVE_SYS_IOCTL_H)

#check for whether we have certain functios
check_function_exists(ioctl HAVE_IOCTL)

#Add compilation with warnings
if(MSVC)
    if (CMAKE_C_FLAGS MATCHES "/w[0-4]")
        string(REGEX REPLACE "/w[0-4]]" "/W4")
    else()
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /W4")
    endif()
else()
    #enable C99
    #this assumes the compiler know about -Wall -pedantic -std=c99 flags
   set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -pedantic -std=c99")
endif()

set(CPACK_DEBIAN_PACKAGE_MAINTAINER   "Maarten Duijndam")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Helps parsing command line options.")
set(CPACK_PACKAGE_DESCRIPTION         "Helps parsing command line options.")
set(CPACK_PACKAGE_NAME                "parse_cmd")
set(CPACK_PACKAGE_VERSION_MAJOR      ${PARSE_CMD_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR      ${PARSE_CMD_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH      ${PARSE_CMD_VERSION_MICRO})

option(BUILD_DPKG "Whether or not add support for creating debian packages" TRUE)
if(BUILD_DPKG)
    set(CPACK_GENERATOR "DEB")
endif()

set(INSTALL_PKGCONFIG_DIR "${CMAKE_INSTALL_PREFIX}/share/pkgconfig" CACHE PATH "Installation directory for pkgconfig (.pc) files")

add_subdirectory(src)
add_subdirectory(test)

include (CPack)
option(CREATE_PC_FILE "Add support for pkg-config" TRUE)
if (CREATE_PC_FILE)
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/parse_cmd.pc.in
                   ${CMAKE_CURRENT_BINARY_DIR}/parse_cmd.pc
                   @ONLY
                   )
    install (FILES "${CMAKE_CURRENT_BINARY_DIR}/parse_cmd.pc"
             DESTINATION "share/pkgconfig"
             )
endif()
