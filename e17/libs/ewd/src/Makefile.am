## Process this file with automake to produce Makefile.in

installed_headersdir = $(prefix)/include/ewd

lib_LTLIBRARIES = libewd.la
installed_headers_DATA = \
	Ewd.h \
	ewd_list.h \
	ewd_macros.h

libewd_la_SOURCES = \
	ewd_list.c \

libewd_la_LIBADD = -lm
libewd_la_LDFLAGS = -version-info 0:0:0
