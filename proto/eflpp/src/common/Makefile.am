## Process this file with automake to produce Makefile.in
## Created by Anjuta - will *NOT* be overwritten

INCLUDES =

AM_CXXFLAGS = @PACKAGE_CFLAGS@ \
	 -Wall

noinst_LTLIBRARIES = libeflpp_common.la

libeflpp_common_la_SOURCES = \
	eflpp_debug.cpp
    
libeflpp_includedir = $(pkgincludedir)

libeflpp_include_DATA = \
	eflpp_common.h\
	eflpp_debug.h\
	eflpp_debug_internal.h\
	eflpp_sys.h

EXTRA_DIST = $(libeflpp_include_DATA)
