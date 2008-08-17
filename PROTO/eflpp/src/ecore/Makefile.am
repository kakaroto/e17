## Process this file with automake to produce Makefile.in
## Created by Anjuta - will *NOT* be overwritten

INCLUDES = -I../common -I../evas

AM_CXXFLAGS = @PACKAGE_CFLAGS@ \
	 -Wall

noinst_LTLIBRARIES = libeflpp_ecore.la

libeflpp_ecore_la_SOURCES = \
	eflpp_ecoreconfig.cpp\
	eflpp_ecore.cpp \
	eflpp_ecore_window.cpp \
	eflpp_ecore_x_window.cpp \
	eflpp_ecore_data.cpp
    
libeflpp_includedir = $(pkgincludedir)

libeflpp_include_DATA = \
	eflpp_ecoreconfig.h\
	eflpp_ecore.h \
	eflpp_ecore_window.h \
	eflpp_ecore_x_window.h \
	eflpp_ecore_data.h

EXTRA_DIST = $(libeflpp_include_DATA)
