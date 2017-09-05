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

cmake_minimum_required(VERSION 3.5)
project (parse-cmd C CXX)

include (CheckIncludeFiles)
include (GenerateExportHeader)

#set versioning variables
set (LIB_PARSE_CMD_VERSION_MAJOR 0)
set (LIB_PARSE_CMD_VERSION_MINOR 0)
set (LIB_PARSE_CMD_VERSION_MICRO 0)
set (LIB_PARSE_CMD_VERSION_STRING
    "\"libparse-cmd-${LIB_PARSE_CMD_VERSION_MAJOR}.\""
    "\"${LIB_PARSE_CMD_VERSION_MAJOR}.${LIB_PARSE_CMD_VERSION_MICRO}\""
    )
set (LIB_PARSE_CMD_INSTALL_HEADER_DIR "include/parse-cmd-${LIB_PARSE_CMD_VERSION_MAJOR}")

#set names for the targets
set (PARSE_CMD_SHARED_LIB
        parse-cmd-${LIB_PARSE_CMD_VERSION_MAJOR}_${LIB_PARSE_CMD_VERSION_MINOR}
        )
set (PARSE_CMD_STATIC_LIB
        parse-cmd-${LIB_PARSE_CMD_VERSION_MAJOR}_${LIB_PARSE_CMD_VERSION_MINOR}-static
        )

#Check for expected headers
include(CheckIncludeFiles)
CHECK_INCLUDE_FILES(stdlib.h HAVE_STDLIB_H)
CHECK_INCLUDE_FILES(string.h HAVE_STRING_H)
CHECK_INCLUDE_FILES(assert.h HAVE_ASSERT_H)
CHECK_INCLUDE_FILES(stdio.h  HAVE_STDIO_H)

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

set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Maarten Duijndam")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Helps parsing command line options.")
set(CPACK_PACKAGE_DESCRIPTION         "Helps parsing command line options.")
option(BUILD_DPKG "Whether or not add support for creating debian packages" TRUE)

if(BUILD_DPKG)
    set(CPACK_GENERATOR "DEB")
endif()

include (CPack)

add_subdirectory(src)