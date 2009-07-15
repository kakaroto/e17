## Process this file with automake to produce Makefile.in
## Created by Anjuta - will *NOT* be overwritten

INCLUDES = -I../common -I../evas

AM_CXXFLAGS = @PACKAGE_CFLAGS@ \
	 -Wall

noinst_LTLIBRARIES = libeflpp_etk.la

libeflpp_etk_la_SOURCES = \
	eflpp_etk.cpp\
	eflpp_etkimage.cpp\
	eflpp_etkcontainer.cpp\
	eflpp_etkwidget.cpp\
	eflpp_etkobject.cpp\
	eflpp_etkbox.cpp\
	eflpp_etkbutton.cpp\
	eflpp_etkapplication.cpp\
	eflpp_etktoplevel.cpp\
	eflpp_etkwindow.cpp
    
libeflpp_includedir = $(pkgincludedir)

libeflpp_include_DATA = \
	eflpp_etk.h\
	eflpp_etkimage.h\
	eflpp_etkcontainer.h\
	eflpp_etkwidget.h\
	eflpp_etkobject.h\
	eflpp_etkbox.h\
	eflpp_etkbutton.h\
	eflpp_etkapplication.h\
	eflpp_etktoplevel.h\
	eflpp_etkwindow.h

EXTRA_DIST = $(libeflpp_include_DATA)
