## Process this file with automake to produce Makefile.in
## Created by Anjuta - will *NOT* be overwritten

INCLUDES = -I../common -I../ecore

AM_CXXFLAGS = @PACKAGE_CFLAGS@ \
	 -Wall

noinst_LTLIBRARIES = libeflpp_evas.la

libeflpp_evas_la_SOURCES = \
	eflpp_evas.cpp\
	eflpp_evasfont.cpp
    
libeflpp_includedir = $(pkgincludedir)

libeflpp_include_DATA = \
	eflpp_evasevent.h\
	eflpp_evasfont.h\
	eflpp_evas.h\
	eflpp_evasutils.h

EXTRA_DIST = $(libeflpp_include_DATA)
