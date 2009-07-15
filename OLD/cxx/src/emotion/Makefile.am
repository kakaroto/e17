## Process this file with automake to produce Makefile.in
## Created by Anjuta - will *NOT* be overwritten

INCLUDES = -I../common -I../evas

AM_CXXFLAGS = @PACKAGE_CFLAGS@ \
	 -Wall

noinst_LTLIBRARIES = libeflpp_emotion.la

libeflpp_emotion_la_SOURCES = \
	eflpp_emotion.cpp
    
libeflpp_includedir = $(pkgincludedir)

libeflpp_include_DATA = \
	eflpp_emotion.h

EXTRA_DIST = $(libeflpp_include_DATA)
