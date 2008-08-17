## Process this file with automake to produce Makefile.in
## Created by Anjuta - will *NOT* be overwritten

INCLUDES = -I../common

AM_CXXFLAGS = @PACKAGE_CFLAGS@ \
	 -Wall

noinst_LTLIBRARIES = libeflpp_misc.la

libeflpp_misc_la_SOURCES = \
	eflpp_housekeeping.cpp
    
libeflpp_includedir = $(pkgincludedir)

libeflpp_include_DATA = \
	eflpp_housekeeping.h

EXTRA_DIST = $(libeflpp_include_DATA)
