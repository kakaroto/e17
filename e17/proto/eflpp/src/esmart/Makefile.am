## Process this file with automake to produce Makefile.in
## Created by Anjuta - will *NOT* be overwritten

INCLUDES = -I../common -I../evas -I../edje

AM_CXXFLAGS = @PACKAGE_CFLAGS@ \
	 -Wall

noinst_LTLIBRARIES = libeflpp_esmart.la

libeflpp_esmart_la_SOURCES = \
	eflpp_esmart.cpp \
	eflpp_esmart_container.cpp \
	eflpp_esmart_textentry.cpp \
	eflpp_esmart_group.cpp
    
libeflpp_includedir = $(pkgincludedir)

libeflpp_include_DATA = \
	eflpp_esmart.h \
	eflpp_esmart_container.h \
	eflpp_esmart_textentry.h \
	eflpp_esmart_group.h

EXTRA_DIST = $(libeflpp_include_DATA)
