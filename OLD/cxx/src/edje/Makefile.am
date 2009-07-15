## Process this file with automake to produce Makefile.in
## Created by Anjuta - will *NOT* be overwritten

INCLUDES = -I../common -I../evas

AM_CXXFLAGS = @PACKAGE_CFLAGS@ \
	 -Wall

noinst_LTLIBRARIES = libeflpp_edje.la

libeflpp_edje_la_SOURCES = \
	eflpp_edje.cpp\
	eflpp_edjepart.cpp\
	eflpp_evasedje.cpp
    
libeflpp_includedir = $(pkgincludedir)

libeflpp_include_DATA = \
	eflpp_edje.h\
	eflpp_edjepart.h\
	eflpp_evasedje.h

EXTRA_DIST = $(libeflpp_include_DATA)
