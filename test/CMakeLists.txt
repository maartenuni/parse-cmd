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


option (BUILD_UNIT_TEST
        "Whether or not to build the unit tests requires CUnit"
        ON
        )

if(BUILD_UNIT_TEST)
    find_library(LIB_CUNIT cunit)

    set(UNIT_TEST unit-test)
    set(UNIT_TEST_SOURCES
            unit_test.c
        )

    set(UNIT_TEST_HEADERS 
        )

    add_executable(${UNIT_TEST} ${UNIT_TEST_SOURCES} ${UNIT_TEST_HEADERS})
    target_link_libraries(${UNIT_TEST} ${PARSE_CMD_STATIC_LIB} ${LIB_CUNIT})
    include_directories(${UNIT_TEST} ${CMAKE_BINARY_DIR}/src)
endif()

