## Process this file with automake to produce Makefile.in

AUTOMAKE_OPTIONS     = 1.4 foreign

MAINTAINERCLEANFILES = Makefile.in

LDFLAGS		     = 
INCLUDES	     = -I$(includedir) \
		       -I$(top_srcdir) \
		       -I$(top_srcdir)/src/lib \
		       -I$(top_srcdir)/src/lib/include \
		       @my_cflags@

lib_LTLIBRARIES      = libevoak.la
include_HEADERS      = Evoak.h
libevoak_la_SOURCES  = \
evoak_main.c \
evoak_ipc.c
evoak_protocol.c \
evoak_private.h \
evoak_protocol.h

libevoak_la_LIBADD       = $(LDFLAGS) @my_libs@
libevoak_la_DEPENDENCIES = $(top_builddir)/config.h
libevoak_la_LDFLAGS      = -version-info 0:1:0
