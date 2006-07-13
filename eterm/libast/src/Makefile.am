# $Id: Makefile.am,v 1.6 2001/09/22 16:25:29 mej Exp $

lib_LTLIBRARIES = libast.la

INCLUDES = -I$(top_srcdir)/include -I$(top_srcdir)/include/$(PACKAGE)
AM_CFLAGS = $(PTHREAD_CFLAGS)
AM_LDFLAGS = $(PTHREAD_LIBS)

libast_la_SOURCES = array.c builtin_hashes.c conf.c debug.c		\
dlinked_list.c file.c linked_list.c mbuff.c mem.c module.c msgs.c	\
obj.c objpair.c options.c pthreads.c regexp.c socket.c str.c strings.c	\
snprintf.c tok.c url.c ustr.c

libast_la_LDFLAGS = -version-info 2:2:0
MAINTAINERCLEANFILES = Makefile.in
