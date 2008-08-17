## Process this file with automake to produce Makefile.in

AUTOMAKE_OPTIONS     = 1.4 foreign

MAINTAINERCLEANFILES = Makefile.in

INCLUDES	     = -I$(top_srcdir) \
		       -I$(top_srcdir)/src/lib \
		       @my_cflags@

lib_LTLIBRARIES      = libevoak.la
include_HEADERS      = Evoak.h
libevoak_la_SOURCES  = \
evoak_main.c \
evoak_protocol.c \
evoak_private.h \
evoak_protocol.h

libevoak_la_LIBADD       = @my_libs@
libevoak_la_DEPENDENCIES = $(top_builddir)/config.h
libevoak_la_LDFLAGS      = -version-info 0:1:0
