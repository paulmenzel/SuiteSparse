#-------------------------------------------------------------------------------
# SuiteSparse/BTF/Makefile
#-------------------------------------------------------------------------------

# BTF: Copyright (c) 2004-2022, University of Florida.  All Rights Reserved.
# Authors: Timothy A. Davis and Ekanathan Palamadai.
# SPDX-License-Identifier: LGPL-2.1+

#-------------------------------------------------------------------------------

# A simple Makefile for BTF, which relies on cmake to do the
# actual build.  All the work is done in cmake so this Makefile is just for
# convenience.

# To compile with an alternate compiler:
#
#       make CC=gcc CXX=g++
#
# To compile/install for system-wide usage:
#
#       make
#       sudo make install
#
# To compile/install for local usage (SuiteSparse/lib and SuiteSparse/include):
#
#       make local
#       make install
#
# To clean up the files:
#
#       make clean

JOBS ?= 8

default: library

# default is to install only in /usr/local
library:
	( cd build && cmake $(CMAKE_OPTIONS) .. && $(MAKE) --jobs=${JOBS} )

# install only in SuiteSparse/lib and SuiteSparse/include
local:
	( cd build && cmake $(CMAKE_OPTIONS) -DGLOBAL_INSTALL=0 -DLOCAL_INSTALL=1 .. && $(MAKE) --jobs=${JOBS} )

# install only in /usr/local (default)
global:
	( cd build && cmake $(CMAKE_OPTIONS) -DGLOBAL_INSTALL=1 -DLOCAL_INSTALL=0 .. && $(MAKE) --jobs=${JOBS} )

# install in SuiteSparse/lib both and SuiteSparse/include and /usr/local
both:
	( cd build && cmake $(CMAKE_OPTIONS) -DGLOBAL_INSTALL=1 -DLOCAL_INSTALL=1 .. && $(MAKE) --jobs=${JOBS} )

debug:
	( cd build && cmake $(CMAKE_OPTIONS) -DCMAKE_BUILD_TYPE=Debug .. && $(MAKE) --jobs=${JOBS} )

all: library

# there is no BTF demo
demos: library

# just compile after running cmake; do not run cmake again
remake:
	( cd build && $(MAKE) --jobs=${JOBS} )

# just run cmake to set things up
setup:
	( cd build && cmake $(CMAKE_OPTIONS) .. )

install:
	( cd build && $(MAKE) install )

# remove any installed libraries and #include files
uninstall:
	- xargs rm < build/install_manifest.txt

# remove all files not in the distribution
clean:
	- $(RM) -rf build/* Config/*.tmp MATLAB/*.o MATLAB/*.mex*

purge: clean

distclean: clean

docs:


