# CMakeLists.txt --- CMake project settings
#    ex) cmake -G "Visual Studio 9 2008"
#    ex) cmake -DCMAKE_BUILD_TYPE=Release -G "MSYS Makefiles"
##############################################################################

# CMake minimum version
cmake_minimum_required(VERSION 2.4)

# project name and language
#project(RadWindowApp C)
project(RadWindowApp CXX)

# set program information
set(PROGRAM_MAJOR 0)
set(PROGRAM_MINOR 1)
set(PROGRAM_BUILD 0)
set(PROGRAM_NAME "RadWindowApp")
set(PROGRAM_AUTHORS "Katayama Hirofumi MZ")
set(PROGRAM_YEARS "2015-2017")
set(VERSION_STRING "${PROGRAM_MAJOR}.${PROGRAM_MINOR}.${PROGRAM_BUILD}")
set(VERSION_INFO_STRING "${PROGRAM_NAME} ${VERSION_STRING} by ${PROGRAM_AUTHORS}")

# check build type
if (NOT CMAKE_BUILD_TYPE)
    message(STATUS "No build type selected, default to Debug")
    set(CMAKE_BUILD_TYPE "Debug")
endif()

# make uppercase string of build type
string(TOUPPER "${CMAKE_BUILD_TYPE}" uppercase_CMAKE_BUILD_TYPE)

# check build type again
if (NOT uppercase_CMAKE_BUILD_TYPE MATCHES "^(DEBUG|RELEASE|RELWITHDEBINFO|MINSIZEREL)$")
    message(FATAL_ERROR "Invalid value for CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}")
endif()

# define _DEBUG on Debug version
if (uppercase_CMAKE_BUILD_TYPE STREQUAL "DEBUG" OR uppercase_CMAKE_BUILD_TYPE STREQUAL "RELWITHDEBINFO")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_DEBUG")
endif()

# set output directory (build/)
set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/build)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR})

# set source files
set(SOURCES src/RadWindowApp.cpp)

# Win32 or not?
if (CMAKE_HOST_WIN32)
    # enable Win32 resource
    ENABLE_LANGUAGE(RC)

    # Win32
    add_executable(RadWindowApp WIN32 ${SOURCES} src/RadWindowApp_res.rc)
else()
    message(FATAL_ERROR "To build this project, use Win32 C++ compiler")
endif()

# do statically link
set_target_properties(RadWindowApp PROPERTIES LINK_DEPENDS_NO_SHARED 1)
set_target_properties(RadWindowApp PROPERTIES LINK_SEARCH_START_STATIC 1)
set_target_properties(RadWindowApp PROPERTIES LINK_SEARCH_END_STATIC 1)

set(CMAKE_FIND_LIBRARY_SUFFIXES ".a")
SET(BUILD_SHARED_LIBRARIES OFF)
if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    # using Clang
    SET(CMAKE_EXE_LINKER_FLAGS "-static -static-libgcc -static-libstdc++")
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    # using GCC
    SET(CMAKE_EXE_LINKER_FLAGS "-static -static-libgcc -static-libstdc++")
endif()

# replace "/MD" with "/MT" (building without runtime DLLs)
if(1)
    set(CompilerFlags
        CMAKE_C_FLAGS
        CMAKE_C_FLAGS_DEBUG
        CMAKE_C_FLAGS_RELEASE
        CMAKE_C_FLAGS_RELWITHDEBINFO
        CMAKE_CXX_FLAGS
        CMAKE_CXX_FLAGS_DEBUG
        CMAKE_CXX_FLAGS_RELEASE
        CMAKE_CXX_FLAGS_RELWITHDEBINFO)
    foreach(CompilerFlags ${CompilerFlags})
        string(REPLACE "/MD" "/MT" ${CompilerFlags} "${${CompilerFlags}}")
    endforeach()
endif()

# configuration files
configure_file(src/config.h.in src/config.h)

# install the program
install(TARGETS RadWindowApp DESTINATION bin)

##############################################################################
