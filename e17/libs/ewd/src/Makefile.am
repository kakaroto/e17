## Process this file with automake to produce Makefile.in

installed_headersdir = $(prefix)/include/ewd

lib_LTLIBRARIES = libewd.la
installed_headers_DATA = \
	Ewd.h \
	ewd_hash.h \
	ewd_list.h \
	ewd_macros.h \
	ewd_threads.h \
	ewd_tree.h \
	ewd_value.h \
	ewd_config.h \
	ewd_plugin.h

libewd_la_CFLAGS = -O6

libewd_la_SOURCES = \
	ewd_hash.c \
	ewd_list.c \
	ewd_tree.c \
	ewd_value.c \
	ewd_plugin.c

libewd_la_LIBADD = -lm
libewd_la_LDFLAGS = -version-info 0:0:0
