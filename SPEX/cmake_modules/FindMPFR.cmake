#-------------------------------------------------------------------------------
# SuiteSparse/SuiteSparse_config/cmake_modules/FindMPFR.cmake
#-------------------------------------------------------------------------------

# Copyright (c) 2022, Timothy A. Davis.  All Rights Reserved.
# SPDX-License-Identifier: BSD-3-clause

# Finds the mpfr include file and compiled library and sets:

# MPFR_INCLUDE_DIR - where to find mpfr.h
# MPFR_LIBRARY     - compiled mpfr library
# MPFR_LIBRARIES   - libraries when using mpfr
# MPFR_FOUND       - true if mpfr found

# set ``MPFR_ROOT`` to a mpfr installation root to
# tell this module where to look.

# To use this file in your application, copy this file into MyApp/cmake_modules
# where MyApp is your application and add the following to your
# MyApp/CMakeLists.txt file:
#
#   set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake_modules")
#
# or, assuming MyApp and SuiteSparse sit side-by-side in a common folder, you
# can leave this file in place and use this command (revise as needed):
#
#   set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH}
#       "${CMAKE_SOURCE_DIR}/../SuiteSparse/SuiteSparse_config/cmake_modules")

#-------------------------------------------------------------------------------

if ( DEFINED ENV{CMAKE_PREFIX_PATH} )
    # import CMAKE_PREFIX_PATH, typically created by spack
    set ( CMAKE_PREFIX_PATH $ENV{CMAKE_PREFIX_PATH} )
endif ( )

# include files for mpfr
find_path ( MPFR_INCLUDE_DIR
    NAMES mpfr.h
    PATHS MPFR_ROOT ENV MPFR_ROOT
    PATH_SUFFIXES include Include
)

# compiled libraries mpfr
find_library ( MPFR_LIBRARY
    NAMES mpfr
    PATHS MPFR_ROOT ENV MPFR_ROOT
    PATH_SUFFIXES lib build alternative
)

# get version of the library
get_filename_component ( MPFR_LIBRARY ${MPFR_LIBRARY} REALPATH )

# look in the middle for 4.1.0 (/spackstuff/mpfr-4.1.0-morestuff/libmpfr.10.4.1)
string ( REGEX MATCH "mpfr-[0-9]+.[0-9]+.[0-9]+" MPFR_VERSION1 ${MPFR_LIBRARY} )

if ( MPFR_VERSION1 STREQUAL "" )
    # mpfr has been found, but not a spack library.  Hunt for the version
    # number in mpfr.h.  The mpfr.h file includes the following line:
    #       #define MPFR_VERSION_STRING "4.0.2"
    file ( STRINGS ${MPFR_INCLUDE_DIR}/mpfr.h MPFR_VER_STRING
        REGEX "MPFR_VERSION_STRING" )
    message ( STATUS "from mpfr.h file: ${MPFR_VER_STRING}" )
    if ( MPFR_VER_STRING STREQUAL "")
        # look at the end of the filename for the version number
        string (
            REGEX MATCH "[0-9]+.[0-9]+.[0-9]+"
            MPFR_VERSION ${MPFR_LIBRARY} )
    else ( )
        # get the version number from inside the mpfr.h file itself
        string ( REGEX MATCH "[0-9]+.[0-9]+.[0-9]+" MPFR_VERSION ${MPFR_VER_STRING} )
    endif ( )
else ( )
    # look at mpfr-4.1.0 for the version number (spack library)
    string ( REGEX MATCH "[0-9]+.[0-9]+.[0-9]+" MPFR_VERSION ${MPFR_VERSION1} )
endif ( )

set ( MPFR_LIBRARIES ${MPFR_LIBRARY} )

include (FindPackageHandleStandardArgs)

find_package_handle_standard_args ( MPFR
    REQUIRED_VARS MPFR_LIBRARIES MPFR_INCLUDE_DIR
    VERSION_VAR MPFR_VERSION
)

mark_as_advanced (
    MPFR_INCLUDE_DIR
    MPFR_LIBRARY
    MPFR_LIBRARIES
)

if ( MPFR_FOUND )
    message ( STATUS "mpfr include dir: ${MPFR_INCLUDE_DIR}" )
    message ( STATUS "mpfr library:     ${MPFR_LIBRARY}" )
    message ( STATUS "mpfr version:     ${MPFR_VERSION}" )
else ( )
    message ( STATUS "mpfr not found" )
endif ( )

