## Process this file with automake to produce Makefile.in

installed_headersdir = $(prefix)/include/ewd

lib_LTLIBRARIES = libewd.la
installed_headers_DATA = \
	Ewd.h \
	ewd_list.h \
	ewd_dlist.h \
	ewd_list_private.h \
	ewd_tree.h \
	ewd_value.h \
	ewd_threads.h \
	ewd_macros.h

libewd_la_SOURCES = \
	ewd_value.c \
	ewd_list.c \
	ewd_dlist.c \
	ewd_tree.c

libewd_la_LIBADD = -lm
libewd_la_LDFLAGS = -version-info 0:0:0
